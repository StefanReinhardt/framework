#include "CloudData.h"
#include <QFile>
#include <qstringlist.h>
#include <core/Core.h>


CloudData::CloudData( Parameters parms ) : SimObject()
{
	m_useSoudingFile = false;
	m_p = parms;
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

	m_res = m_p.res;
	//Constants
	m_rd = 		287; 		// specific gas constant for dry air ( J/[kg K] )
	m_p0 = 		101325.0f;	// standard pressure at sea level (Pa)
	m_gravity = 9.81f;		// gravitational acceleration (m/s²)
	m_lh = 		2501000;	// Latent heat of vaporization of water 2501(kJ/kg)		2501000(J/kg)
	m_cp = 		1003.5;		// specific heat capacity 1003.5 J/(kg K) dry air


	m_cellSize = (m_p.maxAlt-m_p.minAlt)/m_res.y;	//expansion of a cell in meter;

	ScalarField::Ptr density = std::make_shared<ScalarField>();
	density->resize(m_res);
	density->localToWorld(math::V3f(2,2,1));
	density->fill(0.0f);
	density->fill(33.3f,math::Box3f(0.4f,0.1f,0.4f,0.6f,0.4f,0.6f));
	addSubData("density", density);

	VectorField::Ptr velocity = std::make_shared<VectorField>( VectorField::FACE );
	velocity->resize(m_res);
	//velocity->setBound( math::Box3f( math::V3f(-0.51f), math::V3f(0.51f) ) );
	//velocity->fill( math::V3f(1.0f, 1.0f, 1.0f) );
	addSubData("velocity", velocity);

	velocity->getScalarField(0)->fill(0.0f);
	velocity->getScalarField(0)->fill(0.0f,math::Box3f(0.2f,0.1f,0.4f,0.8f,0.4f,0.6f));

	velocity->getScalarField(1)->fill(0.0f);
	velocity->getScalarField(1)->fill(0.0f,math::Box3f(0.4f,0.1f,0.4f,0.6f,0.4f,0.6f));

	velocity->getScalarField(2)->fill(0.0f);
	velocity->getScalarField(2)->fill(0.0f,math::Box3f(0.4f,0.1f,0.4f,0.6f,0.4f,0.6f));




	// read sounding
	if(m_useSoudingFile)
	{
		QFile file(core::expand("$HERE/sounding.txt"));
		if (file.open(QFile::ReadOnly)) {
			char buf[1024];

			for(int i=1; !file.atEnd(); i++)
			{
				qint64 lineLength = file.readLine(buf, sizeof(buf));

				if (lineLength != -1 && i>6)
				{
					QString line = QString::fromUtf8(buf);
					QStringList lineList = line.split(' ', QString::SkipEmptyParts);

					if(lineList[0].toFloat())
					{
						qDebug()<<  lineList[0];
						m_sndPRES.push_back(lineList[0].toFloat());
						m_sndHGHT.push_back(lineList[1].toFloat());
						m_sndTEMP.push_back(lineList[2].toFloat());
						m_sndDWPT.push_back(lineList[3].toFloat());
						m_sndRELH.push_back(lineList[4].toFloat());
						m_sndMIXR.push_back(lineList[5].toFloat());
					}
					else
						break; // Stop after value list ended (skip additional station info)
				}
			}
		}

	}


}



