#include "Application.h"

#include <QSplitter>
#include <QTreeView>


Application::Application( int argc, char **argv ) : QApplication(argc, argv)
{
	// main window ================
	m_mainWindow = new QMainWindow();
	m_mainWindow->resize(800, 600);

	// splitter ---
	QSplitter *split1 = new QSplitter();


    m_mainWindow->setCentralWidget( split1 );

	m_mainWindow->show();
}

Application::~Application()
{

}

Application* Application::getInstance()
{
	return (Application*)instance();
}

