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
}

void CloudData::reset()
{
    qDebug() << "reset Cloud Data";
}
