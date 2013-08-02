#include "Balloon.h"
#include <plugins/clouds/CloudData.h>
#include <core/Data.h>

#include <fstream>
using namespace std;

Balloon::Balloon()
{
	m_release = 200;

	// reset files ---
	fstream f;
	f.open("pt", std::ios_base::trunc | std::ios_base::out);
	f.close();
	f.open("qv", std::ios_base::trunc | std::ios_base::out);
	f.close();
	f.open("qc", std::ios_base::trunc | std::ios_base::out);
	f.close();
	f.open("vel_x", std::ios_base::trunc | std::ios_base::out);
	f.close();
	f.open("vel_y", std::ios_base::trunc | std::ios_base::out);
	f.close();
	f.open("pos_x", std::ios_base::trunc | std::ios_base::out);
	f.close();
	f.open("pos_y", std::ios_base::trunc | std::ios_base::out);
	f.close();
}


void Balloon::applyImpl( SimObject::Ptr so, float dt )
{
	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
	int frame = core::getVariable("$F").toInt();

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr qc = cd->getSubData<ScalarField>("qc");
	VectorField::Ptr vel = cd->getSubData<VectorField>("velocity");
	math::V3f v(0.0f); // current balloon velocity

	// as long as we havent released yet...
	if(frame <= m_release)
	{
		//...reset balloon position
		m_vsP.x = cd->getResolution().x*0.5f;
		m_vsP.y = 2.0f;
		m_vsP.z = 0.5f;
	}else
	{
		// ...else advect balloon position along velocity field

		// convert balloon position to worldspace
		math::V3f wsP = qc->voxelToWorld(m_vsP);
		// evaluate velocity field (nb: vectorfield::evaluate needs worldspace point)
		v = vel->evaluate(wsP);
		// now do a single eulerstep in voxelspace since velocity is given in voxelspace
		m_vsP += dt*v;
	}

	cd->m_info["balloon_x"] = m_vsP.x;
	cd->m_info["balloon_y"] = m_vsP.y;
	cd->m_info["balloon_qv"] = qv->evaluate(m_vsP);
	cd->m_info["balloon_qc"] = qc->evaluate(m_vsP);
	cd->m_info["balloon_pt"] = pt->evaluate(m_vsP);
	cd->m_info["balloon_vel_x"] = v.x;
	cd->m_info["balloon_vel_y"] = v.y;


	// write data ----
	fstream f;

	f.open("pt", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << cd->m_info["balloon_pt"].toFloat() << endl;
	f.close();

	f.open("qv", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << cd->m_info["balloon_qv"].toFloat() << endl;
	f.close();

	f.open("qc", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << cd->m_info["balloon_qc"].toFloat() << endl;
	f.close();

	f.open("vel_x", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << cd->m_info["balloon_vel_x"].toFloat() << endl;
	f.close();

	f.open("vel_y", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << cd->m_info["balloon_vel_y"].toFloat() << endl;
	f.close();

	f.open("pos_x", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << cd->m_info["balloon_x"].toFloat() << endl;
	f.close();

	f.open("pos_y", std::ios_base::app | std::ios_base::out);
	f << core::getVariable("$F").toInt() << " " << cd->m_info["balloon_y"].toFloat() << endl;
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

