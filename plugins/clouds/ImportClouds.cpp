#include "ImportClouds.h"

#include <fstream>

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
	QString filename = getSocket("file")->asString();
	SimObject::Ptr so = std::make_shared<SimObject>();

	qDebug() << "ImportClouds: update " << filename;


	// load houdini file ================
	std::ifstream in( filename.toUtf8(), std::ios_base::in | std::ios_base::binary );
	houdini::HouGeo::Ptr hgeo = houdini::HouGeoIO::import( &in );
	houdini::HouGeo::HouVolume::Ptr hgeo_vol = std::dynamic_pointer_cast<houdini::HouGeo::HouVolume>(hgeo->getPrimitive(0));
	//densityField->m_field = hgeo_vol->field;





	// attach primitives ================
	// so->addSubData( ... );




	getSocket( "output" )->setData(so);
}
