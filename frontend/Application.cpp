#include "Application.h"


#include <QSplitter>
#include <QTreeView>
#include <QMenuBar>
#include <QFrame>


#include <iostream>

#include <core/Core.h>


namespace frontend
{

	Application::Application( int argc, char **argv ) : QApplication(argc, argv)
	{
		//  initialize core =======================
		core::init();

		core::setVariable( "$HERE", applicationDirPath() );

		// main window ================
		m_mainWindow = new QMainWindow();
		m_mainWindow->resize(800, 600);

		// file menu ================
		/*
		QMenu *fileMenu = m_mainWindow->menuBar()->addMenu(tr("&File"));

		m_fileOpen = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
		m_fileOpen->setShortcuts(QKeySequence::Open);
		m_fileOpen->setStatusTip(tr("Open an existing file"));
		connect(m_fileOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
		fileMenu->addAction(m_fileOpen);

		m_fileSave = new QAction(QIcon(":/images/Save.png"), tr("&Save..."), this);
		m_fileSave->setShortcuts(QKeySequence::Save);
		m_fileSave->setStatusTip(tr("Save an existing file"));
		connect(m_fileSave, SIGNAL(triggered()), this, SLOT(fileSave()));
		fileMenu->addAction(m_fileSave);
		*/


		// splitter ---
		m_split1 = new QSplitter(Qt::Horizontal);


		// GLViewer ---
		GLViewer *glv = new GLViewer(m_split1);
		connect(glv, SIGNAL( render(gl::Context*) ), this, SLOT(render(gl::Context*)) );
		connect(core::instance().get(), SIGNAL( frameChanged(int) ), glv, SLOT(update()) );


		QFrame *editor1 = new QFrame;

		m_split1->addWidget(editor1);

		QList<int> sizes;
		sizes.push_back(750);
		sizes.push_back(50);
		m_split1->setSizes(sizes);

		m_mainWindow->setCentralWidget( m_split1 );

		m_mainWindow->show();
	}

	Application::~Application()
	{
		//  shutdown core =======================
		core::shutdown();
	}

	QMainWindow* Application::getMainWindow()
	{
		return m_mainWindow;
	}

	void Application::setRenderer( gl::Renderer::Ptr renderer )
	{
		m_renderer = renderer;
	}

	void Application::render( gl::Context* context )
	{
		if( m_renderer)
		{
			m_renderer->render(context);
		}
	}

	Application* Application::getInstance()
	{
		return (Application*)instance();
	}




	void Application::fileOpen()
	{
		// handle currently opened file
		// TODO
		//core::Graph::Ptr graph = core::load( "c:\\projects\\clouds\\bin\\test.json" );
	}

	void Application::fileSave()
	{
		//core::save( "c:\\projects\\clouds\\bin\\test.json", graph );
	}

}
