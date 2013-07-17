#include "AddHeatSource.h"
#include <plugins/clouds/CloudData.h>
#include <time.h>
#include <core/Core.h>

AddHeatSource::AddHeatSource()
{
	srand(time(NULL));
	m_frequency =      15.0f;	// frequency of the input noise
	m_contrast =       5.0f;	// contrast of the noise pattern
	m_emitterSize =    0.50f;	// size of the ground input field (between 0.0 and 1.0)
	m_add =            0.0f;	// uplift of the noise
	m_dt =             1.0f;	// timestep
	m_animationSpeed = 0.05f;	// speed of the animated noise
	m_tempInput =      5.0f;	// Temp input in 1/100 °C per second
}

void AddHeatSource::apply(SimObject::Ptr so, float dt)
{
	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
	m_dt = cd->m_parms.m_dt;

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	math::V3i res = pt->getResolution();

	float t  = -core::getVariable("$F").toFloat()*m_animationSpeed;
	math::PerlinNoise noise = math::PerlinNoise();
	float random;

	math::V3f wPos;

	int sizex =          int(res.x*abs(m_emitterSize-1)*0.5f);
	int sizez =          int(res.y*abs(m_emitterSize-1)*0.5f);
	math::V3i min =      math::V3i(sizex,0,sizez);
	math::V3i max =      math::V3i(res.x-sizex, int(ceil(res.y/70.0f))+1, res.z-sizez);

	for( int k=min.z; k<max.z; ++k )
		for (int j=min.y; j<max.y; ++j)
			for( int i=min.x; i<max.x; ++i )
			{
				wPos =   pt->voxelToWorld(math::V3f(float(i),float(j),float(k)));
				wPos *= m_frequency;
				random = abs(math::max(-1.0f,math::min(1.0f, m_contrast*(m_add+noise.perlinNoise_4D(wPos.x,wPos.y,wPos.z,t) ) ) ) );
				pt->lvalue(i,j,k) += 0.01 * m_dt * m_tempInput * random;
			}
}

// --- SETTER ---------------------

void AddHeatSource::setAnimationSpeed(float speed)
{
	m_animationSpeed = speed;
}

void AddHeatSource::setContrast(float contrast)
{
	m_contrast = contrast;
}

void AddHeatSource::setEmitterSize(float size)
{
	m_emitterSize = size;
}

void AddHeatSource::setFrequence(float frequency)
{
	m_frequency = frequency;
}
void AddHeatSource::setTemperature(float temp)
{
	m_tempInput = temp;
}


void AddHeatSource::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "frequency", m_frequency );
	o.insert( "emitterSize", m_emitterSize);
	o.insert( "contrast", m_contrast);
	o.insert( "animationSpeed", m_animationSpeed);
	o.insert( "temperature", m_tempInput);
}



void AddHeatSource::load( QJsonObject &o )
{
	Operator::load( o );

	m_frequency =         o["frequency"].toDouble();
	m_emitterSize =       o["emitterSize"].toDouble();
	m_contrast =          o["contrast"].toDouble();
	m_animationSpeed =    o["animationSpeed"].toDouble();
	m_tempInput =         o["temperature"].toDouble();
}
