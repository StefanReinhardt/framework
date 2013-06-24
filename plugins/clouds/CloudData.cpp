#include "CloudData.h"


CloudData::CloudData() : SimObject()
{
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




	density = std::make_shared<ScalarField>();
	density->resize(20,20,5);
	density->localToWorld(math::V3f(2,2,1));
	density->fill(33.3f);
	addSubData("density", density);

	vel_x = std::make_shared<ScalarField>();
	vel_x->resize(20,20,5);
	vel_x->localToWorld(math::V3f(2,2,1));
	vel_x->fill(0.0f);
	vel_x->fill(1,math::Box3f(0.3f,0.3f,0,0.7f,0.7f,1.0f));
	addSubData("vel_x", vel_x );

	vel_y = std::make_shared<ScalarField>();
	vel_y->resize(20,20,5);
	vel_y->localToWorld(math::V3f(2,2,1));
	vel_y->fill(0.0f);
	vel_y->fill(1,math::Box3f(0.0f,0.3f,0,1.0f,0.7f,1.0f));
	addSubData("vel_y", vel_y);


	// TEMP --------
	// staggered grid for velocity test
	VectorField::Ptr velocity = std::make_shared<VectorField>( VectorField::FACE );
	velocity->resize( 50, 50, 50 );
	velocity->setBound( math::Box3f( math::V3f(-0.5), math::V3f(0.5) ) );
	velocity->fill( math::V3f(1.0f, 1.0f, 1.0f) );
	addSubData("velocity", velocity);
}

void CloudData::reset()
{
    qDebug() << "reset Cloud Data";
}
