#include "WaterContinuity2D.h"
#include <plugins/clouds/CloudData.h>
#include <math.h>

WaterContinuity2D::WaterContinuity2D()
{
}

void WaterContinuity2D::apply(SimObject::Ptr so)
{


	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);



	cd->getResolution();

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr qc = cd->getSubData<ScalarField>("qc");
	math::V3i res =cd->getResolution();

	float d_qv, T, qs, exner;

	int k = 0;
	for( int j=1;j<res.y-1;++j )
		for( int i=1;i<res.x-1;++i )
		{
			//alt=((float)j/(float)ssy)*maxAlt;

			//compute 	p(alt) in kPa
			// 			p(alt) = p0*(1- (zL/T0) )^(g/L Rd)
			//			with alt= altitude in m, g = 9.81(m/s^2)
			//			p0 and T0 = p and T at the base altitude
			//			typically: p0 = 10kP and T0=280-310K
			// 			L = Lapse rate in °K or °C per meter
			//			Rd = ideal gas constant ~ 287 J/(kg K)

			exner = pow(cd->pLut.at(j)/cd->p0,0.286f);

			//p = (float) Math.pow(10*(1-(alt*tlr/t0)),(9.81/(tlr*rd)));
			//
			//compute 	T = pt[i,j]/( (^p/p)^k  )
			// 			with  ^p = 100kPa   k = ~0.286
			//			T = pt[i,j]/( (100/p)^0.286)
			T = pt->lvalue(i,j,k)*exner;

			// conversion from Kelvin to Celsius
			T -= 273.15f;
			//
			//compute 	qs= (380.16/p)exp((17.67*T)/(T+243.5))
			// with T in °C and P in Pa
			qs = (float) ( (380.16f / (cd->pLut.at(j)*1000) ) * exp( (17.67f * T) / (T + 243.5f) ) );

			d_qv  = math::min(qs - qv->lvalue(i,j,k),qc->lvalue(i,j,k));

			qv->lvalue(i,j,k) = qv->lvalue(i,j,k) + d_qv;
			qc->lvalue(i,j,k) = qc->lvalue(i,j,k) - d_qv;

			// Update the potential temperature according to condesation
			// Due to condensation latent energy is released in form of heat. -> change in pot temp
			// update potential Temperature ( Thermodynamics Equation )
			// delta p = L/(cp*PI)*(delta C)
			// L  = constant for latent heat released    	2501 J/kg
			// cp = specific heat capacity of dry air 		1005 J/(kg K)
			// C  = condensation rate = condensation per evaporation
			//	  = - min(qvs-qv, qc)  siehe waterCont = d_qv
			// PI = Exner Function = T/pt =
			//_______________L_____/___cp___*________PI___________*____________C_________________________________
			T += 273.15f;

			pt->lvalue(i,j,k) += (cd->lh / ( cd->cp * exner )) * (-d_qv);
		}

	//set Boundary values
	//cd->setBounds(4,cd->pt);
	//cd->setBounds(5,cd->qv);
	//cd->setBounds(6,cd->qc);







}