void CloudData::reset()
{
	ScalarField::Ptr pt = std::make_shared<ScalarField>();
	pt->resize(m_res);
	addSubData("pt", pt);

	ScalarField::Ptr qv = std::make_shared<ScalarField>();
	qv->resize(m_res);
	addSubData("qv", qv);

	ScalarField::Ptr qc = std::make_shared<ScalarField>();
	qc->resize(m_res);
	addSubData("qc", qc);




    qDebug() << "reset Cloud Data";

	// Initialize
	//************************************************************ 

	// absolute Temp lookup and absolute Pressure lookup in Pa with Barometric Formula
	for(int y= 0; y<m_res.y; y++){
		float alt = ( (float)y / (float)m_res.y ) * (m_p.maxAlt-m_p.minAlt) + m_p.minAlt;
		m_tLut.push_back( absTemp(alt) );
		m_pLut.push_back( absPressure(alt) );
	}


	// potential temp		=(abs Temp * exner)
	for(int k= 0; k<m_res.z; k++)
		for(int i= 0; i<m_res.x; i++)
			for(int j= 0; j<m_res.y; j++)
			{
				pt->lvalue(i,j,k)= potTemp(m_tLut[j], m_pLut[j] );
			}
	m_pt0 = pt->lvalue(0,0,0);


	//Saturation vapor mixing ratio and water vapor mixing ratio
	for(int j= 0; j<m_res.y; j++)
	{
		for(int i= 0; i<m_res.x; i++)
			for(int k= 0; k<m_res.z; k++)
			{
				float alt = ( (float)j / (float)m_res.y ) * (m_p.maxAlt-m_p.minAlt) + m_p.minAlt;

				if(m_useSoudingFile)
					qv->lvalue(i,j,k) = 0.001f*(soundingValue(m_sndMIXR, alt) * 100.0f) / soundingValue(m_sndRELH, alt);   // qvs = (100% * qv) / humidity%   convert g/kg to kg/kg

				else
					qv->lvalue(i,j,k) = 		qvs(m_tLut[j], m_pLut[j]) * m_p.hum;
			}
		m_qv0Lut.push_back(qv->lvalue(0,j,0));
	}
	m_qv1 = qv->lvalue(0,m_res.y-1,0);







	//************************************************************
	// Initialize Cloud Control Data
	//************************************************************

	m_cv =                     0.001f;
	m_kp =                     0.06f;
	m_ki =                     0.15f;

	ScalarField::Ptr geoPot = std::make_shared<ScalarField>();
	geoPot->resize(m_res);
	addSubData( "geoPot", geoPot );

	// initialize ground gain fields
	for (int i=0; i<m_res.x; i++)
	{
		m_hc.push_back(0.0f);
		m_htar.push_back(math::max(0.0f, float(sin(i*0.05+30))) * 33 + 55);
		m_Kp.push_back(m_kp * m_htar[i]/m_res.y );
		m_Ki.push_back(m_ki * m_htar[i]/m_res.y );
	}

	// geometric Potential field
	for( int k=0;k<m_res.z; ++k )
		for( int j=0;j<m_res.y; ++j )
			for( int i=0;i<m_res.x; ++i )
				{
					if(j<m_htar[i])
						geoPot->lvalue(i,j,k) = 1.0f;
					else
						geoPot->lvalue(i,j,k) = 0;
				}

	// Blur the geometric pot field
	float a = 0.1f * m_res.x * m_res.y;

	for( int steps =0; steps<5; ++steps )
		for( int k=0; k<m_res.z; ++k )
			for( int j=1;j<m_res.y-1;++j )
				for( int i=0;i<m_res.x-0;++i )
				{
					geoPot->lvalue(i,j,k) =  ( geoPot->lvalue(i,j,k)+
																		a*( geoPot->lvalue(i-1,j,k) + geoPot->lvalue(i+1,j,k) +
																			geoPot->lvalue(i,j-1,k) + geoPot->lvalue(i,j+1,k) ))
																																/ (1+4*a);

				}
	for( int j=0;j<m_res.y;++j )
	{
		geoPot->lvalue(0,j,0) = geoPot->lvalue(1,j,0);
		geoPot->lvalue(m_res.x-1,j,0) = geoPot->lvalue(m_res.x-2,j,0);
	}
	ScalarField::Ptr geoPotForce = std::make_shared<ScalarField>();
	geoPotForce->resize(m_res);
	addSubData( "geoPotForce", geoPotForce );

	// multiply matrix on
	for( int k=0; k<m_res.z; ++k )
		for( int j=2;j<m_res.y-2;++j )
			for( int i=2;i<m_res.x-2;++i )
			{
				geoPotForce->lvalue(i,j,k) =( geoPot->lvalue(i,j,k) - geoPot->lvalue(i-1,j,k) ); // staggered x force
			}


	//************************************************************
	// END -  Initialize Cloud Control Data
	//************************************************************
}

float CloudData::getTimestep()
{
	return m_p.dt;
}

math::V3i CloudData::getResolution()
{
	return m_p.res;
}

void CloudData::resize(math::V3i size)
{
	m_res = size;
}

