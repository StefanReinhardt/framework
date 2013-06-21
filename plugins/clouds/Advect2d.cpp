#include "Advect2d.h"


#include <plugins/primitives/ScalarField.h>

Advect2d::Advect2d() : Operator()
{
	dt= 1.0f;
	qDebug() << "============================== no attribute to advect";

}


void Advect2d::apply( SimObject::Ptr so )
{
	qDebug() << "Advect2d::apply";

	// get density scalarfield ---
	ScalarField::Ptr density = so->getSubData<ScalarField>( "density" );
	ScalarField::Ptr vel_x = so->getSubData<ScalarField>( "vel_x" );
	ScalarField::Ptr vel_y = so->getSubData<ScalarField>( "vel_y" );

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
				float x = i - dt * vel_x->sample(i,j,k);
				float y = j - dt * vel_y->sample(i,j,k);
				float z = k; //k -  &dt * vel_z->sample(i,j,k);

				density->lvalue(i,j,k) = density_old->evaluate(math::V3f(x,y,z));
			}


	// since we swapped pointers we have to swap subdata as well
	so->setSubData( "density", density );
	so->setSubData( "density_old", density_old );
}

