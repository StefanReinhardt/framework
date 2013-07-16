#include "GLViewer.h"

#include <iostream>

#include <QDebug>

//#include "Application.h"
//#include "InteractionOrbitVector.h"

namespace frontend
{


	GLViewer::GLViewer( QWidget *parent ) : QGLWidget(parent)
	{
		// disable background on widget because
		setAttribute(Qt::WA_OpaquePaintEvent);
		// multisampling for higher quality display
		QGLFormat format(QGL::SampleBuffers | QGL::DoubleBuffer | QGL::AlphaChannel);
		format.setSamples(16);
		setFormat(format);
		makeCurrent();
		// ?
		setFocusPolicy( Qt::StrongFocus );
		// ...
		setMouseTracking( true );
		// this will make sure swapbuffers is not called by qt when doing double buffering
		setAutoBufferSwap(false);
	}

	GLViewer::~GLViewer()
	{
//		// get rid of interactionhandler
//		disconnect(m_interactionHandler.get(), 0, this, 0);
//		m_interactionHandler = InteractionHandler::Ptr();

//		emit onShutdownGL();

		m_context = gl::Context::Ptr();
	}


	void GLViewer::initializeGL()
	{
		GLenum glewResult = glewInit();
		if (GLEW_OK != glewResult)
		{
			throw std::runtime_error("glew init failed");
		}else
			qDebug() << "GLEW successfully initialized...";

		m_context = gl::Context::create();

		//emit onInitializeGL();
	}

	void GLViewer::resizeGL(int w, int h)
	{
		// TODO:context
		glViewport(0, 0, (GLint)w, (GLint)h);
		// QT forgets the backgroundmode when resizing
		setAttribute(Qt::WA_OpaquePaintEvent);
	}

	void GLViewer::paintGL()
	{
		glClearColor( 1.0, 0.0, 0.0, 0.0 ); // Let OpenGL clear to black
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		/*
		emit onRender();

		// render InteractionHandle
		if( m_interactionHandler )
			m_interactionHandler->render( m_context );
		*/
		swapBuffers();
	}

/*
	void GLViewer::setInteractionHandler( InteractionHandler::Ptr handler )
	{
		if( m_interactionHandler )
			disconnect(m_interactionHandler.get(), 0, this, 0);
		m_interactionHandler = handler;
		if( m_interactionHandler )
			connect( m_interactionHandler.get(), SIGNAL(updateRender()), this, SLOT(update()) );
	}

	void GLViewer::setAlternativeInteractionHandler( InteractionHandler::Ptr handler )
	{
		if( m_alternativeInteractionHandler )
			disconnect(m_alternativeInteractionHandler.get(), 0, this, 0);
		m_alternativeInteractionHandler = handler;
		if( m_alternativeInteractionHandler )
			connect( m_alternativeInteractionHandler.get(), SIGNAL(updateRender()), this, SLOT(update()) );
	}








	void GLViewer::mouseMoveEvent( QMouseEvent * event )
	{
		if( (QApplication::keyboardModifiers() & Qt::AltModifier) && m_alternativeInteractionHandler )
		{
			m_alternativeInteractionHandler->mouseMoveEvent( event );
		}else
		if( m_interactionHandler )
			m_interactionHandler->mouseMoveEvent( event );
	}

	void GLViewer::keyPressEvent( QKeyEvent * event )
	{

//		if( event->key() == Qt::Key_Alt )
//		{
//			// use alternate interaction handler if available
//			std::cout << "keyxpress" << std::endl;
//			core::Data::Ptr data = Application::getInstance()->getSelectedData();

//			// todo: this should be done in a proper place
//			// maybe Application which sets alternative interaction handler
//			if(  )

//			//InteractionOrbitVector::create(  )
//		}else


//		if( (QApplication::keyboardModifiers() & Qt::AltModifier) && m_alternativeInteractionHandler )
//		{
//			m_alternativeInteractionHandler->keyPressEvent( event );
//		}else
//		if( m_interactionHandler )
//			m_interactionHandler->keyPressEvent( event );
	}

	void GLViewer::keyReleaseEvent( QKeyEvent * event )
	{
		if( (QApplication::keyboardModifiers() & Qt::AltModifier) && m_alternativeInteractionHandler )
		{
			m_alternativeInteractionHandler->keyReleaseEvent( event );
		}else
		if( m_interactionHandler )
			m_interactionHandler->keyReleaseEvent( event );
	}






*/
}
