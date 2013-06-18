#include "ImportClouds.h"

#include <fstream>

#include <core/Core.h>

#include <plugins/sim/SimObject.h>
#include <plugins/houdini/HouGeoIO.h>





ImportClouds::ImportClouds() : core::GraphNode()
{
	addInputSocket( "file" );
	addOutputSocket( "output" );

	getSocket("file")->setString("$HERE/cloud_initial.bgeo");
}

void ImportClouds::update(core::GraphNodeSocket *output)
{
	QString filename = core::expand(getSocket("file")->asString());
	SimObject::Ptr so = std::make_shared<SimObject>();

	// tmp
	qDebug() << "ImportClouds: update " << filename;
	//std::ofstream out( (filename + ".log").toUtf8() , std::ios_base::out | std::ios_base::binary );
	//houdini::HouGeoIO::makeLog( filename.toStdString(), &out );

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
