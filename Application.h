#pragma once

#include <QtGui>
#include <QApplication>
#include <QAction>
#include <QMainWindow>
#include <QTimer>


class Application : public QApplication
{
	Q_OBJECT
public:
                                    Application( int argc, char **argv );
	virtual                         ~Application();

	static Application*             getInstance();

public slots:
	void                            fileOpen();
	void                            fileSave();


private:
	QMainWindow*                    m_mainWindow;
	QAction*                        m_fileOpen;
	QAction*                        m_fileSave;
};



