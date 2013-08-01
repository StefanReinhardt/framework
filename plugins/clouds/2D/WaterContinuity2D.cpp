#include "WaterContinuity2D.h"
#include <plugins/clouds/CloudData.h>
#include <math.h>


WaterContinuity2D::WaterContinuity2D()
{
}

void WaterContinuity2D::applyImpl(SimObject::Ptr so, float dt)
{

	timer.start();

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);



	cd->getResolution();

	ScalarField::Ptr pt = cd->getSubData<ScalarField>("pt");
	ScalarField::Ptr qv = cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr qc = cd->getSubData<ScalarField>("qc");
	ScalarField::Ptr vel_y = cd->getSubData<VectorField>("velocity")->getScalarField(1);
	math::V3i res =cd->getResolution();


	ScalarField::Ptr test;

	if(so->hasSubData("test") )
	{
		test = so->getSubData<ScalarField>( "test" );
	}
	else
	{
		test = std::make_shared<ScalarField>();
		test->resize(res);
		so->addSubData( "test", test );
	}



	int k =               0;


	float d_qv, T, qs, exner;

	for( int i=1;i<res.x-1;++i )
	{
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
			//compute 	qs= (380.16/p)exp((17.67*T)/(T+243.5)) in kg/kg
			// with T in °C and P in Pa

			qs = (float) ( (380.16f / (cd->m_pLut[j]*1000) ) * exp( (17.67f * T) / (T + 243.5f) ) );


			d_qv  = math::min(qs - qv->lvalue(i,j,k),qc->lvalue(i,j,k));

			qv->lvalue(i,j,k) = qv->lvalue(i,j,k) + d_qv;		// evaporisation if d_qv positiv and
			qc->lvalue(i,j,k) = qc->lvalue(i,j,k) - d_qv;		// condensation if d_qv negative

			// Update the potential temperature according to condesation
			// Due to condensation latent energy is released in form of heat. -> change in pot temp
			// update potential Temperature ( Thermodynamics Equation )
			// delta p = L/(cp*PI)*(delta C)
			// L  = constant for latent heat released    	2501 kJ/kg  = 2501000 J/kg
			// cp = specific heat capacity of dry air 		1005 J/(kg K)
			// C  = condensation rate = condensation per evaporation
			//	  = - min(qvs-qv, qc)  siehe waterCont = d_qv
			// PI = Exner Function = T/pt =
			//_______________L_____/___cp___*________PI___________*____________C_________________________________
			T += 273.15f;

			// ToDo: Flickering when stronger LH
			//       remove 0.5!?
			test->lvalue(i,j,k) =  (cd->m_lh / ( cd->m_cp * exner )) * (-d_qv);

			//vel_y->lvalue(i,j,k)+= 10.f* math::max(0.0f,-d_qv);
			pt->lvalue(i,j,k) += (cd->m_lh / ( cd->m_cp * exner )) * (-d_qv);

		}
	}


/*
	for( int i=1;i<res.x-1;++i )
	{
		for( int j=1;j<res.y-1;++j )
		{
			exner = pow(cd->m_pLut[j]/cd->m_p0,0.286f);
			T = pt->lvalue(i,j,k)*exner -  273.15f; // in Celsius

			//---------------------------------------- Budget Equations
			qs = QVS(T,cd->m_pLut[j]);
			d_qv  = math::min(qs - qv->lvalue(i,j,k),qc->lvalue(i,j,k));

			float qv_p = qv->lvalue(i,j,k) + d_qv;								// provisional vapor update
			float qc_p = qc->lvalue(i,j,k) - d_qv;								// provisional cloudwater update
			float T_p = pt->lvalue(i,j,k) + (cd->m_lh / cd->m_cp) * (-d_qv);	// provisional Temperature update

			//---------------------------------------- Cloud Conservative values (total amount of water and energy)
			float qT = qv_p + qc_p;												// total amount of water
			float Th = T_p + (cd->m_lh / cd->m_cp) * (-d_qv);					// enthalpy relatet Temperature

			//---------------------------------------- Adjust provisional values
			float T_pl = T_p - (LHV / CP) * qc_p;								// preliminary temperature T*
			float qs_pl = QVS(T_pl,cd->m_pLut[j]);								// preliminary saturation qs*

			// if subsaturated or just saturated (no cloud)
			if(qT <= qs_pl)
			{
				pt->lvalue(i,j,k) = (T_pl+273.15)/exner;						// by exner to convert from Temp to potTemp
				qv->lvalue(i,j,k) = qT;
				qc->lvalue(i,j,k) = 0.0f;
			}

			// if saturated (clouds visible)
			else
			{
				// TODO: NEWTON ITERATION
				// solve for T_v+1
				//NewtonSolve(T_v) = Th - LHV/CP * ( QVS(T_v,cd->m_pLut[j]) - T_v *()   )

				//pt->lvalue(i,j,k) = T_p/exner;									// by exner to convert from Temp to potTemp
				//qv->lvalue(i,j,k) = qv_p;
				//qc->lvalue(i,j,k) = qc_p;
			}

		}
	}
*/
	//set Boundary values
	cd->setBounds2D(4,so->getSubData<ScalarField>("pt"));
	cd->setBounds2D(5,so->getSubData<ScalarField>("qv"));
	cd->setBounds2D(6,so->getSubData<ScalarField>("qc"));




	timer.stop();
	qCritical() << "WaterCont:" << core::getVariable("$F").toString() << ":" << timer.elapsedSeconds();


}
