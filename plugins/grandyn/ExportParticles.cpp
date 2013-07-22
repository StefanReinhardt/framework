#include "ExportParticles.h"

#include <plugins/sim/SimObject.h>
#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>
#include <plugins/primitives/Attribute.h>
#include <plugins/houdini/HouGeoIO.h>

#include <core/Core.h>

#include <QDebug>




ExportParticles::ExportParticles() : core::GraphNode()
{
	addInputSocket( "input" );
	addInputSocket( "file" );
}

void ExportParticles::update(core::GraphNodeSocket *output)
{
	qDebug() << "ExportParticles: update ";
	SimObject::Ptr so = getSocket("input")->getData<SimObject>();
	QString filename = core::expand(getSocket("file")->asString());



	if( so )
	{
		std::ofstream out( filename.toUtf8() , std::ios_base::out | std::ios_base::binary );
		houdini::HouGeo::Ptr houGeo = std::make_shared<houdini::HouGeo>();

		houdini::HouGeo::HouAttribute::Ptr nameAttr = std::make_shared<houdini::HouGeo::HouAttribute>();

		std::vector<QString> subDataNames = so->getSubDataNames();

		for (int i=0; i<subDataNames.size(); ++i)
		{
			QString subDataName = subDataNames[i];

			// Attribute?
			if( so->getSubData<Attribute>(subDataName) )
			{
				// we assume all attributes attached to the simobject are meant to be pointattributes
				Attribute::Ptr attr = so->getSubData<Attribute>(subDataName);

				//TODO: ensure all attributes have the same number of elements

				// P attribute in houdini is expected to have 4 components. With 3 components it becomes unstable
				// and eventually crashes. As a workaround we promote V3f to V4f when named P
				if( subDataName == "P" && attr->numComponents()==3 )
				{
					Attribute::Ptr org = attr;
					attr = Attribute::createV4f(org->numElements());
					for( int i=0, numElements = org->numElements();i<numElements;++i)
					{
						math::V3f p = org->get<math::V3f>(i);
						attr->set<math::V4f>(i, math::V4f( p.x, p.y, p.z, 1.0f ));
					}
				}

				// create hougeo attribute which serves as a wrapper for export
				houdini::HouGeo::HouAttribute::Ptr houAttr = std::make_shared<houdini::HouGeo::HouAttribute>(subDataName.toStdString(), attr);

				houGeo->setPointAttribute(houAttr);
			}else
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
			}
		}

		//houGeo->setPrimitiveAttribute( "name", nameAttr );

		houdini::HouGeoIO::xport( &out, houGeo );
	}else
		qDebug() << "ExportParticles:: NO so! " << filename;


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
