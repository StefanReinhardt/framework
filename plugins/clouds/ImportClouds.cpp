#include "ImportClouds.h"

#include <fstream>

#include <core/Core.h>

#include <plugins/sim/SimObject.h>
#include <plugins/houdini/HouGeoIO.h>





ImportClouds::ImportClouds() : core::GraphNode()
{
	addInputSocket( "file" );
	addInputSocket( "frame" );
	addOutputSocket( "output" );

	getSocket("frame")->setInt(8);
	getSocket("file")->setString("$HERE/cloud_initial.$F4.bgeo");
}

void ImportClouds::update(core::GraphNodeSocket *output)
{
	qDebug() << "ImportClouds: update ";

	QString filename = core::expand(getSocket("file")->asString());
	SimObject::Ptr so = std::make_shared<SimObject>();
	int frame = getSocket( "frame" )->asInt();


	// load houdini file ================
	std::ifstream in( filename.toUtf8(), std::ios_base::in | std::ios_base::binary );
	houdini::HouGeo::Ptr hgeo = houdini::HouGeoIO::import( &in );
	if( hgeo )
	{
		houdini::HouGeo::HouVolume::Ptr hgeo_vol = std::dynamic_pointer_cast<houdini::HouGeo::HouVolume>(hgeo->getPrimitive(0));
		// attach primitives ================
		so->addSubData( "test", hgeo_vol->field );
		getSocket( "output" )->setData(so);
	}
}
