#include "WaterContinuity2D.h"
#include <plugins/clouds/CloudData.h>
#include <math.h>

WaterContinuity2D::WaterContinuity2D()
{
}

void WaterContinuity2D::apply(SimObject::Ptr so, float dt)
{

	timer.start();

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);



	cd->getResolution();

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr qc = cd->getSubData<ScalarField>("qc");
	ScalarField::Ptr vel_y = cd->getSubData<VectorField>("velocity")->getScalarField(1);
	math::V3i res =cd->getResolution();




	std::vector<float>              r, Kp, Ki;
	std::vector<int>                htar, hc;
	for (int i=0; i<res.x; i++)
	{
		htar.push_back(sin(i*0.1+30)*24+55);
		Kp.push_back(0.004f * htar[i]/res.y );
		Ki.push_back(0.004f * htar[i]/res.y );
	}

	int k =               0;

	float ravg =        0;
	float intSum =        0;
	float dh =            0;
	float Qc =            0;
	float Svc =           0;
	float cv =            0.004f;


	for (int i=0; i<res.x; i++)
	{
		// height of clouds
		int j=res.y-1;
		while(qc->lvalue(i,j,k)<0.000000001 && j>0)
			--j;
		hc.push_back(j);

		// ratio
		r.push_back(hc[i]/htar[i]);
		ravg += r[i];
	}

	// average of the ratios
	ravg /= res.x;


	float d_qv, T, qs, exner;

	for( int i=1;i<res.x-1;++i )
	{
		if(r[i]<1)
		{
			// add vapor if (ratio < avg_ratio)
			if(r[i] < ravg)
				Svc =  cv * cd->m_qv0Lut[hc[i]] ;

			intSum += dt * dh;
			dh= 1-r[i];
			Qc = Kp[i] * dh + Ki[i] * intSum;

			vel_y->lvalue(i,0,0) += Qc;
			vel_y->lvalue(i,1,0) += Qc;
			qv->lvalue(i,0,0) += Svc;
			qv->lvalue(i,1,0) += Svc;
			qv->lvalue(i,hc[i],0) += Svc;

		}


		for( int j=1;j<res.y-1;++j )
		{
			//alt=((float)j/(float)ssy)*maxAlt;

			//compute 	p(alt) in kPa
			// 			p(alt) = p0*(1- (zL/T0) )^(g/L Rd)
			//			with alt= altitude in m, g = 9.81(m/s^2)
			//			p0 and T0 = p and T at the base altitude
			//			typically: p0 = 10kP and T0=280-310K
			// 			L = Lapse rate in °K or °C per meter
			//			Rd = ideal gas constant ~ 287 J/(kg K)

			exner = pow(cd->m_pLut[j]/cd->m_p0,0.286f);

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
			qs = (float) ( (380.16f / (cd->m_pLut[j]*1000) ) * exp( (17.67f * T) / (T + 243.5f) ) );

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

			pt->lvalue(i,j,k) += (cd->m_lh / ( cd->m_cp * exner )) * (-d_qv);
		}
	}

	//set Boundary values
	cd->setBounds2D(4,so->getSubData<ScalarField>("pt"));
	cd->setBounds2D(5,so->getSubData<ScalarField>("qv"));
	cd->setBounds2D(6,so->getSubData<ScalarField>("qc"));




	timer.stop();
	qCritical() << "WaterCont:" << core::getVariable("$F").toString() << ":" << timer.elapsedSeconds();


}
