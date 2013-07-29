#pragma once
#include <math/Math.h>

#include <memory>
#include <stack>
#include "Attribute.h"
#include "Geometry.h"
#include "Shader.h"
/*
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
*/
		void                                                      setView( const math::M44f &view, const math::M44f &viewInv, const math::M44f &proj ); // convinience function for updating transformState from view matrices
/*
		void                                                setModelMatrix( const math::Matrix44f &modelMatrix );
		void                                                  setViewMatrix( const math::Matrix44f &viewMatrix );
		void                                                                               setProjectionMatrix();
		math::Vec3f                                                   worldToView( const math::Vec3f &worldPos );

		math::Matrix44f                                                                     getModelViewMatrix();
		math::Matrix44f                                                                    getModelViewInverse();
		math::Matrix44f                                                                         getViewInverse();

		void                                                       getTransformState( TransformState &ts ) const;
		void                                                       setTransformState( const TransformState &ts );

		*/
		void                                    pushViewport( int width, int height/*, FBO *fbo = 0*/ ); // used by fbos
		ViewportState                           popViewport();
		int                                     getViewportWidth()const;
		int                                     getViewportHeight()const;
		float                                   getViewportAspect()const;

		// global uniform manangement ----
		void                                    setUniform( const std::string &name, Attribute::Ptr uniform );
		Attribute::Ptr                          getUniform( const std::string &name );


		// rendering ----
		void                                    render( Geometry::Ptr geo, Shader::Ptr shader );
		void                                    render( Geometry::Ptr geo, Shader::Ptr shader, const math::Matrix44f &xform );
		void                                    renderScreen( Shader::Ptr shader );
		//void                                    renderScreen( /*Texture2d::Ptr texture*/ );
		void                                    bind( Shader::Ptr shader, Geometry::Ptr geo = Geometry::Ptr() );
		void                                    unbind( Shader::Ptr shader, Geometry::Ptr geo = Geometry::Ptr() );
		/*

		void                                 renderScreenVFip( Texture2dPtr texture );


*/
	private:
		TransformState                          m_currentTransformState;


		Attribute::Ptr                          m_mvmAttr; // model view matrix (world to eye)
		Attribute::Ptr                          m_mvpmAttr; // model view projection matrix (world to screen)
		Attribute::Ptr                          m_mmAttr; // model matrix attribute
		Attribute::Ptr                          m_vmAttr; // view matrix (camera to world) attribute
		Attribute::Ptr                          m_pmAttr; // projection matrix attribute
		Attribute::Ptr                          m_vminvAttr; // view matrix inverse (camera to world)
		Attribute::Ptr                          m_mvminvtAttr; // model view matrix inverse transpose (model view matrix without scaling/shearing) used to transform vectors

		std::map<std::string, Attribute::Ptr>   m_globalUniforms;
		std::stack< ViewportState >             m_viewportStack;
/*
		CameraPtr                                                                                       m_camera;



		// time =========================
		float                                                                                             m_time;
*/

		// standard resources ----
		Geometry::Ptr                           m_screenQuad;
		/*
		Geometry::Ptr                           m_screenQuadVFlipped;
		Shader::Ptr                             m_simpleTextureShaderScreen;
		Shader::Ptr                             m_simpleTextureShader;
		Shader::Ptr                             m_constantShader;
		*/
	};

	Context::Ptr                                                  currentContext(); // returns current context
}