void CloudData::setTimestep(float timestep)
{
	m_p.dt=timestep;
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
					f->lvalue(0,j,k) = m_p.wind;
					f->lvalue(1,j,k) = m_p.wind;
					f->lvalue(res.x-1,j,k) =  m_p.wind;
					f->lvalue(res.x-2,j,k) =  m_p.wind;
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
					//f->lvalue(i,0,k) = 0;
					//f->lvalue(i,res.y-1,k) = 0;
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
					f->lvalue(0,j,k)=			f->lvalue(1,j,k);
					f->lvalue(res.x-1,j,k)=		f->lvalue(res.x-2,j,k);
				}

			for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k)=			f->lvalue(i,1,k);
					f->lvalue(i,res.y-1,k) =	f->lvalue(i,res.y-2,k);
				}
			break;

		// ___________________________________
		// x Velocity
		case 1:
			qDebug()  << "setBounds with b=1 (vel_x)";
			// sides 	= user defined wind
			for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k) =			0;
					f->lvalue(1,j,k) =			0;
					f->lvalue(res.x-1,j,k) =	0;
					f->lvalue(res.x-2,j,k) =	0;
				}

			// bottom 	= noslip
			// top 		= free slip
			//for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k)=			- f->lvalue(i,1,k);
					f->lvalue(i,res.y-1,k)=		0.0f;//f->lvalue(i,res.y-2,k);
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
					f->lvalue(0,j,k)=			-f->lvalue(1,j,k);
					f->lvalue(res.x-1,j,k)=		-f->lvalue(res.x-2,j,k);
				}

			// bottom 	= noslip
			// top 		= free slip
			//for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k) =			0.0f;
					f->lvalue(i,1,k) =			0.0f;
					f->lvalue(i,res.y-1,k) =	math::max(0.0f,f->lvalue(i,res.y-3,k));
					f->lvalue(i,res.y-2,k) =	math::max(0.0f,f->lvalue(i,res.y-3,k));
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
					f->lvalue(0,j,k) = (float) (m_tLut[j] * ( pow( m_p0/m_pLut[j] , 0.286)));
					f->lvalue(res.x-1,j,k) = f->lvalue(0,j,k);
				}

			// bottom 	= noslip
			// top 		= free slip
			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k) = potTemp(m_tLut[0], m_pLut[0]);
					f->lvalue(i,res.y-1,k) = potTemp(m_tLut[res.y-1], m_pLut[res.y-1]);
				}
			break;

		// ___________________________________
		// Water Vapor
		case 5:
		{
			qDebug()  << "setBounds with b=5 (qv)";
			// sides = periodic
			for( int j=0;j<res.y;++j )
			{
				float qv= qvs(m_tLut[j], m_pLut[j]) * m_p.hum;
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
					f->lvalue(i,res.y-1,k) = m_qv1;
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

		// ___________________________________
		// Cloud Water
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

	// ___________________________________
	// set corner values
	f->lvalue(0,0,0)=( f->lvalue(1,0,0) + f->lvalue(0,1,0) )/2;
	f->lvalue(res.x-1,0,0)=( f->lvalue(res.x-2,0,0) + f->lvalue(res.x-1,1,0) )/2;
	f->lvalue(0,res.y-1,0)=( f->lvalue(1,res.y-1,0) + f->lvalue(0,res.y-2,0) )/2;
	f->lvalue(res.x-1,res.y-1,0)=( f->lvalue(res.x-2,res.y-1,0) + f->lvalue(res.x-1,res.y-2,0) )/2;


}


float CloudData::absPressure(float alt)
{
	if(m_useSoudingFile)
		return soundingValue(m_sndPRES, alt) * 100; // hPa in Pa
	else
		return  m_p0* pow(( 1- ( (alt*m_p.tlr)/m_p.t0 ) ),(m_gravity/(m_p.tlr*m_rd)) ) ;
}

float CloudData::absTemp(float alt)
{
	if(m_useSoudingFile)
		return soundingValue(m_sndTEMP, alt) + 273.15f; // C in K
	else
		return m_p.t0 - alt * m_p.tlr;
}

float CloudData::potTemp(float absT, float absP)
{
	return (float) (absT * ( pow( m_p0/absP , 0.286)));
}

float CloudData::potTempToTemp(float pt, float absP)
{
	return pt*pow(absP/m_p0,0.286f);
}

float CloudData::qvs(float t, float p)
{
	// t in K | p in Pa |  qs in kg/kg

	// old
	//return (float) (  (380/p) * exp( (17.67*(t-273.15)) / (t-273.15+243.5))) ;

	// new
	return (RD/RV) * (pvs(t) /( p-(1-RD/RV)*pvs(t) ) ); 				// saturation mixing ratio
}

float CloudData::pvs(float t)
{
	return  610.78f * exp( 17.27f * (t-273.16f)/(t-35.86f) );		// Pressure at saturation
}


float CloudData::soundingValue(std::vector<float> snd, float alt)
{
	int i = 0;
	for(auto it = m_sndHGHT.begin(); it != m_sndHGHT.end();  it++ , i++)
	{
		if(m_sndHGHT[i]>alt) break;
	}

	if(i-1 < 0)
		return snd[0]; //hPa in Pa
	else
	{
		float h1 = m_sndHGHT[i];
		float h0 = m_sndHGHT[math::max(0,i-1)];
		float dist = abs(h1-h0);
		float w1 = 1-(abs(h1-alt) / dist);
		float w0 = 1-(abs(alt-h0) / dist);

		return snd[i]* w1 + snd[math::max(0,i-1)]* w0; // interpolation
	}
}
