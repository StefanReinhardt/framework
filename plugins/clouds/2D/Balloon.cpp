#include "Balloon.h"
#include <plugins/clouds/CloudData.h>
#include <core/Data.h>

#include <fstream>
using namespace std;

Balloon::Balloon()
{
	m_release = 200;
}


void Balloon::applyImpl( SimObject::Ptr so, float dt )
{
	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
	m_posx = cd->getResolution().x*0.5f;
	m_posy = 2.0f;

	// advect
	if(core::getVariable("$F").toInt()>m_release)
	{
		m_posx = m_posx + dt * so->getSubData<VectorField>("velocity")->getScalarField(0)->evaluate(math::V3f(m_posx,m_posy,0));
		m_posy = m_posy + dt * so->getSubData<VectorField>("velocity")->getScalarField(1)->evaluate(math::V3f(m_posx,m_posy,0));
	}

	// write data
	fstream f;

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	f.open("pt", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << pt->evaluate(math::Vec3f(m_posx,m_posy,0)) << endl;
	f.close();

	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	f.open("qv", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << qv->evaluate(math::Vec3f(m_posx,m_posy,0)) << endl;
	f.close();

	ScalarField::Ptr qc = cd->getSubData<ScalarField>("qc");
	f.open("qc", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << qc->evaluate(math::Vec3f(m_posx,m_posy,0)) << endl;
	f.close();

	ScalarField::Ptr vel_x = cd->getSubData<VectorField>("velocity")->getScalarField(0);
	f.open("vel_x", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << vel_x->evaluate(math::Vec3f(m_posx,m_posy,0)) << endl;
	f.close();

	ScalarField::Ptr vel_y = cd->getSubData<VectorField>("velocity")->getScalarField(1);
	f.open("vel_y", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << vel_y->evaluate(math::Vec3f(m_posx,m_posy,0)) << endl;
	f.close();

	f.open("pos_x", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << m_posx << endl;
	f.close();

	f.open("pos_y", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << m_posy << endl;
	f.close();
}


void Balloon::setPosition(float x, float y)
{
	m_posx = x;
	m_posy = y;
}

void Balloon::setReleaseFrame(int release)
{
	m_release = release;
}


void Balloon::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "m_posx", m_posx );
	o.insert( "m_posy", m_posy);
	o.insert( "m_release", m_release);
}

void Balloon::load( QJsonObject &o )
{
	Operator::load( o );

	m_posx = o["m_posx"].toDouble();
	m_posy = o["m_posy"].toDouble();
	m_release = int(o["m_release"].toDouble());
}

