#pragma once

#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QTimer>


class Application : public QApplication
{

	Q_OBJECT
public:
                                    Application( int argc, char **argv );
    virtual                         ~Application();

    static Application*             getInstance();

private:
	QMainWindow*                    m_mainWindow;
};



