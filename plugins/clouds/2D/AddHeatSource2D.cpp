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
	m_strenght =       1.0f;	// Temp input in 1/100 °C per second
	m_input =          BOTTOM;
	m_emitQv =         false;
	m_emitPt=          false;
	m_emitVel=         false;

}


void AddHeatSource2D::applyImpl(SimObject::Ptr so, float dt)
{
	timer.start();

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	math::V3i res = pt->getResolution();

	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr vel_y = cd->getSubData<VectorField>("velocity")->getScalarField(1);

	math::PerlinNoise noise = math::PerlinNoise();
	math::V3f wPos;
	float random;
	int k =                 0;
	float t  =              -core::getVariable("$F").toFloat() * m_animationSpeed;

	int min_x, min_y, max_x, max_y;

	switch (m_input)
	{
		case BOTTOM:
		{
			min_y =  0;
			max_y =  int(ceil(res.y/70.0f));
			min_x =  int(res.x*abs(m_emitterSize-1)*0.5f);
			max_x =  res.x-min_x;
		}break;

		case LEFT:
		{
			min_x =  0;
			max_x =  int(ceil(res.x/70.0f));
			min_y =  res.y/2.8;//int(res.y*abs(m_emitterSize-1)*0.5f);
			max_y =  res.y-1;//res.y-min_y;
		}break;

		case RIGHT:
		{
			min_x =  res.x-1-int(ceil(res.x/70.0f));
			max_x =  res.x-1;
			min_y =  0;//int(res.y*abs(m_emitterSize-1)*0.5f);
			max_y =  res.y/2.6;//res.y-min_y;
		}
	}

	cd->setBounds2D(5,qv);

	for (int j=min_y; j<max_y; ++j)
		for( int i=min_x; i<max_x; ++i )
		{
			// calculate world Position for resolution independent noise
			wPos = pt->voxelToWorld(math::V3f(float(i),float(j),float(k))) * m_frequency;

			// create noise value with given parameters
			random = abs(math::max(-1.0f,math::min(1.0f, m_offset + m_contrast*( noise.perlinNoise_3D(wPos.x,wPos.y,t) ) ) ) );

			if(m_emitPt)
				pt->lvalue(i,j,k) += 0.01f * m_strenght * random;

			if(m_emitQv)
				qv->lvalue(i,1,k) = qv->lvalue(i,0,k) + (m_strenght * random);
				//qv->lvalue(i,1,k) = qv->lvalue(i,0,k) * (1.10f + (0.20f * random));

			if(m_emitVel)
				vel_y->lvalue(i,j+1,k) = m_strenght * random;
				//vel_y->lvalue(i,j+1,k) = 2.6f * random;
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

void AddHeatSource2D::setStrenght(float strenght)
{
	m_strenght = strenght;
}

void AddHeatSource2D::setOffset(float offset)
{
	m_offset = offset;
}

void AddHeatSource2D::setInputFace(Input input)
{
	m_input = input;
}
void AddHeatSource2D::setQvEmission(bool emitQv)
{
	m_emitQv = emitQv;
}
void AddHeatSource2D::setPtEmission(bool emitPt)
{
	m_emitPt = emitPt;
}
void AddHeatSource2D::setVelEmission(bool emitVel)
{
	m_emitVel = emitVel;
}


void AddHeatSource2D::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "frequency", m_frequency );
	o.insert( "emitterSize", m_emitterSize);
	o.insert( "contrast", m_contrast);
	o.insert( "animationSpeed", m_animationSpeed);
	o.insert( "strenght", m_strenght);
	o.insert( "offset", m_offset);
	o.insert( "emitQv", m_emitQv);
	o.insert( "emitPt", m_emitPt);
	o.insert( "emitVel", m_emitVel);

	if(m_input == BOTTOM)		o.insert( "inputFace", 0);
	else if(m_input == LEFT)	o.insert( "inputFace", 1);
	else if(m_input == RIGHT)	o.insert( "inputFace", 2);
}



void AddHeatSource2D::load( QJsonObject &o )
{
	Operator::load( o );

	m_frequency =         o["frequency"].toDouble();
	m_emitterSize =       o["emitterSize"].toDouble();
	m_contrast =          o["contrast"].toDouble();
	m_animationSpeed =    o["animationSpeed"].toDouble();
	m_strenght =          o["strenght"].toDouble();
	m_offset =            o["offset"].toDouble();
	m_emitQv =            o["emitQv"].toBool();
	m_emitPt =            o["emitPt"].toBool();
	m_emitVel =           o["emitVel"].toBool();


	if(o["inputFace"].toDouble() == 0)		m_input = BOTTOM;
	else if(o["inputFace"].toDouble() == 1) m_input = LEFT;
	else if(o["inputFace"].toDouble() == 2) m_input = RIGHT;
}

