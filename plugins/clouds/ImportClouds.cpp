#include "ImportClouds.h"

#include <fstream>

#include <core/Core.h>

#include <plugins/sim/SimObject.h>
#include <plugins/houdini/HouGeoIO.h>





ImportClouds::ImportClouds() : core::GraphNode()
{
	addInputSocket( "file" );
	addOutputSocket( "output" );

	// TODO: default?
	getSocket("file")->setString("$HERE/cloud_initial.$F4.bgeo");
}

void ImportClouds::update(core::GraphNodeSocket *output)
{
	qDebug() << "ImportClouds: update ";

	QString filename = core::expand(getSocket("file")->asString());
	SimObject::Ptr so = std::make_shared<SimObject>();


	// load houdini file ================
	std::ifstream in( filename.toUtf8(), std::ios_base::in | std::ios_base::binary );
	houdini::HouGeo::Ptr hgeo = houdini::HouGeoIO::import( &in );
	if( hgeo )
	{
		// we look for volume primitives by name
		if( hgeo->hasPrimitiveAttribute("name") )
		{
			houdini::HouGeo::HouAttribute::Ptr primNames = std::dynamic_pointer_cast<houdini::HouGeo::HouAttribute>(hgeo->getPrimitiveAttribute("name"));

			// now load each primitive and name it accordingly
			int primIndex = 0;
			for( auto it = primNames->strings.begin(), end = primNames->strings.end();it != end;++it, ++primIndex )
			{
				std::string &name = *it;


				houdini::HouGeo::Primitive::Ptr prim = hgeo->getPrimitive(primIndex);
				if(std::dynamic_pointer_cast<houdini::HouGeo::HouVolume>(prim) )
				{
					houdini::HouGeo::HouVolume::Ptr volprim = std::dynamic_pointer_cast<houdini::HouGeo::HouVolume>(prim);
					// attach primitive to simdata
					so->addSubData( QString::fromStdString(name), volprim->field );

				}
			}
		}
		so->print();
		getSocket( "output" )->setData(so);

		/*
		houdini::HouGeo::HouVolume::Ptr hgeo_vol = std::dynamic_pointer_cast<houdini::HouGeo::HouVolume>(hgeo->getPrimitive(0));
		// attach primitives ================
		so->addSubData( "test", hgeo_vol->field );
		getSocket( "output" )->setData(so);
		*/
		/*
		{
			std::ofstream out( (filename+".log").toUtf8() , std::ios_base::out | std::ios_base::binary );
			houdini::HouGeoIO::makeLog( filename.toStdString(), &out );
		}
		*/
	}
}

