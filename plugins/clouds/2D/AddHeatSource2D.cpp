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
	int min= cd->m_pt0;
	int max = min+cd->m_parms.m_heatSrc;
	std::vector<float> random;

	for( int i=0;i<res.x;++i )
	{
		random.push_back(min + rand()%(max-min+1));
	}



	math::PerlinNoise noise = math::PerlinNoise();
	float contrast = 5.0f;
	float add = 0.0f;

	float t  = -core::getVariable("$F").toFloat()*0.1f;
	// Ground Vapor Saturation Ratio
	//float exner = pow(cd->pLut.at(1)/cd->p0,0.286f);
	//float T = pt->lvalue(1,1,0)*exner;
	//T -= 273.15f;
	//float qs= (float) ( (380.16f / (cd->pLut.at(1)*1000) ) * exp( (17.67f * T) / (T + 243.5f) ) );
	//float t_env=283.8f;


	int height = int(ceil(res.y/70.0f));
	qCritical() << height;
	int size = int(res.x/3.3);
	for (int j=0; j<height+1; ++j)
		for( int i=size;i<res.x-size;++i )
		{
			//pt->lvalue(i,1,k) = min + (rand()%(max-min+1))*0.5*(1.2+sin(0.01f*core::getVariable("$F").toFloat())); //(r1+r2+r4)/3;
			//pt->lvalue(i,0,k) = pt->lvalue(i,1,k);

			pt->lvalue(i,j,k) = abs(math::max(-1.0f,math::min(1.0f, contrast*(add+noise.perlinNoise_2D((float)i,(float)(t+j))))));
			pt->lvalue(i,j,k) = pt->lvalue(i,j,k) * cd->m_parms.m_heatSrc + cd->m_pt0;


	//		pt->lvalue(i,0,k) = abs(noise.perlinNoise_2D(float(i),t+0.0f))*cd->heatSrc+cd->pt0;
	//		pt->lvalue(i,1,k) = abs(noise.perlinNoise_2D(float(i),t+1.0f))*cd->heatSrc+cd->pt0;
	//		pt->lvalue(i,2,k) = abs(noise.perlinNoise_2D(float(i),t+2.0f))*cd->heatSrc+cd->pt0;
	//		pt->lvalue(i,3,k) = abs(noise.perlinNoise_2D(float(i),t+3.0f))*cd->heatSrc+min;
	//		pt->lvalue(i,4,k) = abs(noise.perlinNoise_2D(float(i),t+4.0f))*cd->heatSrc+min;
	//		pt->lvalue(i,5,k) = abs(noise.perlinNoise_2D(float(i),t+5.0f))*cd->heatSrc+min;
	//		pt->lvalue(i,6,k) = abs(noise.perlinNoise_2D(float(i),t+6.0f))*cd->heatSrc+min;
	//		pt->lvalue(i,7,k) = abs(noise.perlinNoise_2D(float(i),t+7.0f))*cd->heatSrc+min;


			//qv->lvalue(i,0,k) += 0.0015;
			//qv->lvalue(i,1,k) += 0.0015; //(r1+r2+r4)/3;

			//if(i>31 && i<69)
				//qv->lvalue(i,1,k) = qs*0.10*math::max((core::getVariable("$F").toFloat()/50.0f),1.0f);
		}
}
