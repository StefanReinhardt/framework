#ifndef CLOUDDATA_H
#define CLOUDDATA_H

#include <plugins/sim/SimObject.h>
#include <vector>
#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>


class CloudData  : public SimObject
{
public:
	struct Parameters
	{
		Parameters()
		{
			m_dt =                       1.0f;
			m_maxAlt =                   4000.0f;		// altitude in meter on top of sim grid
			m_tlr =                      0.009f;		// Kelvin per 1 meter (between 0.55 and 0.99)
			m_t0 =                       295.0f;		// temp on ground in Kelvin
			m_hum =                      0.6f;			// humidty
			m_buoyancy =                 0.8f;
			m_vorticity =                0.05f;
			m_wind =                     0.0f;
			m_heatSrc =                  10.0f;
			m_resolution =               math::Vec3i(50,50,1);
		}

		float                            m_dt;
		float                            m_maxAlt;		// altitude in meter on top of sim grid
		float                            m_tlr;			// Kelvin per 1 meter (between 0.55 and 0.99)
		float                            m_t0;			// temp on ground in Kelvin
		float                            m_hum;			// humidty
		float                            m_buoyancy;
		float                            m_vorticity;
		float                            m_wind;
		float                            m_heatSrc;
		math::V3i                        m_resolution;
	};

	typedef std::shared_ptr<CloudData> Ptr;

    CloudData();
	CloudData( Parameters parms );

	void                                 reset();
	void                                 initialize();


	math::V3i                            getResolution();
	void                                 resize(math::V3i);

	float                                getTimestep();
	void                                 setTimestep(float);

	void                                 setBounds(int, ScalarField::Ptr);
	void                                 setBounds2D(int, ScalarField::Ptr);


	Parameters                           m_parms;

	math::V3i                            m_resolution;
	float                                m_gravity;
	float                                m_diff;			// Viscosity
	float                                m_visc;			// Diffusionrate
	float                                m_p0;			// ground pressure
	float                                m_pt0;			// ground pot temp
	float                                m_rd;
	float                                m_lh;
	float                                m_cp;
	float                                m_exner;
	float                                m_qs;

	std::vector<float>                   m_tLut;			// absolute Temperature at altitude in K
	std::vector<float>                   m_pLut;			// absolute Pressure at altitude in kPa


};

#endif // CLOUDDATA_H
