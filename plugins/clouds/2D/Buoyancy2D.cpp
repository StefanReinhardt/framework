#include "Buoyancy2D.h"
#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>
#include <plugins/clouds/CloudData.h>

Buoyancy2D::Buoyancy2D()
{
	m_dt =                1.0f;
	m_buoyancy =          0.80f;	// Strength
	m_gravity =           9.81f;	// Strength
}



void Buoyancy2D::applyImpl(SimObject::Ptr so, float dt)
{
	timer.start();

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);

	math::Vec3i res = cd->getResolution();

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr qc = cd->getSubData<ScalarField>("qc");
	ScalarField::Ptr vel_y = cd->getSubData<VectorField>("velocity")->getScalarField(1);

	// B = (vpt-avpt)/ avpt - (g* qc)
	// vpt and avpt in Kelvin
	// g = 9.81 m/s²
	// qc in g/kg

	ScalarField::Ptr buoyForce;


	if(so->hasSubData("buoyancy") )
	{
		buoyForce = so->getSubData<ScalarField>("buoyancy");
	}
	else
	{
		buoyForce = std::make_shared<ScalarField>();
		buoyForce->resize(res);
		so->addSubData("buoyancy", buoyForce);
	}



	int k = 0;
	for (int i=0; i<res.x; i++)
		for (int j=0; j<res.y; j++)
		{
			float avpt = cd->m_tLut[j] * pow( cd->m_p0/cd->m_pLut[j], 0.286 ) * (1 + 0.61f * qv->lvalue(i,j,k) );
			float vpt = pt->lvalue(i,j,k) * ( 1 + 0.61f *  qv->lvalue(i,j,k) );

			float vpt_c = pt->lvalue(i,j,k) * ( 1 + 0.61f *  qv->lvalue(i,j,k) );

			buoyForce->lvalue(i,j,k) = dt * (m_buoyancy * ( math::max(0.0f,(vpt-avpt) / avpt )) - 0.08f*m_gravity * qc->lvalue(i,j,k) );

			buoyForce->lvalue(i,j,k) = dt * (m_buoyancy * ( math::max(0.0f,(vpt-avpt) / avpt )) - 0.08f*m_gravity * qc->lvalue(i,j,k) );
		}

	for (int i=1; i<res.x-1; i++)
		for (int j=1; j<res.y-1; j++)
		{
			vel_y->lvalue(i,j,k) += (buoyForce->lvalue(i,j,k)+ buoyForce->lvalue(i,j-1,k))*0.5f / cd->m_cellSize;
		}

	timer.stop();
	qCritical() << "Buoyancy:" << core::getVariable("$F").toString() << ":" << timer.elapsedSeconds();
}


void Buoyancy2D::setStrenght(float buoyancy)
{
	m_buoyancy = buoyancy;
}
void Buoyancy2D::setGravity(float gravity)
{
	m_gravity = gravity;
}



void Buoyancy2D::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "buoyancyStrength", m_buoyancy );
	o.insert( "gravityStrength", m_gravity );
}

void Buoyancy2D::load( QJsonObject &o )
{
	Operator::load( o );

	m_buoyancy = o["buoyancyStrength"].toDouble();
	m_gravity =  o["gravityStrength"].toDouble();
}
