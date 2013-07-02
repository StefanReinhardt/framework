#include "Buoyancy.h"
//#include <core/Data.h>

#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>
#include <plugins/clouds/CloudData.h>

Buoyancy::Buoyancy()
{
}



void Buoyancy::apply(SimObject::Ptr so)
{

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);

	math::Vec3i res = cd->getResolution();

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr qc = cd->getSubData<ScalarField>("qc");
	ScalarField::Ptr vel_y = cd->getSubData<VectorField>("velocity")->getScalarField(1);


	float vpt,avpt;

	int k=0;
	for (int i=1; i<res.x-2; i++)
		for (int j=1; j<=res.y-2; j++){
			//avpt = (float) (absT[j] * ( Math.pow( (p0/absP[j]) , 0.286)))*( 1 + 0.61f * qv[i][j]);

			// TODO:
			// average before avpt
			avpt = cd->tLut.at(j)*  pow( cd->p0/cd->pLut.at(j), 0.286 ) * (1 + 0.61f *qv->lvalue(i,j,k) );
			//avpt += cd->tLut.at(j-1)*  pow( cd->p0/cd->pLut.at(j-1), 0.286 ) * (1 + 0.61f *qv->lvalue(i,j-1,k) );
			//avpt /= 2;

			vpt = pt->lvalue(i,j,k) * ( 1 + 0.61f * qv->lvalue(i,j,k) );
			//vpt = pt->lvalue(i,j-1,k) * ( 1 + 0.61f * qv->lvalue(i,j-1,k) );
			//vpt /= 2;

			float qc_mid = ( qc->lvalue(i,j,k) + qc->lvalue(i,j-1,k) )/2;

			// B = (vpt-avpt)/ avpt - (g* qc)
			// vpt and avpt in Kelvin
			// g = 9.81 m/s²
			// qc in g/kg
			//vpt = pt[i][j] * ( 1 + 0.61f * qv[i][j]);


			//f[i][j] = k*( ( (vpt-avpt) / avpt ) - 9.81f * qc[i][j] );

			vel_y->lvalue(i,j,k) += cd->buoyancy *( ( (vpt-avpt) / avpt ) - cd->gravity * qc_mid );

		}

}

