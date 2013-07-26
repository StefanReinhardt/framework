#pragma once

#include "GLViewer.h"

#include <core/GraphNodeSocket.h>

#include <QtGui>
#include <QApplication>
#include <QAction>
#include <QMainWindow>
#include <QTimer>


namespace frontend
{
	class Application : public QApplication
	{
		Q_OBJECT
	public:
		Application( int argc, char **argv );
		virtual                         ~Application();

		static Application*             getInstance();
		QMainWindow*                    getMainWindow();


		// rendering
		void                            setRenderer( gl::Renderer::Ptr renderer );


	public slots:
		void                            render( gl::Context* context );
		void                            fileOpen();
		void                            fileSave();


	private:
		QMainWindow*                    m_mainWindow;
		QAction*                        m_fileOpen;
		QAction*                        m_fileSave;

		gl::Renderer::Ptr               m_renderer;
	};

}

