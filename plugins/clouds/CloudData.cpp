#include "CloudData.h"


CloudData::CloudData() : SimObject()
{
	dt = 1.0f;

	diff = 0.0000f;         // diffusion
	visc = 0.000000000f;    // viscosity

	//Constants
	rd = 		287; 		// specific gas constant for dry air
	p0 = 		100;		// pressure at sea level (kPa)
	gravity = 	9.81f;		// gravitational acceleration (m/s²)
	lh = 		2501;		// Latent heat of vaporization of water (J/kg)
	cp = 		1005;		// specific heat capacity J/(kg K)
	tlr =       0.6f;		// Temperature lapse rate in °C per 100meter

	maxAlt = 	4000;		// altitude in meter on top of sim grid
	tlr = 		0.009f; 	// Kelvin per 1 meter (between 0.55 and 0.99)
	t0 = 		295;		// temp on ground in Kelvin
	hum = 		0.6f;		// humidty
	buoyancy =  0.8f;
	vorticity = 0.2f;
	wind = 		0.1f;
	heatSrc = 	20.0f;

	resolution = math::Vec3i(40,40,5);


	density = std::make_shared<ScalarField>();
	density->resize(resolution);
	density->localToWorld(math::V3f(2,2,1));
	density->fill(0.0f);
	density->fill(33.3f,math::Box3f(0.4f,0.4f,0,0.6f,0.6f,1.0f));
	addSubData("density", density);

	vel_x = std::make_shared<ScalarField>();
	vel_x->resize(resolution);
	vel_x->localToWorld(math::V3f(2,2,1));
	vel_x->fill(0.0f);
	vel_x->fill(1,math::Box3f(0.4f,0.55f,0,0.8f,0.8f,1.0f));
	addSubData("vel_x", vel_x );

	vel_y = std::make_shared<ScalarField>();
	vel_y->resize(resolution);
	vel_y->localToWorld(math::V3f(2,2,1));
	vel_y->fill(0.0f);
	vel_y->fill(1,math::Box3f(0.4f,0.4f,0,0.6f,0.6f,1.0f));
	addSubData("vel_y", vel_y);


	// TEMP --------
	// velocity test
	VectorField::Ptr velocity = std::make_shared<VectorField>(  );
	velocity->resize( density->getResolution().x, density->getResolution().y, density->getResolution().z );
	velocity->setBound( math::Box3f( math::V3f(-7.5), math::V3f(7.5) ) );
	velocity->fill( math::V3f(1.0f, 1.0f, 1.0f) );
	addSubData("velocity", velocity);
}

void CloudData::reset()
{
    qDebug() << "reset Cloud Data";
}

float CloudData::getTimestep()
{
	return dt;
}

math::V3i CloudData::getResolution()
{
	return resolution;
}

void CloudData::resize(math::V3i size)
{
	resolution = size;
}
