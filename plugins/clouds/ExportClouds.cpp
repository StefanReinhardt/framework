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

	// TODO: default?
	getSocket("file")->setString("$HERE/cloud_output.$F4.bgeo");
}

void ExportClouds::update(core::GraphNodeSocket *output)
{
	qDebug() << "ExportClouds: update ";
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
		std::ofstream out( filename.toUtf8() , std::ios_base::out | std::ios_base::binary );
		houdini::HouGeo::Ptr houGeo = std::make_shared<houdini::HouGeo>();

		houdini::HouGeo::HouAttribute::Ptr nameAttr = std::make_shared<houdini::HouGeo::HouAttribute>();

		ScalarField::Ptr density = so->getSubData<ScalarField>( "density" );
		if( density )
		{
			// add primitive
			houGeo->addPrimitive( density );

			// add primitive name
			nameAttr->addString( "density" );
		}


		ScalarField::Ptr volume2 = so->getSubData<ScalarField>( "volume2" );
		if( volume2 )
		{
			// add primitive
			houGeo->addPrimitive( volume2 );

			// add primitive name
			nameAttr->addString( "volume2" );
		}


		houGeo->setPrimitiveAttribute( "name", nameAttr );
		houdini::HouGeoIO::xport( &out, houGeo );
	}else
		qDebug() << "ExportClouds:: NO so! " << filename;

	/*
	if(so)
	{
		std::ofstream out( (filename+".log").toUtf8() , std::ios_base::out | std::ios_base::binary );
		houdini::HouGeoIO::makeLog( filename.toStdString(), &out );
	}

	{
		filename = "c:/projects/clouds/bin/null.bgeo";
		std::ofstream out( (filename+".log").toUtf8() , std::ios_base::out | std::ios_base::binary );
		houdini::HouGeoIO::makeLog( filename.toStdString(), &out );
	}
	{
		filename = "c:/projects/clouds/bin/test.bgeo";
		std::ofstream out( (filename+".log").toUtf8() , std::ios_base::out | std::ios_base::binary );
		houdini::HouGeoIO::makeLog( filename.toStdString(), &out );
	}
	*/
}
