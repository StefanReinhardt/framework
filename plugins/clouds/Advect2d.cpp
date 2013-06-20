#include "Advect2d.h"


#include <plugins/primitives/ScalarField.h>

Advect2d::Advect2d() : Operator()
{
}

void Advect2d::apply( SimObject::Ptr so )
{
	qDebug() << "Advect2d::apply";

	// get density scalarfield ---
	ScalarField::Ptr density = so->getSubData<ScalarField>( "density" );

	// get temporary scalarfield for update ---
	ScalarField::Ptr density_old;
	if( so->hasSubData("density_old") )
		density_old = so->getSubData<ScalarField>( "density_old" );
	else
	{
		// initialize density_old field from density field
		density_old = std::make_shared<ScalarField>( density );
		so->addSubData( "density_old", density_old );
	}

	// make current density -> density_old
	std::swap( density, density_old );

	// as a temporary action, move all voxels one voxel to the right
	math::V3i res = density->getResolution();
	for( int k=0;k<res.z;++k )
		for( int j=0;j<res.y;++j )
			for( int i=0;i<res.x;++i )
			{
                if(i==res.x-1)
					density->lvalue(i, j, k) = 0.0f;
				else
                    density->lvalue(i, j, k) = density_old->sample(i+1, j, k);
			}


	// since we swapped pointers we have to swap subdata as well
	so->setSubData( "density", density );
	so->setSubData( "density_old", density_old );
}

