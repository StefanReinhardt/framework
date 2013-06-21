#include "CreateClouds.h"

#include <core/Core.h>
#include <plugins/sim/SimObject.h>
#include <plugins/clouds/CloudData.h>
#include <plugins/primitives/ScalarField.h>

CreateClouds::CreateClouds() : core::GraphNode()
{
    addOutputSocket( "output" );
}

void CreateClouds::update(core::GraphNodeSocket *output)
{
    qDebug() << "CreateClouds: update ";

	CloudData::Ptr cd = std::make_shared<CloudData>();

	cd->print();
	getSocket( "output" )->setData(cd);
}
