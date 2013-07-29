#pragma once

#include <plugins/gl/glew/gl.h>
#include <plugins/gl/Context.h>
#include <plugins/gl/Renderer.h>
/*
#include "gltools/gl.h"
#include "gltools/misc.h"
*/
#include <QtOpenGL/QGLWidget>
//#include <QMouseEvent>
/*
#include "InteractionHandler.h"
*/


namespace frontend
{
	class GLViewer : public QGLWidget
	{
		Q_OBJECT
	public:

		GLViewer( QWidget *parent = 0 );
		virtual                         ~GLViewer();

	signals:
		void                            render( gl::Context* context );
	protected:

		virtual void                    initializeGL()override;
		virtual void                    resizeGL(int w, int h)override;
		virtual void                    paintGL()override;
		/*
		virtual void                    mouseMoveEvent( QMouseEvent * event )override;
		virtual void                    keyPressEvent( QKeyEvent * event )override;
		virtual void                    keyReleaseEvent( QKeyEvent * event )override;
		*/

/*
	public:

		typedef void (*InitCallback)( void );
		typedef void (*RenderCallback)( base::CameraPtr );
		typedef void (*ShutdownCallback)( void );
		typedef void (*MouseMoveCallback)( base::MouseState state );
		typedef void (*KeyPressCallback)( base::KeyboardState &state );


		base::KeyboardState                                                   &getKeyboardState();
		void                          setMouseMoveCallback( MouseMoveCallback mouseMoveCallback );
		void                             setKeyPressCallback( KeyPressCallback keyPressCallback );

		void        setView( math::Vec3f lookat, float distance, float azimuth, float elevation );
		base::OrbitNavigator                                                 &getOrbitNavigator();
		base::CameraPtr                                                               getCamera();
	public slots:
		void                                              setRenderInSeperateThread( bool state );
	private:
		InitCallback                                                                       m_init;
		RenderCallback                                                                   m_render;
		ShutdownCallback                                                               m_shutdown;
		MouseMoveCallback                                                             m_mouseMove;
		KeyPressCallback                                                               m_keyPress;
		base::OrbitNavigator                                                     m_orbitNavigator;

		GLThread                                                                   m_renderThread;

		base::KeyboardState                                                       m_keyboardState;
		std::map<int, int>                                                                m_qtKey; // maps qt keys to our keys

		friend class GLThread;

*/
		gl::Context::Ptr                    m_context;
	/*
		InteractionHandler::Ptr             m_interactionHandler;
		InteractionHandler::Ptr             m_alternativeInteractionHandler; // activated when alt is pressed
	*/
	};

}
