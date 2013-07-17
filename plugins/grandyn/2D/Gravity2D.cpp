
/*
#include "Buoyancy2D.h"
//#include <core/Data.h>

#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>
#include <plugins/clouds/CloudData.h>

Buoyancy2D::Buoyancy2D()
{
	m_dt=1.0f;
}



void Buoyancy2D::apply(SimObject::Ptr so)
{

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
	m_dt = cd->m_parms.m_dt;

	math::Vec3i res = cd->getResolution();

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr qc = cd->getSubData<ScalarField>("qc");
	ScalarField::Ptr vel_y = cd->getSubData<VectorField>("velocity")->getScalarField(1);


	float vpt,avpt, t_m, p_m, qv_m, pt_m, qc_m;

	int k=0;
	for (int i=1; i<res.x-2; i++)
		for (int j=1; j<=res.y-2; j++){
			//avpt = (float) (absT[j] * ( Math.pow( (p0/absP[j]) , 0.286)))*( 1 + 0.61f * qv[i][j]);

			// calculate average temp at shifted velocity position
			t_m = ( cd->m_tLut.at(j));// + cd->tLut.at(j-1) )/2;
			p_m = ( cd->m_pLut.at(j));// + cd->pLut.at(j-1) )/2;
			qv_m =( qv->lvalue(i,j,k));// + qv->lvalue(i,j-1,k) )/2;
			pt_m = ( pt->lvalue(i,j,k));// + pt->lvalue(i,j-1,k) )/2;
			qc_m = ( qc->lvalue(i,j,k));// + qc->lvalue(i,j-1,k) )/2;

			avpt = t_m*  pow( cd->m_p0/p_m, 0.286 ) * (1 + 0.61f *qv_m );

			vpt = pt_m * ( 1 + 0.61f * qv_m );




			// B = (vpt-avpt)/ avpt - (g* qc)
			// vpt and avpt in Kelvin
			// g = 9.81 m/s²
			// qc in g/kg
			//vpt = pt[i][j] * ( 1 + 0.61f * qv[i][j]);


			//f[i][j] = k*( ( (vpt-avpt) / avpt ) - 9.81f * qc[i][j] );


			// TODO:
			// faktor vor gravity !?
			vel_y->lvalue(i,j,k) += m_dt*cd->m_parms.m_buoyancy *( ( (vpt-avpt) / avpt ) - cd->m_gravity *0.5f* qc_m );
			//vel_y->lvalue(i,j,k) += m_dt*cd->buoyancy *( cd->gravity*((vpt / avpt) -  qc_m) );

		}

}

*/
