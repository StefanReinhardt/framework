#include "WaterContinuity2D.h"
#include <plugins/clouds/CloudData.h>
#include <math.h>


WaterContinuity2D::WaterContinuity2D()
{
	m_qsCorrection = true;
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

	// old Harris Method
	//===============================================================
	if (!m_qsCorrection)
	{
		for( int i=1;i<res.x-1;++i )
			for( int j=1;j<res.y-1;++j )
			{
				// unit independent temperature T = pt*exner
				float T = cd->potTempToTemp(pt->lvalue(i,j,k) , cd->m_pLut[j]);

				float qs = cd->qvs(T, cd->m_pLut[j]);
				float d_qv  = math::min(qs - qv->lvalue(i,j,k),qc->lvalue(i,j,k));

				qv->lvalue(i,j,k) = qv->lvalue(i,j,k) + d_qv;		// evaporisation if d_qv positiv and
				qc->lvalue(i,j,k) = qc->lvalue(i,j,k) - d_qv;		// condensation if d_qv negative

				// ToDo: Flickering when stronger LH
				//       remove 0.5!?

				// Update the potential temperature according to condesation
				pt->lvalue(i,j,k) += cd->potTemp( (cd->m_lh / cd->m_cp) * (-d_qv) , cd->m_pLut[j] );

				test->lvalue(i,j,k) =  cd->potTemp( (cd->m_lh / cd->m_cp) * (-d_qv) , cd->m_pLut[j] );
			}
	}

	// new correction Method
	//===============================================================
	else
	{
		for( int i=1;i<res.x-1;++i )
		{
			for( int j=1;j<res.y-1;++j )
			{
				float T = cd->potTempToTemp(pt->lvalue(i,j,k) , cd->m_pLut[j]);

				//---------------------------------------- Budget Equations
				float qs = cd->qvs( T , cd->m_pLut[j] );
				float d_qv  = math::min( qs - qv->lvalue(i,j,k) , qc->lvalue(i,j,k) );

				float qv_p = qv->lvalue(i,j,k) + d_qv;								// provisional vapor update			= q~v
				float qc_p = qc->lvalue(i,j,k) - d_qv;								// provisional cloudwater update	= q~c
				float T_p = T + (cd->m_lh / cd->m_cp) * (-d_qv);					// provisional Temperature update	= T~

				//---------------------------------------- Cloud Conservative values (total amount of water and energy)
				float qT = qv_p + qc_p;												// total amount of water			= q~T
				float Th = T_p + (cd->m_lh / cd->m_cp) * qv_p;					// enthalpy relatet Temperature		= T~h

				//---------------------------------------- Adjust provisional values
				float T_pl = T_p - (cd->m_lh / cd->m_cp) * qc_p;					// preliminary temperature			= T*
				float qs_pl = cd->qvs( T_pl , cd->m_pLut[j] );						// preliminary saturation			= qs*

				// if subsaturated or just saturated (no cloud)
				if(qT <= qs_pl)
				{
					pt->lvalue(i,j,k) = cd->potTemp(T_pl, cd->m_pLut[j]);			// convert from Temp to potTemp
					qv->lvalue(i,j,k) = qT;
					qc->lvalue(i,j,k) = 0.0f;
				}

				// if saturated (clouds visible)
				else
				{
					// TODO: NEWTON ITERATION
					// solve for T_v+1
					//NewtonSolve(T_v) = Th - LHV/CP * ( QVS(T_v,cd->m_pLut[j]) - T_v *()   )


					float aw = 17.27f;
					float bw = 35.86f;
					float Tr = 273.16f;		// K
					float p0 = 610.78f;		// Pa

					float d_qvs = - (aw * exp( aw*(T-Tr)/(-bw+T) ) * cd->m_pLut[j] * p0 * RD * RV * (bw-Tr) ) /
									(pow(( exp( aw*(T-Tr)/(-bw+T) ) * p0 * (RD - RV) + cd->m_pLut[j] * RV), 2) * pow(bw-T,2) );

//					qCritical() << "===============================================d qvs=" << d_qvs;
//					qCritical() << "===============================================Th=" << Th;
//					qCritical() << "===============================================T=" << T;
//					qCritical() << "===============================================qvs=" << cd->qvs(T, cd->m_pLut[j]);

					float T_n =	(Th - (cd->m_lh/cd->m_cp) * ( cd->qvs(T, cd->m_pLut[j]) - T*d_qvs) ) /
								( 1 + (cd->m_lh/cd->m_cp) * d_qvs) ;

//					qCritical() << "===============================================T_neu=" << T_n << "alt:" << T;

					pt->lvalue(i,j,k) = cd->potTemp(T, cd->m_pLut[j]);			// convert from Temp to potTemp
					qv->lvalue(i,j,k) = cd->qvs(T,cd->m_pLut[j])+(T_n-T)*d_qvs;
					qc->lvalue(i,j,k) = qT - qv->lvalue(i,j,k);
				}

			}
		}
	}
	//set Boundary values
	cd->setBounds2D(4,so->getSubData<ScalarField>("pt"));
	cd->setBounds2D(5,so->getSubData<ScalarField>("qv"));
	cd->setBounds2D(6,so->getSubData<ScalarField>("qc"));




	timer.stop();
	qCritical() << "WaterCont:" << core::getVariable("$F").toString() << ":" << timer.elapsedSeconds();


}
