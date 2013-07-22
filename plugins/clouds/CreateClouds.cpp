#include "CreateClouds.h"

#include <core/Core.h>
#include <plugins/sim/SimObject.h>
#include <plugins/clouds/CloudData.h>
#include <plugins/primitives/ScalarField.h>

CreateClouds::CreateClouds() : core::GraphNode()
{
    addOutputSocket( "output" );


	m_p.dt =          1.00f;
	m_p.maxAlt =      6000.0f;		// altitude in meter on top of sim grid
	m_p.minAlt =      2000.0f;		// altitude in meter on ground of sim grid
	m_p.tlr =         0.006f;		// Kelvin per 1 meter (between 0.55 and 0.99)
	m_p.t0 =          295.0f;		// temp on ground in Kelvin
	m_p.hum =         0.95f;		// humidty
	m_p.wind =        0.0f;
	m_p.resolution =  math::Vec3i(100,100,1);

	// Temperature Lapse rate		Kelvin per 1 meter (between 0.55 and 0.99)
	m_p.tlr =         0.006f;		// controlls how likely clouds will form

	// Humidity of the air
	m_p.hum =         0.6f;			// controls the altitude of the condensation level

	m_p.t0 =          300.0f;		// temp on ground in Kelvin
	m_p.wind =        0.00f;
	m_p.res =  math::Vec3i(160,160,1);

	//scale = (m_p.maxAlt-m_p.minAlt)/m_p.res.y;
	//math::Vec3f size = math::Vec3f(scale*m_p.res.x, scale*m_p.res.y, scale*m_p.res.z);

}

void CreateClouds::update(core::GraphNodeSocket *output)
{
    qDebug() << "CreateClouds: update ";

	// TODO: use user values

	CloudData::Ptr cd = std::make_shared<CloudData>(m_p);
	//CloudDataBoundLayer::Ptr cd = std::make_shared<CloudDataBoundLayer>();



	cd->print();
	getSocket( "output" )->setData(cd);
}


void CreateClouds::store( QJsonObject &o, QJsonDocument &doc )
{
	GraphNode::store(o, doc);

	// clouddata user parameters
	o.insert("timestep", m_p.dt);
	o.insert("maxAltitude", m_p.maxAlt);
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
	m_p.tlr=          o.value("tempreatureLapseRate" ).toDouble();
	m_p.t0=           o.value("groundTemperature").toDouble();
	m_p.hum=          o.value("humidity" ).toDouble();
	m_p.wind=         o.value("wind" ).toDouble();
	m_p.res.x= o.value("resolution.x").toDouble();
	m_p.res.y= o.value("resolution.y").toDouble();
	m_p.res.z= o.value("resolution.z" ).toDouble();
}
