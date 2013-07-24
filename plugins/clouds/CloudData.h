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
			dt =                       1.0f;
			maxAlt =                   6000.0f;		// altitude in meter on top of sim grid
			minAlt =                   2000.0f;
			tlr =                      0.009f;		// Kelvin per 1 meter (between 0.55 and 0.99) standard (wiki) 6.4K/km
			t0 =                       295.0f;		// temp on ground in Kelvin
			hum =                      0.6f;			// humidty
			wind =                     0.0f;
			res =                      math::Vec3i(50,50,1);
		}

		float                            dt;
		float                            maxAlt;		// altitude in meter on top of sim grid
		float                            minAlt;		// altitude in meter on ground of sim grid
		float                            tlr;			// Kelvin per 1 meter (between 0.55 and 0.99)
		float                            t0;			// temp on ground in Kelvin
		float                            hum;			// humidty
		float                            wind;
		math::V3i                        res;
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


	Parameters                           m_p;

	math::V3i                            m_res;
	float                                m_gravity;
	float                                m_diff;			// Viscosity
	float                                m_visc;			// Diffusionrate
	float                                m_p0;				// ground pressure
	float                                m_pt0;				// ground pot temp
	float                                m_rd;
	float                                m_lh;
	float                                m_cp;
	float                                m_exner;
	float                                m_qs;
	float                                m_cellSize;

	float                                m_qv1;

	std::vector<float>                   m_tLut;			// absolute Temperature at altitude in K
	std::vector<float>                   m_pLut;			// absolute Pressure at altitude in kPa
	std::vector<float>                   m_qv0Lut;			// initial Vapor distribution Lookup


};

#endif // CLOUDDATA_H
