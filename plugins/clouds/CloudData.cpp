#include "CloudData.h"


CloudData::CloudData( Parameters parms ) : SimObject()
{
	m_parms = parms;
	initialize();
	reset();
}

CloudData::CloudData() : SimObject()
{
	initialize();
	reset();

}



void CloudData::initialize()
{
	m_diff = 0.0000f;         // diffusion
	m_visc = 0.000000000f;    // viscosity

	m_resolution = m_parms.m_resolution;
	//Constants
	m_rd = 		287; 		// specific gas constant for dry air
	m_p0 = 		100;		// pressure at sea level (kPa)
	m_gravity = 	9.81f;		// gravitational acceleration (m/s²)
	m_lh = 		2501;		// Latent heat of vaporization of water (J/kg)
	m_cp = 		1005;		// specific heat capacity J/(kg K)

	ScalarField::Ptr density = std::make_shared<ScalarField>();
	density->resize(m_resolution);
	density->localToWorld(math::V3f(2,2,1));
	density->fill(0.0f);
	density->fill(33.3f,math::Box3f(0.4f,0.1f,0.4f,0.6f,0.4f,0.6f));
	addSubData("density", density);

	VectorField::Ptr velocity = std::make_shared<VectorField>( VectorField::FACE );
	velocity->resize(m_resolution);
	//velocity->setBound( math::Box3f( math::V3f(-0.51f), math::V3f(0.51f) ) );
	//velocity->fill( math::V3f(1.0f, 1.0f, 1.0f) );
	addSubData("velocity", velocity);

	velocity->getScalarField(0)->fill(0.0f);
	velocity->getScalarField(0)->fill(0.0f,math::Box3f(0.4f,0.1f,0.4f,0.6f,0.4f,0.6f));

	velocity->getScalarField(1)->fill(0.0f);
	velocity->getScalarField(1)->fill(1.0f,math::Box3f(0.4f,0.1f,0.4f,0.6f,0.4f,0.6f));

	velocity->getScalarField(2)->fill(0.0f);
	velocity->getScalarField(2)->fill(0.0f,math::Box3f(0.4f,0.1f,0.4f,0.6f,0.4f,0.6f));
}



void CloudData::reset()
{
	ScalarField::Ptr pt = std::make_shared<ScalarField>();
	pt->resize(m_resolution);
	addSubData("pt", pt);

	ScalarField::Ptr qv = std::make_shared<ScalarField>();
	qv->resize(m_resolution);
	addSubData("qv", qv);

	ScalarField::Ptr qc = std::make_shared<ScalarField>();
	qc->resize(m_resolution);
	addSubData("qc", qc);




    qDebug() << "reset Cloud Data";

	// Initialize absolute Temp lookup
	//************************************************************
	for(int y= 0; y<m_resolution.y; y++){
		// Ground Temp - (altitude / 100)* tempLapseRate
		m_tLut.push_back(m_parms.m_t0 - (( (float)y/(float)m_resolution.y ) * m_parms.m_maxAlt) * m_parms.m_tlr);
	}

	// Initialize absolute Pressure lookup in kPa
	//************************************************************
	for(int y= 0; y<m_resolution.y; y++){
		float alt = ( (float)y / (float)m_resolution.y ) * m_parms.m_maxAlt;
		m_pLut.push_back((float) (m_p0* pow(( 1- ( (alt*m_parms.m_tlr)/m_parms.m_t0 ) ),(m_gravity/(m_parms.m_tlr*m_rd)) )) );
	}

	// Initialize pot temp
	//************************************************************
	for(int k= 0; k<m_resolution.z; k++)
		for(int i= 0; i<m_resolution.x; i++)
			for(int j= 0; j<m_resolution.y; j++)
			{
				//					K                   kPa/kPa
				pt->lvalue(i,j,k)= (float) (m_tLut.at(j) * ( pow( (m_p0/m_pLut.at(j)) , 0.286)));
				//pt_old->lvalue(i,j,k) = pt->lvalue(i,j,k);
			}


	// Init ground pot temp
	//************************************************************
	m_pt0 = m_tLut[0] * pow( m_p0/m_pLut[0] , 0.286);

	// Initialize Saturation vapor mixing ratio and water vapor mixing ratio
	//************************************************************
	//T in celsius
	for(int k= 0; k<m_resolution.z; k++)
		for(int i= 0; i<m_resolution.x; i++)
			for(int j= 0; j<m_resolution.y; j++)
			{
				// temp in °C and p in Pa
				m_qs		=		(float) (  (380/(m_pLut[j]*1000)  ) * exp( (17.67*(m_tLut[j]-273.15)) / (m_tLut[j]-273.15+243.5))) ;
				qv->lvalue(i,j,k) = 		m_qs * m_parms.m_hum;
			}

	//pt->fill(305.0,math::Box3f(0.4f,0.05f,0.4f,0.60f,0.91f,0.6f));

}

