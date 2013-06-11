#include "ExportClouds.h"

#include <plugins/sim/SimObject.h>

#include <QDebug>




ExportClouds::ExportClouds() : core::GraphNode()
{
	addInputSocket( "input" );
}

void ExportClouds::update(core::GraphNodeSocket *output)
{

	SimObject::Ptr so = getSocket("input")->getData<SimObject>();
	if( so )
	{
		qDebug() << "ExportClouds:: got so!";
	}else
		qDebug() << "ExportClouds:: NO so!";
/*
  QString filename = core::expand(getSocket("file")->asString());
  // access so data and store
 */



}
