#include "CreateClouds.h"

#include <core/Core.h>
#include <plugins/sim/SimObject.h>
#include <plugins/clouds/CloudData.h>
#include <plugins/primitives/ScalarField.h>

CreateClouds::CreateClouds() : core::GraphNode()
{
    addOutputSocket( "output" );

	m_cloudParms.m_dt =          1.00f;
	m_cloudParms.m_maxAlt =      4000.0f;		// altitude in meter on top of sim grid
	m_cloudParms.m_tlr =         0.009f;		// Kelvin per 1 meter (between 0.55 and 0.99)
	m_cloudParms.m_t0 =          295.0f;		// temp on ground in Kelvin
	m_cloudParms.m_hum =         0.6f;			// humidty
	m_cloudParms.m_buoyancy =    0.8f;
	m_cloudParms.m_vorticity =   0.623456789f;
	m_cloudParms.m_wind =        0.0f;
	m_cloudParms.m_heatSrc =     3.0f;
	m_cloudParms.m_resolution =  math::Vec3i(100,100,100);



}

void CreateClouds::update(core::GraphNodeSocket *output)
{
    qDebug() << "CreateClouds: update ";

	// TODO: use user values

	CloudData::Ptr cd = std::make_shared<CloudData>(m_cloudParms);
	//CloudDataBoundLayer::Ptr cd = std::make_shared<CloudDataBoundLayer>();



	cd->print();
	getSocket( "output" )->setData(cd);
}


void CreateClouds::store( QJsonObject &o, QJsonDocument &doc )
{
	GraphNode::store(o, doc);

	// clouddata user parameters
	o.insert("timestep", m_cloudParms.m_dt);
	o.insert("maxAltitude", m_cloudParms.m_maxAlt);
	o.insert("tempreatureLapseRate", m_cloudParms.m_tlr);
	o.insert("groundTemperature", m_cloudParms.m_t0);
	o.insert("humidity", m_cloudParms.m_hum);
	o.insert("buoyancy", m_cloudParms.m_buoyancy);
	o.insert("vorticity", m_cloudParms.m_vorticity);
	o.insert("wind", m_cloudParms.m_wind);
	o.insert("heatSource", m_cloudParms.m_heatSrc);
	o.insert("resolution.x", m_cloudParms.m_resolution.x);
	o.insert("resolution.y", m_cloudParms.m_resolution.y);
	o.insert("resolution.z", m_cloudParms.m_resolution.z);
}


void CreateClouds::load( QJsonObject &o )
{
	GraphNode::load(o);

	m_cloudParms.m_dt=           o.value("timestep").toDouble();
	m_cloudParms.m_maxAlt=       o.value("maxAltitude").toDouble();
	m_cloudParms.m_tlr=          o.value("tempreatureLapseRate" ).toDouble();
	m_cloudParms.m_t0=           o.value("groundTemperature").toDouble();
	m_cloudParms.m_hum=          o.value("humidity" ).toDouble();
	m_cloudParms.m_buoyancy=     o.value("buoyancy" ).toDouble();
	m_cloudParms.m_vorticity=    o.value("vorticity" ).toDouble();
	m_cloudParms.m_wind=         o.value("wind" ).toDouble();
	m_cloudParms.m_heatSrc=      o.value("heatSource" ).toDouble();
	m_cloudParms.m_resolution.x= o.value("resolution.x").toDouble();
	m_cloudParms.m_resolution.y= o.value("resolution.y").toDouble();
	m_cloudParms.m_resolution.z= o.value("resolution.z" ).toDouble();
}
