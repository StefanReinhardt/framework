#include "AddHeatSource2D.h"
#include <plugins/clouds/CloudData.h>
#include <time.h>
#include <core/Core.h>

AddHeatSource2D::AddHeatSource2D()
{
	srand(time(NULL));
}

void AddHeatSource2D::apply(SimObject::Ptr so)
{
	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);

	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	math::V3i res = pt->getResolution();




	int k = 0;
	int min= cd->pt0;
	int max = min+15;
	std::vector<float> random;

	for( int i=0;i<res.x;++i )
	{
		random.push_back(min + rand()%(max-min+1));
	}




	// Ground Vapor Saturation Ratio
	float exner = pow(cd->pLut.at(1)/cd->p0,0.286f);
	float T = pt->lvalue(1,1,0)*exner;
	T -= 273.15f;
	float qs= (float) ( (380.16f / (cd->pLut.at(1)*1000) ) * exp( (17.67f * T) / (T + 243.5f) ) );
	float t_env=283.8f;

	for( int i=1;i<res.x-1;++i )
	{
		pt->lvalue(i,1,k) = min + rand()%(max-min+1); //(r1+r2+r4)/3;

		//if(i>31 && i<69)
			//qv->lvalue(i,1,k) = qs*0.10*math::max((core::getVariable("$F").toFloat()/50.0f),1.0f);
	}
}
