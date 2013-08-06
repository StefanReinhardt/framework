#include "CreateClouds.h"

#include <core/Core.h>
#include <plugins/sim/SimObject.h>
#include <plugins/clouds/CloudData.h>
#include <plugins/primitives/ScalarField.h>

CreateClouds::CreateClouds() : core::GraphNode()
{
	addOutputSocket( "output" );
	addInputSocket( "frame" );


	m_p.dt =          1.00f;
	m_p.maxAlt =      6000.0f;		// altitude in meter on top of sim grid
	m_p.minAlt =      0000.0f;		// altitude in meter on ground of sim grid

	// Temperature Lapse rate		Kelvin per 1 meter (between 0.55 and 0.99)
	m_p.tlr =         0.007f;		// controlls how likely clouds will form

	// Humidity of the air
	m_p.hum =         0.6f;			// controls the altitude of the condensation level

	m_p.t0 =          300.0f;		// temp on ground in Kelvin
	m_p.wind =        0.00f;
	m_p.res =         math::Vec3i(128, 128, 1);

	//scale = (m_p.maxAlt-m_p.minAlt)/m_p.res.y;
	//math::Vec3f size = math::Vec3f(scale*m_p.res.x, scale*m_p.res.y, scale*m_p.res.z);

}

void CreateClouds::update(core::GraphNodeSocket *output)
{
	qDebug() << "CreateClouds: update ";

	// get current frame
	int frame = getSocket( "frame" )->asInt();

	// (re)set cloud data when frame==1 or no output was written yet
	if( frame == 1 || !getSocket( "output" )->getDataWithoutUpdate<CloudData>())
	{
		// TODO: use user values
		CloudData::Ptr cd = std::make_shared<CloudData>(m_p);
		getSocket( "output" )->setData(cd);
	}
}


void CreateClouds::store( QJsonObject &o, QJsonDocument &doc )
{
	GraphNode::store(o, doc);

	// clouddata user parameters
	o.insert("timestep", m_p.dt);
	o.insert("maxAltitude", m_p.maxAlt);
	o.insert("minAltitude", m_p.minAlt);
	o.insert("tempreatureLapseRate", m_p.tlr);
	o.insert("groundTemperature", m_p.t0);
	o.insert("humidity", m_p.hum);
	o.insert("wind", m_p.wind);
	o.insert("resolution.x", m_p.res.x);
	o.insert("resolution.y", m_p.res.y);
	o.insert("resolution.z", m_p.res.z);
}


void CreateClouds::load( QJsonObject &o )
{
	GraphNode::load(o);

	m_p.dt=           o.value("timestep").toDouble();
	m_p.maxAlt=       o.value("maxAltitude").toDouble();
	m_p.minAlt=       o.value("minAltitude").toDouble();
	m_p.tlr=          o.value("tempreatureLapseRate" ).toDouble();
	m_p.t0=           o.value("groundTemperature").toDouble();
	m_p.hum=          o.value("humidity" ).toDouble();
	m_p.wind=         o.value("wind" ).toDouble();
	m_p.res.x= o.value("resolution.x").toDouble();
	m_p.res.y= o.value("resolution.y").toDouble();
	m_p.res.z= o.value("resolution.z" ).toDouble();
}
