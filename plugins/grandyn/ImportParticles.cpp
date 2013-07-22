#include "ImportParticles.h"

#include <fstream>

#include <core/Core.h>

#include <plugins/primitives/Attribute.h>
#include <plugins/sim/SimObject.h>
#include <plugins/houdini/HouGeoIO.h>





ImportParticles::ImportParticles() : core::GraphNode()
{
	addInputSocket( "file" );
	addOutputSocket( "output" );
}

void ImportParticles::update(core::GraphNodeSocket *output)
{
	qDebug() << "ImportParticles: update ";

	QString filename = core::expand(getSocket("file")->asString());
	SimObject::Ptr so = std::make_shared<SimObject>();

	// if filename specified....
	if( !filename.isEmpty() )
	{
		// load houdini file ================
		std::ifstream in( filename.toUtf8(), std::ios_base::in | std::ios_base::binary );
		houdini::HouGeo::Ptr hgeo = houdini::HouGeoIO::import( &in );
		if( hgeo )
		{
			// iterate all point attributes
			std::vector<std::string> attrNames;
			hgeo->getPointAttributeNames(attrNames);

			for( auto attrName : attrNames )
			{
				houdini::HouGeo::HouAttribute::Ptr hattr = std::dynamic_pointer_cast<houdini::HouGeo::HouAttribute>(hgeo->getPointAttribute(attrName));
				so->addSubData(QString::fromStdString(attrName), hattr->m_attr);
			}
		}
	}

	int numParticles = 0;

	// create default P attribute if required
	if(!so->getSubData<Attribute>("P"))
	{
		numParticles = 1000;
		Attribute::Ptr pAttr = Attribute::createV3f(numParticles);
		so->addSubData("P", pAttr);

		for( int i=0;i<numParticles;++i )
			pAttr->set<math::V3f>( i, math::V3f(math::g_randomNumber.randomFloat(), math::g_randomNumber.randomFloat(), math::g_randomNumber.randomFloat()) );
	}else
		numParticles = so->getSubData<Attribute>("P")->numElements();

	// initialize velocity if required
	if( !so->getSubData<Attribute>("velocity") )
	{
		Attribute::Ptr velAttr = Attribute::createV3f(numParticles);
		so->addSubData("velocity", velAttr);

		for( int i=0;i<numParticles;++i )
			velAttr->set<math::V3f>( i, math::V3f(0.0f) );
	}

	getSocket( "output" )->setData(so);
	so->print();
}
