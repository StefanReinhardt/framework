#include "VortexConfinement.h"
#include <plugins/clouds/CloudData.h>


VortexConfinement::VortexConfinement()
{
}

void VortexConfinement::setField(QString name)
{
this->vortField = name;
}



void VortexConfinement::apply(SimObject::Ptr so)
{
	if (vortField == 0)
	{
		qCritical() << "vortex confinement: no field set!";
		return;
	}

	qDebug() << "apply: vortex confinement";

	vel_x = so->getSubData<VectorField>(vortField)->getScalarField(0);
	vel_y = so->getSubData<VectorField>(vortField)->getScalarField(1);
	vel_z = so->getSubData<VectorField>(vortField)->getScalarField(2);


	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);

	math::V3i res = vel_x->getResolution();
	res = math::V3i(res.x-1,res.y,res.z);


	// TODO:
	//Scalar Field to Vector if Vort in 3D

	// create vorticity field
	ScalarField::Ptr vorticity;

	if(so->hasSubData("vorticity") )
	{
		vorticity = so->getSubData<ScalarField>( "vorticity" );
	}
	else
	{
		vorticity =std::make_shared<ScalarField>(vel_x);
		so->addSubData( "vorticity", vorticity );
	}



	float nab_nx, nab_ny, mag_n, nx, ny;
	int k = 0;

	//Calculate vorticity magnitude field = n
	for (int i=1; i<res.x-2; i++)
		for (int j=1; j<res.y-2; j++){
			vorticity->lvalue(i,j,k)= abs(curl(i,j,k));
		}



	//Calculate vorticity Gradient N = (nabla n) / ||n||
	//for( int k=2;k<res.z-3;++k )
		for( int j=2;j<res.y-3;++j )
			for( int i=2;i<res.x-3;++i )
			{
				//calc nabla n for x and y by central difference
				nab_nx = (vorticity->lvalue(i+1,j,k) - vorticity->lvalue(i-1,j,k))/2;
				nab_ny = (vorticity->lvalue(i,j+1,k) - vorticity->lvalue(i,j-1,k))/2;

				//calculate magnitude of the vector (nab_nx , nab_ny)
				//add small value to prevent divide by zero
				mag_n = (float) sqrt(nab_nx*nab_nx + nab_ny*nab_ny) + 0.00000001f;

				// N = (nabla n) / ||n||
				nx = nab_nx / mag_n;
				ny = nab_ny / mag_n;

				// F = N x vorticity
				// F = Nx * vort_y - Ny * vort_x
				vel_x->lvalue(i,j,k) +=  -	ny * ( vorticity->lvalue(i,j,k) + vorticity->lvalue(i-1,j,k) ) * 0.5f * cd->vorticity;
				vel_y->lvalue(i,j,k) +=		nx * ( vorticity->lvalue(i,j,k) + vorticity->lvalue(i,j-1,k) ) * 0.5f * cd->vorticity;
			}



}


float VortexConfinement::curl(int i, int j, int k){

	k=0;
	// central diff of vel_y in x direction
	float dv_dx =(		(vel_y->lvalue(i,j,k) + vel_y->lvalue(i+1,j,k) + vel_y->lvalue(i,j+1,k) + vel_y->lvalue(i+1,j+1,k)	) /4
					-	(vel_y->lvalue(i,j,k) + vel_y->lvalue(i-1,j,k) + vel_y->lvalue(i,j+1,k) + vel_y->lvalue(i-1,j+1,k)	) /4
					) * 0.5f;

	// central diff of vel_x in y direction
	float du_dy =(		(vel_x->lvalue(i,j+1,k) + vel_x->lvalue(i+1,j+1,k) + vel_x->lvalue(i,j,k) + vel_x->lvalue(i+1,j,k)	) /4
					-	(vel_x->lvalue(i,j,k) + vel_x->lvalue(i+1,j,k) + vel_x->lvalue(i,j-1,k) + vel_x->lvalue(i+1,j-1,k)	) /4
					) * 0.5f;

	return du_dy - dv_dx;
}






// Serialize
void VortexConfinement::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "vortField", vortField );
}


void VortexConfinement::load( QJsonObject &o )
{
	Operator::load( o );

	vortField = o["vortField"].toString();
}

