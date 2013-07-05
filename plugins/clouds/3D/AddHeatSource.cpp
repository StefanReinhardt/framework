#include "AddHeatSource.h"
#include <plugins/clouds/CloudData.h>
#include <time.h>
#include <core/Core.h>

AddHeatSource::AddHeatSource()
{
	srand(time(NULL));
}

void AddHeatSource::apply(SimObject::Ptr so)
{
	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);

	//ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	math::V3i res = pt->getResolution();



	if(core::getVariable("$F").toInt()<300000)
	{
		int k = 0;
		int min= cd->pt0;
		int max = min+cd->heatSrc;
		std::vector<float> random;

		for( int i=0;i<res.x;++i )
		{
			random.push_back(min + rand()%(max-min+1));
		}


		for( int k=10;k<res.z-11;++k )
			for( int i=10;i<res.x-11;++i )
			{
				//int index = k*i+i
				float r4 = (random.at(i-2)+random.at(i-1)+random.at(i)+random.at(i+1))/4;
				float r2 = (random.at(i)+random.at(i+1))/2;
				float r1 = random.at(i);

				pt->lvalue(i,1,k) = min + rand()%(max-min+1); //(r1+r2+r4)/3;
				//qv->lvalue(i,1,k) = qv->lvalue(i,1,k)+((rand()%1000)*(qv->lvalue(i,1,k)/6))/1000; //(r1+r2+r4)/3;
			}
	}
}
