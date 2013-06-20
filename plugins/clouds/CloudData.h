#ifndef CLOUDDATA_H
#define CLOUDDATA_H

#include <plugins/sim/SimObject.h>
#include <vector>


class CloudData  : public SimObject
{
public:
//typedef std::shared_ptr<Operator> Ptr;

    CloudData();

    void reset();

private:

    float tlr;					// Temperature lapse rate in °C per 100meter
    float hum;					// humidity in percent 0-1
    float maxAlt;				// upper boarder of altitude in simulation in meter
    float t0;                   // ground temp
    float p0;                   // ground pressure
    float vorticity;
    float buoyancy;
    float gravity;

    float diff; 				// Viscosity
    float visc; 				// Diffusionrate
    float wind;					// horizontal wind
    float heatSrc;				// input temperature

    float rd;
    float lh;
    float cp;
    float exner;
    float qs;

    std::vector<float> tLut;	// absolute Temperature at altitude in K
    std::vector<float> pLut;	// absolute Pressure at altitude in kPa

};

#endif // CLOUDDATA_H
