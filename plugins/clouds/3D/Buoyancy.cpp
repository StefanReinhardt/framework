#include "Buoyancy.h"
//#include <core/Data.h>

#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>
#include <plugins/clouds/CloudData.h>

Buoyancy::Buoyancy()
{
	m_dt =                1.0f;
	m_buoyancy =          0.80f;	// Strength
}



void Buoyancy::apply(SimObject::Ptr so, float dt)
{

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
	m_dt = cd->m_p.dt;

	math::Vec3i res = cd->getResolution();

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr qc = cd->getSubData<ScalarField>("qc");
	ScalarField::Ptr vel_y = cd->getSubData<VectorField>("velocity")->getScalarField(1);

	ScalarField::Ptr buoyForce = std::make_shared<ScalarField>();
	buoyForce->resize(res);

	// B = (vpt-avpt)/ avpt - (g* qc)
	// vpt and avpt in Kelvin
	// g = 9.81 m/s²
	// qc in g/kg

	for (int k=0; k<res.z; k++)
		for (int i=0; i<res.x; i++)
			for (int j=0; j<res.y; j++)
			{
				float avpt = cd->m_tLut[j] * pow( cd->m_p0/cd->m_pLut[j], 0.286 ) * (1 + 0.61f * qv->lvalue(i,j,k) );
				float vpt = pt->lvalue(i,j,k) * ( 1 + 0.61f *  qv->lvalue(i,j,k) );
				buoyForce->lvalue(i,j,k) = m_dt * m_buoyancy *( ( (vpt-avpt) / avpt ) - cd->m_gravity * qc->lvalue(i,j,k) );
			}

	for (int k=1; k<res.z-1; k++)
		for (int i=1; i<res.x-1; i++)
			for (int j=1; j<res.y-1; j++)
				vel_y->lvalue(i,j,k) += (buoyForce->lvalue(i,j,k)+buoyForce->lvalue(i,j-1,k))*0.5f;


}


void Buoyancy::setStrenght(float buoyancy)
{
	m_buoyancy = buoyancy;
}


void Buoyancy::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "buoyancyStrength", m_buoyancy );
}

void Buoyancy::load( QJsonObject &o )
{
	Operator::load( o );

	m_buoyancy = o["buoyancyStrength"].toDouble();
}
