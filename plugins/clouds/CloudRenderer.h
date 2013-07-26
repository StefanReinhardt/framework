#pragma once
#include <core/Data.h>
#include <core/GraphNodeSocket.h>
#include <plugins/gl/Renderer.h>
#include <plugins/gl/Visualizer.h>
#include <plugins/gl/Texture.h>
#include <plugins/primitives/Camera.h>

#include "CloudData.h"

class CloudRenderer : public gl::Renderer
{
	Q_OBJECT
public:
	typedef std::shared_ptr<CloudRenderer> Ptr;

	CloudRenderer();
	CloudRenderer( core::GraphNodeSocket::Ptr src );


	virtual void                           render( gl::Context* context )override;

private:
	core::GraphNodeSocket::Ptr             m_src;
	CloudData::Ptr                         m_cd;
	gl::Visualizer::Ptr                    m_visualizer;
	gl::Texture2d::Ptr                     m_tex_qc;
	gl::Shader::Ptr                        m_shader;
	Camera::Ptr                            m_camera;
};
