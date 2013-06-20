#include "CreateCloud.h"

#include <core/Core.h>
#include <plugins/sim/SimObject.h>
#include <plugins/primitives/ScalarField.h>

CreateCloud::CreateCloud() : core::GraphNode()
{
    addOutputSocket( "output" );
}


void CreateCloud::update(core::GraphNodeSocket *output)
{
    qDebug() << "CreateClouds: update ";

    SimObject::Ptr so = std::make_shared<SimObject>();

    // Create fields
    // pt, qc, qv

    ScalarField::Ptr pt;
    pt->create(math::V3i(10),math::M44f() );

    so->addSubData("pt", pt );

    so->print();
    getSocket( "output" )->setData(so);

}
