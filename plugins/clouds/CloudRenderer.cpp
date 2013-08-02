#include "CloudRenderer.h"
#include <sstream>

CloudRenderer::CloudRenderer() : gl::Renderer()
{
	m_visualizer = std::make_shared<gl::Visualizer>();
}

CloudRenderer::CloudRenderer( core::GraphNodeSocket::Ptr src ) : gl::Renderer(), m_src(src)
{
	m_tex_qc = gl::Texture2d::createFloat32();
	//m_shader = gl::Shader::createSimpleTextureShader( m_tex_qc );


	// cloud shader ----
	{
		unsigned char vertexShaderSource[] =
			"#version 400 core\n"
			"in vec3 P;\n"
			"in vec2 UV;\n"
			"out vec2 uv;\n"
			"uniform mat4 mvpm;\n"
			"void main(void)\n"
			"{\n"
			"\tuv = UV;\n"
			"\tgl_Position = mvpm * vec4(P,1.0);\n"
			"}\n";
		std::stringstream vs_src;
		vs_src << vertexShaderSource;


		unsigned char pixelShaderSource[] =
			"#version 400 core\n"
			"in vec2 uv;\n"
			"out vec4 color;\n"
			"uniform sampler2D color_texture;\n"
			"uniform float rng_min;\n"
			"uniform float rng_max;\n"
			"void main()\n"
			"{\n"
			"\tfloat v = texture(color_texture, uv).x;\n"
			"\tv = (v-rng_min) / (rng_max - rng_min);\n"
			"\tcolor = vec4( v, v, v, 1.0 );\n"
			"}\n";

		std::stringstream ps_src;
		ps_src << pixelShaderSource;


		gl::Shader::ShaderLoader sl = gl::Shader::create("simple texture");
		sl.attachVS( vs_src.str() );
		sl.attachPS( ps_src.str() );
		m_shader = sl;
		m_shader->setUniform( "color_texture", m_tex_qc );
		//m_shader->setUniform( "color_texture", gl::Texture2d::createUVRefTexture() );
		m_shader->setUniform( "rng_min", 0.0f );
		m_shader->setUniform( "rng_max", 0.005f );
		//m_shader->setUniform( "rng_max", 1.0f );
	}

	m_camera = std::make_shared<Camera>();

	m_visualizer = std::make_shared<gl::Visualizer>();
	m_visualizer->setPrimitiveName( m_visualizer->geometry( std::make_shared<gl::Geometry>( createQuad() ), m_shader ), "qc");


	/*
	gl::Visualizer::Points::Ptr points = m_visualizer->points();
	m_visualizer->setPrimitiveName( points, "test");
	points->add( math::V3f(0.0f, 0.0f, -1.0f), math::V3f(1.0f, 0.0f, 0.0f) );
	points->setPointSize(10.0f);
	*/
}


void CloudRenderer::render(gl::Context* context)
{
	// clear screen ---
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f ); // Let OpenGL clear to black
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// setup viewport ---
	Camera cam( math::degToRad(45.0f), context->getViewportAspect() );
	cam.setViewToWorld( math::M44f::TranslationMatrix(0.0f, 0.0f, 1.8f) );
	context->setView( cam.m_worldToView, cam.m_viewToWorld, cam.m_viewToNDC );

	// render cloud data ---
	glDisable( GL_DEPTH_TEST );

	CloudData::Ptr cd;
	if( m_src )
		cd = m_src->getData<CloudData>();


	if( cd )
	{
		qDebug() << "RENDERING CLOUDS! - got CD";
		math::V3i res = cd->getResolution();

		// render quad with qc ---
		m_tex_qc->uploadFloat32( res.x, res.y, cd->getSubData<ScalarField>("qc")->getRawPointer() );

		// render balloon ---
		if( cd->m_info.contains("balloon_x") )
		{
			float b_qv, b_qc, b_pt;

			// get balloon position in local space
			math::V3f b_vsP( cd->m_info["balloon_x"].toFloat(), cd->m_info["balloon_y"].toFloat(), 0.5f );
			qDebug() << "!!!!!!!!!!!!!! " << b_vsP.x << " " << b_vsP.y << " " << b_vsP.z;
			math::V3f b_lsP = cd->getSubData<ScalarField>("qc")->voxelToLocal(b_vsP);
			qDebug() << "!!!!!!!!!!!!!! " << b_lsP.x << " " << b_lsP.y << " " << b_lsP.z;
			math::V3f b_P = b_lsP - math::V3f(0.5f, 0.5f, 0.0f);
			b_P.z = 0.0f;
			qDebug() << "!!!!!!!!!!!!!! " << b_P.x << " " << b_P.y << " " << b_P.z;
			gl::Visualizer::Points::Ptr points = std::dynamic_pointer_cast<gl::Visualizer::Points>(m_visualizer->getPrimitive("balloon"));
			if( !points )
			{

				points = m_visualizer->points();
				m_visualizer->setPrimitiveName( points, "balloon");
				points->add( math::V3f(0.0f, 0.0f, -1.0f), math::V3f(1.0f, 0.0f, 0.0f) );
				points->setPointSize(10.0f);
			}

			//points->setPosition(0, math::V3f(0.0f, -0.5f, 0.0f));
			points->setPosition(0, b_P);
			//points->setPosition(0, math::V3f(.5f, 0.5f, 0.0f));
			//qDebug() << "!!!!!!!!!!!!!! " << b_lsP.x << " " << b_lsP.y << " " << b_lsP.z;
		}
		//m_visualizer->getPrimitive("qc")->hide();

		m_visualizer->render(context);
	}

}

