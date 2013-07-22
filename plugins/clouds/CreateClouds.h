#ifndef CREATECLOUDS_H
#define CREATECLOUDS_H

#include <core/GraphNode.h>
#include <plugins/primitives/ScalarField.h>
#include <plugins/clouds/CloudData.h>

class CreateClouds : public core::GraphNode
{
    Q_OBJECT
public:
    typedef std::shared_ptr<CreateClouds> Ptr;
	CreateClouds();
	virtual void					update( core::GraphNodeSocket *output )override;

	// Data
	virtual void                    store( QJsonObject &o, QJsonDocument &doc )override;
	virtual void                    load( QJsonObject &o )override;

private:
	CloudData::Parameters m_p;

//	float                            m_dt;
//	float                            m_maxAlt;		// altitude in meter on top of sim grid
//	float                            m_tlr;			// Kelvin per 1 meter (between 0.55 and 0.99)
//	float                            m_t0;			// temp on ground in Kelvin
//	float                            m_hum;			// humidty
//	float                            m_buoyancy;
//	float                            m_vorticity;
//	float                            m_wind;
//	float                            m_heatSrc;
//	math::V3i                        m_resolution;

};

#endif // CREATECLOUDS_H

