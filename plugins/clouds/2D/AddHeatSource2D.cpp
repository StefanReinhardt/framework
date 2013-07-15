#include "AddHeatSource2D.h"
#include <plugins/clouds/CloudData.h>
#include <time.h>
#include <core/Core.h>

AddHeatSource2D::AddHeatSource2D()
{
	srand(time(NULL));
	m_scale = 30.0f;
	m_contrast = 5.0f;
	m_add = 0.0f;
	m_dt = 1.0f;
}

void AddHeatSource2D::apply(SimObject::Ptr so)
{
	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
	m_dt = cd->m_parms.m_dt;

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	math::V3i res = pt->getResolution();

	int k = 0;
	float t  = -core::getVariable("$F").toFloat()*0.05f;
	float random;
	math::PerlinNoise noise = math::PerlinNoise();

	math::V3f wPos;

	int height = int(ceil(res.y/70.0f));
	int size = int(res.x/3.3);
	for (int j=0; j<height+1; ++j)
		for( int i=size;i<res.x-size;++i )
		{
			wPos =   pt->voxelToWorld(math::V3f(float(i),float(j),float(k))) * m_scale;
			random = abs(math::max(-1.0f,math::min(1.0f, m_contrast*(m_add+noise.perlinNoise_3D(wPos.x,wPos.y,t) ) ) ) );
			pt->lvalue(i,j,k) += m_dt * cd->m_parms.m_heatSrc * random;
		}
}
