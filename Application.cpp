#include "Application.h"

#include <QSplitter>
#include <QTreeView>
#include <QMenuBar>


#include <iostream>

#include <core/Core.h>


Application::Application( int argc, char **argv ) : QApplication(argc, argv)
{
	//  initialize core =======================
	core::init();

	core::setVariable( "$HERE", applicationDirPath() );
/*
	// main window ================
	m_mainWindow = new QMainWindow();
	m_mainWindow->resize(800, 600);
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


	// splitter ---
	QSplitter *split1 = new QSplitter();


	m_mainWindow->setCentralWidget( split1 );

	m_mainWindow->show();
*/

}

Application::~Application()
{
	//  shutdown core =======================
	core::shutdown();
}

Application* Application::getInstance()
{
	return (Application*)instance();
}


void Application::fileOpen()
{
	// handle currently opened file
	// TODO
	core::Graph::Ptr graph = core::load( "c:\\projects\\clouds\\bin\\test.json" );
	graph->print();
}

void Application::fileSave()
{
	//core::save( "c:\\projects\\clouds\\bin\\test.json", graph );
}
