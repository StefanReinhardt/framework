#pragma once
#include <math/Math.h>

#include <memory>
#include <stack>
/*

#include "Attribute.h"
#include "Shader.h"
#include "Geometry.h"
#include "Camera.h"
#include "Texture.h"
*/


namespace gl
{
	//struct FBO;
	struct Context
	{
		typedef std::shared_ptr<Context> Ptr;

		// used to store and cache all matrices (including redundant ones) of the transform pipeline
		struct TransformState
		{
			math::Matrix44f                                       modelMatrix; // object to world
			math::Matrix44f                                       viewMatrix; // world to camera
			math::Matrix44f                                       modelViewMatrix; // world to eye
			math::Matrix44f                                       projectionMatrix; // camera to view
			math::Matrix44f                                       modelViewProjectionMatrix; // model view projection matrix (world to screen)
			math::Matrix44f                                       viewInverseMatrix; // view matrix inverse (camera to world)
			math::Matrix44f                                       modelViewInverse; // model view matrix inverse
			math::Matrix33f                                       modelViewInverseTranspose; // model view matrix inverse transpose (model view matrix without scaling/shearing) used to transform vectors
		};

		struct ViewportState
		{
			int                                                   width;
			int                                                   height;
			//FBO*                                                  fbo;
		};

		Context();
		~Context();
		static Ptr                                                create();
		/*
		static void                                                      setCurrentContext( ContextPtr context ); // sets current context
		*/

/*
		// timing info
		float                                                                                             time(); // returns current time
		void                                                                               setTime( float time ); // sets current time

		// transform info
		CameraPtr                                                                                       camera();
		void                                                                       setCamera( CameraPtr camera );
		void setView( const math::Matrix44f &view, const math::Matrix44f &viewinv, const math::Matrix44f &proj ); // convinience function for updating transformState from view matrices

		void                                                setModelMatrix( const math::Matrix44f &modelMatrix );
		void                                                  setViewMatrix( const math::Matrix44f &viewMatrix );
		void                                                                               setProjectionMatrix();
		math::Vec3f                                                   worldToView( const math::Vec3f &worldPos );

		math::Matrix44f                                                                     getModelViewMatrix();
		math::Matrix44f                                                                    getModelViewInverse();
		math::Matrix44f                                                                         getViewInverse();

		void                                                       getTransformState( TransformState &ts ) const;
		void                                                       setTransformState( const TransformState &ts );

		void                                                 pushViewport( int width, int height, FBO *fbo = 0 ); // used by fbos
		ViewportState                                                                              popViewport();
		int                                                                              getViewportWidth()const;
		int                                                                             getViewportHeight()const;

		//
		// global uniform manangement
		//
		void                                         setUniform( const std::string &name, AttributePtr uniform );
		AttributePtr                                                       getUniform( const std::string &name );
		std::map<std::string, AttributePtr>                                                     m_globalUniforms;


		//
		// rendering
		//
		void                                                         render( Geometry::Ptr geo, ShaderPtr shader );
		void                           render( Geometry::Ptr geo, ShaderPtr shader, const math::Matrix44f &xform );
		void                                                                    renderScreen( ShaderPtr shader );
		void                                 renderScreen( Texture2dPtr texture );
		void                                 renderScreenVFip( Texture2dPtr texture );

		void                                           bind( ShaderPtr shader, Geometry::Ptr geo = Geometry::Ptr() );
		void                                         unbind( ShaderPtr shader, Geometry::Ptr geo = Geometry::Ptr() );

		//
		// some standard resources
		//
		Geometry::Ptr                        m_screenQuad;
		Geometry::Ptr                        m_screenQuadVFlipped;
		ShaderPtr                            m_simpleTextureShaderScreen;
		ShaderPtr                            m_simpleTextureShader;
		ShaderPtr                            m_constantShader;
*/
	private:
		TransformState                                            m_currentTransformState;

/*
		AttributePtr                                                                                   m_mvmAttr; // model view matrix (world to eye)
		AttributePtr                                                                                  m_mvpmAttr; // model view projection matrix (world to screen)
		AttributePtr                                                                                    m_mmAttr; // model matrix attribute
		AttributePtr                                                                                    m_vmAttr; // view matrix (camera to world) attribute
		AttributePtr                                                                                    m_pmAttr; // projection matrix attribute
		AttributePtr                                                                                 m_vminvAttr; // view matrix inverse (camera to world)
		AttributePtr                                                                               m_mvminvtAttr; // model view matrix inverse transpose (model view matrix without scaling/shearing) used to transform vectors

		CameraPtr                                                                                       m_camera;

		std::stack< ViewportState >                                                              m_viewportStack;

		// time =========================
		float                                                                                             m_time;
*/
	};

	Context::Ptr                                                  currentContext(); // returns current context
}