float CloudData::getTimestep()
{
	return m_parms.m_dt;
}

math::V3i CloudData::getResolution()
{
	return m_parms.m_resolution;
}

void CloudData::resize(math::V3i size)
{
	m_resolution = size;
}

void CloudData::setTimestep(float timestep)
{
	m_parms.m_dt=timestep;
}


// ========Settings for b
// 0=NeumannBounds || 1=vel_x || 2=vel_y || 3=vel_z || 4=potentialTemperature || 5=waterVapor || 6=cloudWater

void CloudData::setBounds(int b, ScalarField::Ptr f)
{
	math::V3i res = f->getResolution();



	switch ( b )
	{

		// ___________________________________
		// Neumann Boundaries
		case 0:
			qDebug()  << "setBounds with b=0 (Neumann) res = "<< res.x;

			for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k)= f->lvalue(1,j,k);
					f->lvalue(res.x-1,j,k)= f->lvalue(res.x-2,j,k);
				}

			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k)= f->lvalue(i,1,k);
					f->lvalue(i,res.y-1,k) = f->lvalue(i,res.y-2,k);
				}

			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,j,0) = f->lvalue(i,j,1);
					f->lvalue(i,j,res.z-1) = f->lvalue(i,j,res.z-2);
				}

			break;

		// ___________________________________
		// x Velocity
		case 1:
			qDebug()  << "setBounds with b=1 (vel_x)";

			// sides 	= user defined wind
			for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k) = m_parms.m_wind;
					f->lvalue(1,j,k) = m_parms.m_wind;
					f->lvalue(res.x-1,j,k) =  m_parms.m_wind;
					f->lvalue(res.x-2,j,k) =  m_parms.m_wind;
				}

			// bottom 	= noslip
			// top 		= free slip
			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					//f->lvalue(i,0,k)=0;
					f->lvalue(i,0,k)=-f->lvalue(i,1,k);
					f->lvalue(i,res.y-1,k)=f->lvalue(i,res.y-2,k);
				}

			// backFront = free slip
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,j,0) = f->lvalue(i,j,1);
					f->lvalue(i,j,res.z-1) = f->lvalue(i,j,res.z-2);
				}
			break;

		// ___________________________________
		// y Velocity
		case 2:
			qDebug()  << "setBounds with b=2 (vel_y)";

			// sides = zero
			for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k)= 0;
					f->lvalue(res.x-1,j,k)= 0;
				}

			// bottom 	= noslip
			// top 		= free slip
			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k) = 0;
					f->lvalue(i,1,k) = 0;
					f->lvalue(i,res.y-1,k) = 0;
					f->lvalue(i,res.y-2,k) = 0;
				}

			// frontBack = zero
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,j,0)= 0;
					f->lvalue(i,j,res.z-1)= 0;
				}
			break;

		// ___________________________________
		// z Velocity
		case 3:
			qDebug()  << "setBounds with b=3 (vel_z)";

			// sides = wind
			for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k) = 0;
					f->lvalue(res.x-1,j,k) = 0;
				}

			// bottom 	= noslip
			// top 		= free slip
			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k)=-f->lvalue(i,1,k);
					f->lvalue(i,res.y-1,k)=f->lvalue(i,res.y-2,k);
				}

			// frontBack = free slip
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,j,0)=0;
					f->lvalue(i,j,1)=0;
					f->lvalue(i,j,res.z-1)=0;
					f->lvalue(i,j,res.z-2)=0;

				}


			break;

		case 4:
			{
				float pt_trav=0;
				float pt0 = m_tLut[0]*pow(100/m_pLut[0], 0.286);
				float pty = m_tLut[res.y-1]*pow(100/m_pLut[res.y-1], 0.286);

				qDebug()  << "setBounds with b=4 (pt)";

				// Sides
				for( int j=0;j<res.y;++j )
				{
					pt_trav=m_tLut[j]*pow(100/m_pLut[j], 0.286);
					for( int k=0;k<res.z;++k )
					{
						f->lvalue(0,j,k) =			pt_trav;
						f->lvalue(res.x-1,j,k) =	f->lvalue(0,j,k);
					}
				}
				// top bottom

				for( int k=0;k<res.z;++k )
					for( int i=0;i<res.x;++i )
					{
						f->lvalue(i,0,k) =			pt0;
						f->lvalue(i,res.y-1,k) =	pty;
					}

				// frontBack
				for( int j=0;j<res.y;++j )
					for( int i=0;i<res.x;++i )
					{
						f->lvalue(i,j,0) =			f->lvalue(0,j,0) ;
						f->lvalue(i,j,res.z-1) =	f->lvalue(i,j,0);
					}
			}
			break;


		case 5:
			qDebug()  << "setBounds with b=5 (qv)";


			// sides = periodic
			for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k) = 13370.1337f;
					f->lvalue(res.x-1,j,k) = 13370.1337f;
				}

			// bottom 	= noise
			// top 		= 0
			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k) = 0;
					f->lvalue(i,res.y-1,k) = 0;
				}

			// frontBack = periodic?
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					//f->lvalue(i,j,0) = tLut.at(j)*pow(100/pLut.at(j), 0.286);
					//f->lvalue(i,j,res.z-1) = tLut.at(j)*pow(100/pLut.at(j), 0.286);
				}

			break;


		case 6:
			qDebug()  << "setBounds with b=6 (qc)";


			// sides = 0
			for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k) =			0;
					f->lvalue(res.x-1,j,k) =	0;
				}

			// bottom 	= 0
			// top 		= 0
			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k) =			0;
					f->lvalue(i,res.y-1,k) =	0;
				}

			// frontBack = periodic?
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,j,0) =		0;
					f->lvalue(i,j,res.z-1) =	0;
				}

			break;

		default:
			qCritical() << "wrong Boundary settings. b="<<b;
			break;
	}
		// set corner values
		f->lvalue(0,0,0)=					( f->lvalue(1,0,0) + f->lvalue(0,1,0) + f->lvalue(0,0,1))/3;
		f->lvalue(res.x-1,0,0)=				( f->lvalue(res.x-2,0,0) + f->lvalue(res.x-1,1,0) + f->lvalue(res.x-1,0,1))/3;
		f->lvalue(0,res.y-1,0)=				( f->lvalue(1,res.y-1,0) + f->lvalue(0,res.y-2,0) +  f->lvalue(0,res.y-1,1) )/3;
		f->lvalue(res.x-1,res.y-1,0)=		( f->lvalue(res.x-2,res.y-1,0) + f->lvalue(res.x-1,res.y-2,0) +  f->lvalue(res.x-1,res.y-1,1) )/3;

		f->lvalue(0,0,res.z-1)=				( f->lvalue(1,0,res.z-1) + f->lvalue(0,1,res.z-1) + f->lvalue(0,0,res.z-2))/3;
		f->lvalue(res.x-1,0,res.z-1)=		( f->lvalue(res.x-2,0,res.z-1) + f->lvalue(res.x-1,1,res.z-1) + f->lvalue(res.x-1,0,res.z-2))/3;
		f->lvalue(0,res.y-1,res.z-1)=		( f->lvalue(1,res.y-1,res.z-1) + f->lvalue(0,res.y-2,res.z-1) +  f->lvalue(0,res.y-1,res.z-2) )/3;
		f->lvalue(res.x-1,res.y-1,res.z-1)=	( f->lvalue(res.x-2,res.y-1,res.z-1) + f->lvalue(res.x-1,res.y-2,res.z-1) +  f->lvalue(res.x-1,res.y-1,res.z-2) )/3;




}

