#include "AddHeatSource.h"
#include <plugins/clouds/CloudData.h>
#include <time.h>
#include <core/Core.h>

AddHeatSource::AddHeatSource()
{
	srand(time(NULL));
	m_scale = 30.0f;
	m_contrast = 5.0f;
	m_add = 0.0f;
	m_dt = 1.0;
}

void AddHeatSource::apply(SimObject::Ptr so)
{
	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
	m_dt = cd->m_parms.m_dt;

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	math::V3i res = pt->getResolution();

	float t  = -core::getVariable("$F").toFloat()*0.05f;
	math::PerlinNoise noise = math::PerlinNoise();
	float random;

	math::V3f wPos;
	math::V3f min = math::V3f(res.x/3.3f,-0.01f,res.z/3.3f);
	math::V3f max = math::V3f(res.x-res.x/3.3f, ceil(res.y/70.0f)+1, res.z-res.z/3.3f);
	min = pt->voxelToWorld(min);
	max = pt->voxelToWorld(max);


	math::BoundingBox3f tempInput = math::BoundingBox3f(min, max);

	for( int k=0;k<res.z;++k )
		for (int j=0; j<res.y; ++j)
			for( int i=0;i<res.x;++i )
			{
				wPos =   pt->voxelToWorld(math::V3f(float(i),float(j),float(k)));

				// If input box is within pt box
				if(tempInput.encloses(wPos))
				{
					wPos *= m_scale;
					random = abs(math::max(-1.0f,math::min(1.0f, m_contrast*(m_add+noise.perlinNoise_4D(wPos.x,wPos.y,wPos.z,t) ) ) ) );
					pt->lvalue(i,j,k) += 0.1 * m_dt * cd->m_parms.m_heatSrc;
				}
			}
}
