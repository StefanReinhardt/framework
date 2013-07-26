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
	m_visualizer->geometry( std::make_shared<gl::Geometry>( createQuad() ), m_shader );


}


void CloudRenderer::render(gl::Context* context)
{
	Camera cam( math::degToRad(45.0f), context->getViewportAspect() );
	context->setView( cam.m_worldToView, cam.m_viewToWorld, cam.m_viewToNDC );

	glDisable( GL_DEPTH_TEST );

	CloudData::Ptr cd;
	if( m_src )
		cd = m_src->getData<CloudData>();


	if( cd )
	{
		qDebug() << "RENDERING CLOUDS! - got CD";
		math::V3i res = cd->getResolution();

		//cd->getSubData<ScalarField>("qv")->fill( 1.0f );
		//m_tex_qc->uploadFloat32( res.x, res.y, cd->getSubData<ScalarField>("qt")->getRawPointer() );

		/*
		float* pixels = (float*) malloc( res.x*res.y*sizeof(float) );
		float *ptr = pixels;
		for( int j=0;j<res.y;++j )
			for( int i=0;i<res.x;++i, ++ptr )
			{
				ptr[0] = i/float(res.x);
				//ptr[0] = 1.0f;
			}
		memcpy( cd->getSubData<ScalarField>("qv")->getRawPointer(), pixels, res.x*res.y*sizeof(float) );
		free(pixels);
		*/

		m_tex_qc->uploadFloat32( res.x, res.y, cd->getSubData<ScalarField>("qc")->getRawPointer() );
		m_visualizer->render(context);
	}

}

