#include "ExportClouds.h"

#include <plugins/sim/SimObject.h>
#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>
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

		std::vector<QString> subDataNames = so->getSubDataNames();

		for (int i=0; i<subDataNames.size(); ++i)
		{
			QString subDataName = subDataNames[i];

			//if((subDataName!="qc") && (subDataName!="qv") )
			//	continue;

			// ScalarField?
			if( so->getSubData<ScalarField>(subDataName) )
			{
				houGeo->addPrimitive(so->getSubData<ScalarField>(subDataName));
				nameAttr->addString(subDataName.toStdString());
			}else
			// VectorField?
			if( so->getSubData<VectorField>(subDataName) )
			{
				houGeo->addPrimitive(so->getSubData<VectorField>(subDataName)->getScalarField(0));
				nameAttr->addString((subDataName+".x").toStdString());
				houGeo->addPrimitive(so->getSubData<VectorField>(subDataName)->getScalarField(1));
				nameAttr->addString((subDataName+".y").toStdString());
				houGeo->addPrimitive(so->getSubData<VectorField>(subDataName)->getScalarField(2));
				nameAttr->addString((subDataName+".z").toStdString());

				for( int j=0;j<3;++j )
				{
					ScalarField::Ptr f = so->getSubData<VectorField>(subDataName)->getScalarField(j);
					math::Box3f b = f->bound();
					qDebug() << f->getVoxelSize().x << " " << f->getVoxelSize().y << " " << f->getVoxelSize().z;
				}

			}
		}

		/*
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
		*/
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
	*/
	/*
    {
        filename = "c:/projects/clouds/bin/null.bgeo";
        std::ofstream out( (filename+".log").toUtf8() , std::ios_base::out | std::ios_base::binary );
        houdini::HouGeoIO::makeLog( filename.toStdString(), &out );
    }
	*/
	/*
	{
		filename = "c:/projects/clouds/bin/ref.bgeo";
		std::ofstream out( (filename+".log").toUtf8() , std::ios_base::out | std::ios_base::binary );
		houdini::HouGeoIO::makeLog( filename.toStdString(), &out );
	}
	*/
}
