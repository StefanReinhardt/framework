#include "VortexConfinement2D.h"
#include <plugins/clouds/CloudData.h>


VortexConfinement2D::VortexConfinement2D()
{
		m_dt = 1.0f;
}

void VortexConfinement2D::setField(QString name)
{
this->vortField = name;
}



void VortexConfinement2D::apply(SimObject::Ptr so)
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
	m_dt = cd->m_parms.m_dt;

	math::V3i res = vel_x->getResolution();
	res = math::V3i(res.x-1,res.y,res.z);


	// TODO:
	//Scalar Field to Vector if Vort in 3D

	// create vorticity field
	ScalarField::Ptr vorticity;

	VectorField::Ptr test;

	if(so->hasSubData("vorticity") )
	{
		vorticity = so->getSubData<ScalarField>( "vorticity" );
		test = so->getSubData<VectorField>("test");
	}
	else
	{
		vorticity =std::make_shared<ScalarField>(vel_x);
		vorticity->resize(res);
		so->addSubData( "vorticity", vorticity );

		test = std::make_shared<VectorField>();
		test->resize(res);
		so->addSubData( "test", test);
	}



	float nab_nx, nab_ny, mag_n, nx, ny;
	int k = 0;

	//Calculate vorticity magnitude field = n
	for (int i=1; i<res.x-1; i++)
		for (int j=1; j<res.y-1; j++){
			vorticity->lvalue(i,j,k)= curl(i,j,k);
		}


	//Calculate vorticity Gradient N = (nabla n) / ||n||
	//for( int k=2;k<res.z-3;++k )
		for( int j=2;j<res.y-2;++j )
			for( int i=2;i<res.x-2;++i )
			{
				//calc nabla n for x and y by central difference
				nab_nx = (abs(vorticity->lvalue(i+1,j,k)) - abs(vorticity->lvalue(i-1,j,k)))/2;
				nab_ny = (abs(vorticity->lvalue(i,j+1,k)) - abs(vorticity->lvalue(i,j-1,k)))/2;

				//calculate magnitude of the vector (nab_nx , nab_ny)
				//add small value to prevent divide by zero
				mag_n = (float) sqrt(nab_nx*nab_nx + nab_ny*nab_ny) + 0.00000001f;

				// N = (nabla n) / ||n||
				nx = nab_nx / mag_n;
				ny = nab_ny / mag_n;

				test->getScalarField(0)->lvalue(i,j,k) = - ny * ( vorticity->lvalue(i,j,k) + vorticity->lvalue(i-1,j,k) );
				test->getScalarField(1)->lvalue(i,j,k) = nx * ( vorticity->lvalue(i,j,k) + vorticity->lvalue(i,j-1,k) );

				// F = N x vorticity
				// F = Nx * vort_y - Ny * vort_x
				//if(i>1 && i<res.x-2)
					vel_x->lvalue(i,j,k) +=	-	ny * ( vorticity->lvalue(i,j,k) + vorticity->lvalue(i-1,j,k) ) * 0.5f * cd->m_parms.m_vorticity * m_dt;
				//if(j>1 && j<res.y-2)
					vel_y->lvalue(i,j,k) +=		nx * ( vorticity->lvalue(i,j,k) + vorticity->lvalue(i,j-1,k) ) * 0.5f * cd->m_parms.m_vorticity * m_dt;
				//vel_y->lvalue(i,j,k) = vel_y->evaluate(math::V3f(i+0.5f,j,k+0.5f));
				//vel_x->lvalue(i,j,k) = vel_x->evaluate(math::V3f(i,j+0.5f,k+0.5f));
			}



}


float VortexConfinement2D::curl(int i, int j, int k){

	k=0;
	// central diff of vel_y in x direction
	float dv_dx =	(vel_y->evaluate(math::V3f(i+1.0f,j+0.5f,k+0.5f))-vel_y->evaluate(math::V3f(i,j+0.5f,k+0.5f)));
	float du_dy =	(vel_x->evaluate(math::V3f(i+0.5f,j+1.0f,k+0.5f))-vel_x->evaluate(math::V3f(i+0.5f,j,k+0.5f)));

//	float dv_dx =(		(vel_y->lvalue(i,j,k) + vel_y->lvalue(i+1,j,k) + vel_y->lvalue(i,j+1,k) + vel_y->lvalue(i+1,j+1,k)	) /4
//					-	(vel_y->lvalue(i,j,k) + vel_y->lvalue(i-1,j,k) + vel_y->lvalue(i,j+1,k) + vel_y->lvalue(i-1,j+1,k)	) /4
//					) * 0.5f;

	// central diff of vel_x in y direction
//	float du_dy =(		(vel_x->lvalue(i,j+1,k) + vel_x->lvalue(i+1,j+1,k) + vel_x->lvalue(i,j,k) + vel_x->lvalue(i+1,j,k)	) /4
//					-	(vel_x->lvalue(i,j,k) + vel_x->lvalue(i+1,j,k) + vel_x->lvalue(i,j-1,k) + vel_x->lvalue(i+1,j-1,k)	) /4
//					) * 0.5f;

	return du_dy - dv_dx;
}






// Serialize
void VortexConfinement2D::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "vortField", vortField );
}


void VortexConfinement2D::load( QJsonObject &o )
{
	Operator::load( o );

	vortField = o["vortField"].toString();
}

