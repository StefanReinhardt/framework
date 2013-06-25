#ifndef CLOUDDATA_H
#define CLOUDDATA_H

#include <plugins/sim/SimObject.h>
#include <vector>
#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>


class CloudData  : public SimObject
{
public:

	typedef std::shared_ptr<CloudData> Ptr;

    CloudData();

	void							reset();

	math::V3i						getResolution();
	void							resize(math::V3i);

	float							getTimestep();
	void							setTimestep(float);

	void							setBounds(int, ScalarField::Ptr);

private:
	math::V3i						resolution;

	float							dt;

	float							tlr;		// Temperature lapse rate in °C per 100meter
	float							hum;		// humidity in percent 0-1
	float							maxAlt;		// upper boarder of altitude in simulation in meter
	float							t0;			// ground temp
	float							p0;			// ground pressure
	float							vorticity;
	float							buoyancy;
	float							gravity;

	float							diff; 		// Viscosity
	float							visc; 		// Diffusionrate
	float							wind;		// horizontal wind
	float							heatSrc;	// input temperature

	float							rd;
	float							lh;
	float							cp;
	float							exner;
	float							qs;

	std::vector<float>				tLut;		// absolute Temperature at altitude in K
	std::vector<float>				pLut;		// absolute Pressure at altitude in kPa

	ScalarField::Ptr				density;
	ScalarField::Ptr				vel_x;
	ScalarField::Ptr				vel_y;
	ScalarField::Ptr				vel_z;


};

#endif // CLOUDDATA_H
