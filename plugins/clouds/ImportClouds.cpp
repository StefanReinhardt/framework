#include "ImportClouds.h"
#include <plugins/sim/SimObject.h>





ImportClouds::ImportClouds() : core::GraphNode()
{
	addInputSocket( "file" );
	addOutputSocket( "output" );
}

void ImportClouds::update(core::GraphNodeSocket *output)
{
	qDebug() << "ImportClouds: update";
	SimObject::Ptr so = std::make_shared<SimObject>();
	getSocket( "output" )->setData(so);
}
