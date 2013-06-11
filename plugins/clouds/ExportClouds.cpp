#include "ExportClouds.h"

#include <plugins/sim/SimObject.h>

#include <QDebug>




ExportClouds::ExportClouds() : core::GraphNode()
{
	addInputSocket( "input" );
	addInputSocket( "file" );

	getSocket("file")->setString("$HERE/output.bgeo");
}

void ExportClouds::update(core::GraphNodeSocket *output)
{
	SimObject::Ptr so = getSocket("input")->getData<SimObject>();
	QString filename = getSocket("file")->asString();

	if( so )
	{
		qDebug() << "ExportClouds:: got so! " << filename;
	}else
		qDebug() << "ExportClouds:: NO so! " << filename;



}
