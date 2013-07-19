#include "AddHeatSource2D.h"
#include <plugins/clouds/CloudData.h>
#include <time.h>


AddHeatSource2D::AddHeatSource2D()
{
	srand(time(NULL));

	m_frequency =      15.0f;	// frequency of the input noise
	m_contrast =       5.0f;	// contrast of the noise pattern
	m_emitterSize =    0.50f;	// size of the ground input field (between 0.0 and 1.0)
	m_offset =         0.0f;	// uplift of the noise (between -2 and 2)
	m_dt =             1.0f;	// timestep
	m_animationSpeed = 0.05f;	// speed of the animated noise
	m_tempInput =      5.0f;	// Temp input in 1/100 °C per second

}


void AddHeatSource2D::apply(SimObject::Ptr so, float dt)
{
	timer.start();

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
	m_dt = cd->m_parms.m_dt;

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	math::V3i res = pt->getResolution();

	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");

	math::PerlinNoise noise = math::PerlinNoise();
	math::V3f wPos;
	float random;
	int k =                 0;
	float t  =              -core::getVariable("$F").toFloat() * m_animationSpeed;

	// extension of the emission Field
	int height =            int(ceil(res.y/70.0f));
	int size =              int(res.x*abs(m_emitterSize-1)*0.5f);

	for (int j=0; j<height+1; ++j)
		for( int i=size; i<res.x-size; ++i )
		{
			// calculate world Position for resolution independent noise
			wPos = pt->voxelToWorld(math::V3f(float(i),float(j),float(k))) * m_frequency;

			// create noise value with given parameters
			random = abs(math::max(-1.0f,math::min(1.0f, m_offset + m_contrast*( noise.perlinNoise_3D(wPos.x,wPos.y,t) ) ) ) );
			pt->lvalue(i,j,k) += 0.01f * m_tempInput * random;
			qv->lvalue(i,j+1,k) += 0.0001f * random;
		}

	timer.stop();
	qCritical() << "AddHeatSource:" << core::getVariable("$F").toString() << ":" << timer.elapsedSeconds();
}


// --- SETTER ---------------------

void AddHeatSource2D::setAnimationSpeed(float speed)
{
	m_animationSpeed = speed;
}

void AddHeatSource2D::setContrast(float contrast)
{
	m_contrast = contrast;
}

void AddHeatSource2D::setEmitterSize(float size)
{
	m_emitterSize = size;
}

void AddHeatSource2D::setFrequence(float frequency)
{
	m_frequency = frequency;
}

void AddHeatSource2D::setTemperature(float temp)
{
	m_tempInput = temp;
}

void AddHeatSource2D::setOffset(float offset)
{
	m_offset = offset;
}

void AddHeatSource2D::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "frequency", m_frequency );
	o.insert( "emitterSize", m_emitterSize);
	o.insert( "contrast", m_contrast);
	o.insert( "animationSpeed", m_animationSpeed);
	o.insert( "temperature", m_tempInput);
	o.insert( "offset", m_offset);
}



void AddHeatSource2D::load( QJsonObject &o )
{
	Operator::load( o );

	m_frequency =         o["frequency"].toDouble();
	m_emitterSize =       o["emitterSize"].toDouble();
	m_contrast =          o["contrast"].toDouble();
	m_animationSpeed =    o["animationSpeed"].toDouble();
	m_tempInput =         o["temperature"].toDouble();
	m_offset =            o["offset"].toDouble();
}

