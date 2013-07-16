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



void Buoyancy::apply(SimObject::Ptr so)
{

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
	m_dt = cd->m_parms.m_dt;

	math::Vec3i res = cd->getResolution();

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr qc = cd->getSubData<ScalarField>("qc");
	ScalarField::Ptr vel_y = cd->getSubData<VectorField>("velocity")->getScalarField(1);

	// B = (vpt-avpt)/ avpt - (g* qc)
	// vpt and avpt in Kelvin
	// g = 9.81 m/s²
	// qc in g/kg

	float vpt,avpt;
	float t_mid;
	float p_mid;
	float qv_mid;
	float qc_mid;

	for (int k=1; k<res.z-2; k++)
		for (int i=1; i<res.x-2; i++)
			for (int j=1; j<=res.y-2; j++)
			{
				t_mid = (cd->m_tLut[j]+cd->m_tLut[j-1])/2;
				p_mid = (cd->m_pLut[j]+cd->m_pLut[j-1])/2;
				qv_mid = ( qv->lvalue(i,j,k) + qv->lvalue(i,j-1,k) )/2;
				qc_mid = ( qc->lvalue(i,j,k) + qc->lvalue(i,j-1,k) )/2;

				avpt = t_mid * pow( cd->m_p0/p_mid, 0.286 ) * (1 + 0.61f *qv_mid );
				vpt = pt->lvalue(i,j,k) * ( 1 + 0.61f * qv_mid );

				vel_y->lvalue(i,j,k) += m_dt * m_buoyancy *( ( (vpt-avpt) / avpt ) - cd->m_gravity * qc_mid );

			}

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
