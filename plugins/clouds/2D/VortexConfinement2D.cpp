#include "VortexConfinement2D.h"
#include <plugins/clouds/CloudData.h>


VortexConfinement2D::VortexConfinement2D()
{
		m_dt = 1.0f;
		m_strenght = 0.123456789f;
}


void VortexConfinement2D::apply(SimObject::Ptr so)
{
	if (m_vortField == 0)
	{
		qCritical() << "vortex confinement: no field set!";
		return;
	}

	qDebug() << "apply: vortex confinement";

	vel_x = so->getSubData<VectorField>(m_vortField)->getScalarField(0);
	vel_y = so->getSubData<VectorField>(m_vortField)->getScalarField(1);
	vel_z = so->getSubData<VectorField>(m_vortField)->getScalarField(2);


	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
	m_dt = cd->m_parms.m_dt;

	math::V3i res = vel_x->getResolution();
	res = math::V3i(res.x-1,res.y,res.z);


	// TODO:
	//Scalar Field to Vector if Vort in 3D

	// create vorticity field
	ScalarField::Ptr vorticity;

	ScalarField::Ptr vortForce_x = std::make_shared<ScalarField>();
	vortForce_x->resize(res);
	ScalarField::Ptr vortForce_y = std::make_shared<ScalarField>();
	vortForce_y->resize(res);

	if(so->hasSubData("vorticity") )
	{
		vorticity = so->getSubData<ScalarField>( "vorticity" );
		//vortForce_x = so->getSubData<VectorField>("vortForce_x");
		//vortForce_y = so->getSubData<VectorField>("vortForce_y");
	}
	else
	{
		vorticity =std::make_shared<ScalarField>();
		vorticity->resize(res);
		so->addSubData( "vorticity", vorticity );


		//so->addSubData( "vortForce", vortForce);
	}



	float nab_nx, nab_ny, mag_n, nx, ny, nX_x, nX_y, nY_x, nY_y;
	int k = 0;

	//Calculate vorticity magnitude field = n
	for (int i=0; i<res.x; i++)
		for (int j=0; j<res.y; j++){
			vorticity->lvalue(i,j,k)= curl(i,j,k);
		}


	//Calculate vorticity Gradient N = (nabla n) / ||n||
	//for( int k=2;k<res.z-3;++k )
		for( int j=1;j<res.y-1;++j )
			for( int i=1;i<res.x-1;++i )
			{

/*
				// vorticity gradient X face ---
				// back diff
				nX_x = abs(vorticity->lvalue(i,j,k)) - abs(vorticity->lvalue(i-1,j,k));
				//nX_y = ((abs(vorticity->lvalue(i-1,j+1,k))+abs(vorticity->lvalue(i,j+1,k))+abs(vorticity->lvalue(i-1,j,k))+abs(vorticity->lvalue(i,j,k)))/4-
				//		(abs(vorticity->lvalue(i,j-1,k))+abs(vorticity->lvalue(i-1,j-1,k))+abs(vorticity->lvalue(i-1,j,k))+abs(vorticity->lvalue(i,j,k)))/4);
				nX_y = abs(vorticity->evaluate(math::V3f(i,j+1,k)))-abs(vorticity->evaluate(math::V3f(i,j,k)));

				//if (nX_y != nX_y2 && abs(nX_y - nX_y2 )> 0.00001f )
				//	qCritical() << i << j << "diff: " << abs(nX_y - nX_y2 );


				float mag_nX = (float) sqrt(nX_x*nX_x + nX_y*nX_y);
				if(mag_nX > 0.00001f)
				{
					ny = nX_y / mag_nX;
					vel_x->lvalue(i,j,k) +=	-	ny * ( vorticity->lvalue(i,j,k) + vorticity->lvalue(i-1,j,k) ) * 0.5f * m_strenght * m_dt;
				}


				// vorticity gradient Y face ---
				// back diff
				nY_y = abs(vorticity->lvalue(i,j,k)) - abs(vorticity->lvalue(i,j-1,k));
				//nY_x = ((abs(vorticity->lvalue(i+1,j,k))+abs(vorticity->lvalue(i+1,j+1,k))+abs(vorticity->lvalue(i,j+1,k))+abs(vorticity->lvalue(i,j,k)))/4-
				//		(abs(vorticity->lvalue(i-1,j,k))+abs(vorticity->lvalue(i-1,j+1,k))+abs(vorticity->lvalue(i,j+1,k))+abs(vorticity->lvalue(i,j,k)))/4);
				nY_x = abs(vorticity->evaluate(math::V3f(i+1,j,k)))-abs(vorticity->evaluate(math::V3f(i,j,k)));

				float mag_nY = (float) sqrt(nY_y*nY_y + nY_x*nY_x);
				if(mag_nY > 0.00001f)
				{
					nx = nY_x / mag_nY;
					vel_y->lvalue(i,j,k) +=		nx * ( vorticity->lvalue(i,j,k) + vorticity->lvalue(i,j-1,k) ) * 0.5f * m_strenght * m_dt;
				}
*/

				//calc nabla n for x and y by central difference
				nab_nx = (abs(vorticity->lvalue(i+1,j,k)) - abs(vorticity->lvalue(i-1,j,k)));
				nab_ny = (abs(vorticity->lvalue(i,j+1,k)) - abs(vorticity->lvalue(i,j-1,k)));

				//calculate magnitude of the vector (nab_nx , nab_ny)
				//add small value to prevent divide by zero
				mag_n = (float) sqrt(nab_nx*nab_nx + nab_ny*nab_ny) + 0.00000001f;

				// normalize vector ->  N = (nabla n) / ||n||
				nx = nab_nx / mag_n;
				ny = nab_ny / mag_n;

				vortForce_x->lvalue(i,j,k) = -	ny * vorticity->lvalue(i,j,k);
				vortForce_y->lvalue(i,j,k) =	nx * vorticity->lvalue(i,j,k);
			}

		for( int j=2;j<res.y-2;++j )
			for( int i=2;i<res.x-2;++i )
			{
				vel_x->lvalue(i,j,k) +=		0.5*(vortForce_x->lvalue(i,j,k) + vortForce_x->lvalue(i-1,j,k) ) * m_strenght * m_dt;
				vel_y->lvalue(i,j,k) +=		0.5*(vortForce_y->lvalue(i,j,k) + vortForce_y->lvalue(i,j-1,k) ) * m_strenght * m_dt;
			}

		cd->setBounds2D(1,vel_x);
		cd->setBounds2D(2,vel_y);



}

void VortexConfinement2D::setField(QString name)
{
	m_vortField = name;
}

void VortexConfinement2D::setStrenght(float strenght)
{
	m_strenght = strenght;
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

	o.insert( "vortField", m_vortField );
	o.insert( "vortStrenght", m_strenght );
}


void VortexConfinement2D::load( QJsonObject &o )
{
	Operator::load( o );

	m_vortField = o["vortField"].toString();
	m_strenght = o["vortStrenght"].toDouble();
}