void CloudData::setBounds2D(int b, ScalarField::Ptr f)
{
	math::V3i res = f->getResolution();
	int k=0;
	switch ( b )
	{

		// ___________________________________
		// Neumann Boundaries
		case 0:
			qDebug()  << "setBounds with b=0 (Neumann) res = "<< res.x;
			for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k)= f->lvalue(1,j,k);
					f->lvalue(res.x-1,j,k)= f->lvalue(res.x-2,j,k);
				}

			for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k)= f->lvalue(i,1,k);
					f->lvalue(i,res.y-1,k) = f->lvalue(i,res.y-2,k);
				}
			break;

		// ___________________________________
		// x Velocity
		case 1:
			qDebug()  << "setBounds with b=1 (vel_x)";
			// sides 	= user defined wind
			for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k) =  m_parms.m_wind;
					f->lvalue(1,j,k) =  m_parms.m_wind;
					f->lvalue(res.x-1,j,k) =  m_parms.m_wind;
					f->lvalue(res.x-2,j,k) =  m_parms.m_wind;
				}

			// bottom 	= noslip
			// top 		= free slip
			//for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k)=-f->lvalue(i,1,k);
					f->lvalue(i,res.y-1,k)=f->lvalue(i,res.y-2,k);
				}
			break;

		// ___________________________________
		// y Velocity
		case 2:
			qDebug()  << "setBounds with b=2 (vel_y)";
			// sides = zero
			//for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k)= -f->lvalue(1,j,k);
					f->lvalue(res.x-1,j,k)= -f->lvalue(res.x-2,j,k);
				}

			// bottom 	= noslip
			// top 		= free slip
			//for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k) = 0;
					f->lvalue(i,1,k) = 0;
					//if(i>30 && i<70){
					//	f->lvalue(i,0,k) = 0.1;
					//	f->lvalue(i,1,k) = 0.1;
					//}
					f->lvalue(i,res.y-1,k) = 0;
					f->lvalue(i,res.y-2,k) = 0;
				}
			break;

		// ___________________________________
		// z Velocity
		case 3:
			qDebug()  << "setBounds with b=3 (vel_z)";
			break;

		case 4:
			qDebug()  << "setBounds with b=4 (pt)";
			// sides = wind
			for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k) = m_tLut.at(j)*pow(100/m_pLut.at(j), 0.286);
					f->lvalue(res.x-1,j,k) = m_tLut.at(j)*pow(100/m_pLut.at(j), 0.286);
				}

			// bottom 	= noslip
			// top 		= free slip
			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k) = m_tLut.at(0)*pow(100/m_pLut.at(0), 0.286);
					f->lvalue(i,res.y-1,k) = m_tLut.at(res.y-1)*pow(100/m_pLut.at(res.y-1), 0.286);
				}
			break;


		case 5:
		{
			qDebug()  << "setBounds with b=5 (qv)";
			// sides = periodic
			float qv=0;
			for( int j=0;j<res.y;++j )
			{
				qv = m_parms.m_hum * (float) (  (380/(m_pLut[j]*1000)  ) * exp( (17.67*(m_tLut[j]-273.15)) / (m_tLut[j]-273.15+243.5))) ;
				for( int k=0;k<res.z;++k )
				{
					f->lvalue(0,j,k) = qv;
					f->lvalue(res.x-1,j,k) = qv;
				}
			}
			// bottom 	= noise
			// top 		= 0
			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					//f->lvalue(i,0,k) = 33.3f;
					f->lvalue(i,res.y-1,k) = 0;
				}
			// frontBack = periodic?
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					//f->lvalue(i,j,0) = tLut.at(j)*pow(100/pLut.at(j), 0.286);
					//f->lvalue(i,j,res.z-1) = tLut.at(j)*pow(100/pLut.at(j), 0.286);
				}
		}
			break;


		case 6:
			qDebug()  << "setBounds with b=5 (qc)";
			// sides = 0
			for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k) =			0;
					f->lvalue(res.x-1,j,k) =	0;
				}
			// bottom 	= 0
			// top 		= 0
			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k) =			0;
					f->lvalue(i,res.y-1,k) =	0;
				}
			break;

		default:
			qCritical() << "wrong Boundary settings. b="<<b;
			break;
	}
		// set corner values
		f->lvalue(0,0,0)=( f->lvalue(1,0,0) + f->lvalue(0,1,0) )/2;
		f->lvalue(res.x-1,0,0)=( f->lvalue(res.x-2,0,0) + f->lvalue(res.x-1,1,0) )/2;
		f->lvalue(0,res.y-1,0)=( f->lvalue(1,res.y-1,0) + f->lvalue(0,res.y-2,0) )/2;
		f->lvalue(res.x-1,res.y-1,0)=( f->lvalue(res.x-2,res.y-1,0) + f->lvalue(res.x-1,res.y-2,0) )/2;


}
