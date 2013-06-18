#include "ExportClouds.h"

#include <plugins/sim/SimObject.h>
#include <plugins/primitives/ScalarField.h>
#include <plugins/houdini/HouGeoIO.h>

#include <core/Core.h>

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
	QString filename = core::expand(getSocket("file")->asString());


	// test
	/*
	{
		ScalarField::Ptr test = std::make_shared<ScalarField>();
		std::ofstream out( filename.toUtf8() , std::ios_base::out | std::ios_base::binary );
		houdini::HouGeo::Ptr houGeo = std::make_shared<houdini::HouGeo>();
		houGeo->addPrimitive( test );
		houdini::HouGeoIO::xport( &out, houGeo );
	}
	{
		std::ofstream out( (filename+".log").toUtf8() , std::ios_base::out | std::ios_base::binary );
		houdini::HouGeoIO::makeLog( filename.toStdString(), &out );
	}
	*/

	if( so )
	{
		qDebug() << "ExportClouds:: got so! " << filename;

		ScalarField::Ptr test = so->getSubData<ScalarField>( "test" );

		if( test )
		{
			{
				std::ofstream out( filename.toUtf8() , std::ios_base::out | std::ios_base::binary );
				houdini::HouGeo::Ptr houGeo = std::make_shared<houdini::HouGeo>();
				houGeo->addPrimitive( test );
				houdini::HouGeoIO::xport( &out, houGeo );

				qDebug() << "test " << test->getResolution().x << test->getResolution().y << test->getResolution().z;
			}

			{
				//std::ofstream out( (filename+".log").toUtf8() , std::ios_base::out | std::ios_base::binary );
				//houdini::HouGeoIO::makeLog( filename.toStdString(), &out );
			}
		}
	}else
		qDebug() << "ExportClouds:: NO so! " << filename;


}
