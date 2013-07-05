#ifndef CloudDataBoundLayer_H
#define CloudDataBoundLayer_H

#include <plugins/sim/SimObject.h>
#include <vector>
#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>


class CloudDataBoundLayer  : public SimObject
{
public:

	typedef std::shared_ptr<CloudDataBoundLayer> Ptr;

	CloudDataBoundLayer();

	void							reset();
	void							setMinAltitude();

	math::V3i						getResolution();
	void							resize(math::V3i);

	float							getTimestep();
	void							setTimestep(float);

	void							setBounds(int, ScalarField::Ptr);
	void							setBounds2D(int, ScalarField::Ptr);


	math::V3i						resolution;

	float							dt;

	float							tlr;		// Temperature lapse rate in °C per 100meter
	float							hum;		// humidity in percent 0-1
	float							maxAlt;		// upper boarder of altitude in simulation in meter
	float							t0;			// ground temp
	float							p0;			// ground pressure
	float							pt0;		// ground pot temp
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
	std::vector<float>				qsLut;
/*
	ScalarField::Ptr				pt;
	ScalarField::Ptr				qv;
	ScalarField::Ptr				qc;

	VectorField::Ptr				velocity;

	ScalarField::Ptr				density;
*/


};

#endif // CloudDataBoundLayer_H
