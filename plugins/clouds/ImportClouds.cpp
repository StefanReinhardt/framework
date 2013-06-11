#include "ImportClouds.h"
#include <plugins/sim/SimObject.h>





ImportClouds::ImportClouds() : core::GraphNode()
{
	addInputSocket( "file" );
	addOutputSocket( "output" );

	getSocket("file")->setString("$HERE/input.bgeo");
}

void ImportClouds::update(core::GraphNodeSocket *output)
{
	QString filename = getSocket("file")->asString();
	SimObject::Ptr so = std::make_shared<SimObject>();

	qDebug() << "ImportClouds: update " << filename;
	getSocket( "output" )->setData(so);
}
