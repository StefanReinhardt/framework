#pragma once
#include <iostream>
#include <ui_Clouds.h>

#include <core/Graph.h>

struct CloudsUI : public QWidget
{
	Q_OBJECT
public:
	CloudsUI( core::Graph::Ptr graph, QWidget *parent = 0);


public slots:
	void onVCToggled( bool checked );
	void onVCStrengthChanged( int value );

signals:
	void makeDirty(void);
private:
	core::Graph::Ptr                       m_graph;
	Ui_Clouds                              ui;
};
