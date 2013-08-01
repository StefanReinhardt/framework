#include "CloudControl2D.h"
#include <plugins/clouds/CloudData.h>
#include <math.h>

CloudControl2D::CloudControl2D()
{
}

void CloudControl2D::applyImpl(SimObject::Ptr so, float dt)
{

	timer.start();

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);

	ScalarField::Ptr geoPotForce =       cd->getSubData<ScalarField>("geoPotForce");
	ScalarField::Ptr pt =       cd->getSubData<ScalarField>("pt");
	ScalarField::Ptr geoPot =   cd->getSubData<ScalarField>("geoPot");
	ScalarField::Ptr qv =       cd->getSubData<ScalarField>("qv");
	ScalarField::Ptr qc =       cd->getSubData<ScalarField>("qc");
	ScalarField::Ptr vel_y=     cd->getSubData<VectorField>("velocity")->getScalarField(1);
	ScalarField::Ptr vel_x=     cd->getSubData<VectorField>("velocity")->getScalarField(0);
	math::V3i res =             cd->getResolution();





	int k =                              0;

	float ravg =                         0;
	float intSum =                       0;
	float dh =                           0;
	float Qc =                           0;
	float Svc =                          0;
	std::vector<float>                   r;

	// calculate height of clouds and height ratio
	for (int i=0; i<res.x; i++)
	{
		int j=res.y-1;
		while(qc->lvalue(i,j,k)<0.000000001 && j>0)
			--j;
		cd->m_hc[i] = j;
		r.push_back(cd->m_hc[i]/cd->m_htar[i]);
		ravg += r[i];
	}
	ravg /= res.x;


	// add vapor and latent Heat
	for( int i=2;i<res.x-2;++i )
	{
		if(r[i]<1)
		{
			Svc=0;
			if(r[i] < ravg)
				Svc =  cd->m_cv * cd->m_qv0Lut[cd->m_hc[i]] ;

			intSum += dt * dh;
			dh= 1.0f - r[i];

			Qc = cd->m_Kp[i] * dh;  // +cd->m_Ki[i] * intSum;

			// ToDo:  add Qc to pt instead vel
			pt->lvalue(i,1,0) += Qc;
			pt->lvalue(i,2,0) += Qc;
			vel_y->lvalue(i,2,0) += Qc;
			qv->lvalue(i,0,0) += Svc;
			qv->lvalue(i,1,0) += Svc;
			qv->lvalue(i,cd->m_hc[i],0) += Svc;

		}
	}

	// Apply Force of geometric Potential Field
	for( int j=1;j<res.y-1;++j )
		for( int i=1;i<res.x-1;++i )
		{
			vel_x->lvalue(i,j,k) = qc->lvalue(i,j,k) * geoPotForce->lvalue(i,j,k);
		}

	timer.stop();
	qCritical() << "WaterCont:" << core::getVariable("$F").toString() << ":" << timer.elapsedSeconds();


}
