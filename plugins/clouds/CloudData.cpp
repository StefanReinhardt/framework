#include "CloudData.h"


CloudData::CloudData() : SimObject()
{
	dt = 1.0f;

	diff = 0.0000f;         // diffusion
	visc = 0.000000000f;    // viscosity

	//Constants
	rd = 		287; 		// specific gas constant for dry air
	p0 = 		100;		// pressure at sea level (kPa)
	gravity = 	9.81f;		// gravitational acceleration (m/s²)
	lh = 		2501;		// Latent heat of vaporization of water (J/kg)
	cp = 		1005;		// specific heat capacity J/(kg K)

	maxAlt = 	4000;		// altitude in meter on top of sim grid
	tlr = 		0.009f; 	// Kelvin per 1 meter (between 0.55 and 0.99)
	t0 = 		295;		// temp on ground in Kelvin
	hum = 		0.6f;		// humidty
	buoyancy =  0.8f;
	vorticity = 0.3f;
	wind = 		0.0f;
	heatSrc = 	0.0f;

	resolution = math::Vec3i(100,100,1);

	ScalarField::Ptr density = std::make_shared<ScalarField>();
	density->resize(resolution);
	density->localToWorld(math::V3f(2,2,1));
	density->fill(0.0f);
	density->fill(33.3f,math::Box3f(0.4f,0.4f,0,0.95f,0.6f,1.0f));
	addSubData("density", density);

	// TEMP --------
	// staggered grid for velocity test
	VectorField::Ptr velocity = std::make_shared<VectorField>( VectorField::FACE );
	velocity->resize(resolution);
	//velocity->setBound( math::Box3f( math::V3f(-0.51f), math::V3f(0.51f) ) );
	//velocity->fill( math::V3f(1.0f, 1.0f, 1.0f) );
	addSubData("velocity", velocity);


	//vel_x = std::make_shared<ScalarField>();
	//vel_x->resize(resVelX);
	//vel_x->localToWorld(math::V3f(2,2,1));
	velocity->getScalarField(0)->fill(0.00f);
	velocity->getScalarField(0)->fill(0,math::Box3f(0.30f,0.15f,0,0.80f,0.9f,1.0f));
	//addSubData("vel_x", vel_x );

	//vel_y = std::make_shared<ScalarField>();
	//vel_y->resize(resVelY);
	//vel_y->localToWorld(math::V3f(2,2,1));
	velocity->getScalarField(1)->fill(0.0f);
	velocity->getScalarField(1)->fill(0,math::Box3f(0.4f,0.1f,0,0.6f,0.9f,1.0f));
	//addSubData("vel_y", vel_y);

	reset();

	//pt->fill(305.0,math::Box3f(0.4f,0.05f,0,0.60f,0.31f,1.0f));
}

void CloudData::reset()
{
	ScalarField::Ptr pt = std::make_shared<ScalarField>();
	pt->resize(resolution);
	addSubData("pt", pt);

	ScalarField::Ptr qv = std::make_shared<ScalarField>();
	qv->resize(resolution);
	addSubData("qv", qv);

	ScalarField::Ptr qc = std::make_shared<ScalarField>();
	qc->resize(resolution);
	addSubData("qc", qc);


    qDebug() << "reset Cloud Data";

	// Initialize absolute Temp lookup
	//************************************************************
	for(int y= 0; y<resolution.y; y++){
		// Ground Temp - (altitude / 100)* tempLapseRate
		tLut.push_back(t0 - (( (float)y/(float)resolution.y ) * maxAlt) * tlr);
	}

	// Initialize absolute Pressure lookup in kPa
	//************************************************************
	for(int y= 0; y<resolution.y; y++){
		float alt = ( (float)y / (float)resolution.y ) * maxAlt;
		pLut.push_back((float) (p0* pow(( 1- ( (alt*tlr)/t0 ) ),(gravity/(tlr*rd)) )) );
	}

	// Initialize pot temp
	//************************************************************
	int k=0;
	for(int i= 0; i<resolution.x; i++){
		for(int j= 0; j<resolution.y; j++){
			//					K                   kPa/kPa
			pt->lvalue(i,j,k)= (float) (tLut.at(j) * ( pow( (p0/pLut.at(j)) , 0.286)));
			//pt_old->lvalue(i,j,k) = pt->lvalue(i,j,k);
		}
	}

	// Init ground pot temp
	//************************************************************
	pt0 = tLut.at(0) * pow( p0/pLut.at(0) , 0.286);

	// Initialize Saturation vapor mixing ratio and water vapor mixing ratio
	//************************************************************
	//T in celsius
	for(int i= 0; i<resolution.x; i++){
		for(int j= 0; j<resolution.y; j++){
				// temp in °C and p in Pa
			qs		=		(float) (  (380/(pLut.at(j)*1000)  ) * exp( (17.67*(tLut.at(j)-273.15)) / (tLut.at(j)-273.15+243.5))) ;
				qv->lvalue(i,j,k) = 		qs * hum;
				//qv_old->lvalue(i,j,k) =		qs * hum;
		}
	}
}

float CloudData::getTimestep()
{
	return dt;
}

math::V3i CloudData::getResolution()
{
	return resolution;
}

void CloudData::resize(math::V3i size)
{
	resolution = size;
}

void CloudData::setTimestep(float timestep)
{
	dt=timestep;
}


// ========Settings for b
// 0=NeumannBounds || 1=vel_x || 2=vel_y || 3=vel_z || 4=potentialTemperature || 5=waterVapor || 6=cloudWater

void CloudData::setBounds(int b, ScalarField::Ptr f)
{
	math::V3i res = f->getResolution();
	int k=0;
	switch ( b )
	{

		// ___________________________________
		// Neumann Boundaries
		case 0:
			qDebug()  << "setBounds with b=0 (Neumann) res = "<< res.x;

			//for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k)= f->lvalue(1,j,k);
					f->lvalue(res.x-1,j,k)= f->lvalue(res.x-2,j,k);
				}

			//for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k)= f->lvalue(i,1,k);
					f->lvalue(i,res.y-1,k) = f->lvalue(i,res.y-2,k);
				}
			/*
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,j,0) = f->lvalue(i,j,1);
					f->lvalue(i,j,res.z-1) = f->lvalue(i,j,res.z-2);
				}
			*/
			break;

		// ___________________________________
		// x Velocity
		case 1:
			qDebug()  << "setBounds with b=1 (vel_x)";

			// sides 	= user defined wind
			//for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k) = wind;
					f->lvalue(1,j,k) = wind;
					f->lvalue(res.x-1,j,k) = wind;
					f->lvalue(res.x-2,j,k) = wind;
				}

			// bottom 	= noslip
			// top 		= free slip
			//for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					//f->lvalue(i,0,k)=0;
					f->lvalue(i,0,k)=-f->lvalue(i,1,k);
					f->lvalue(i,res.y-1,k)=f->lvalue(i,res.y-2,k);
				}

			// backFront = free slip
			/*
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,j,0) = f->lvalue(i,j,1);
					f->lvalue(i,j,res.z-1) = f->lvalue(i,j,res.z-2);
				}
			*/
			break;

		// ___________________________________
		// y Velocity
		case 2:
			qDebug()  << "setBounds with b=2 (vel_y)";


			// sides = zero
			//for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k)= 0;
					f->lvalue(res.x-1,j,k)= 0;
				}

			// bottom 	= noslip
			// top 		= free slip
			//for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k) = 0;
					f->lvalue(i,1,k) = 0;
					f->lvalue(i,res.y-1,k) = 0;
					f->lvalue(i,res.y-2,k) = 0;
				}
			/*
			// frontBack = zero
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,j,0)= 0;
					f->lvalue(i,j,res.z-1)= 0;
				}
			*/
			break;

		// ___________________________________
		// z Velocity
		case 3:
			qDebug()  << "setBounds with b=3 (vel_z)";

			/*

			// sides = wind
			for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
				}

			// bottom 	= noslip
			// top 		= free slip
			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
				}

			// frontBack = free slip
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
				}
			*/

			break;

		case 4:
			qDebug()  << "setBounds with b=4 (pt)";


			// sides = wind
			for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
				{
					f->lvalue(0,j,k) = tLut.at(j)*pow(100/pLut.at(j), 0.286);
					f->lvalue(res.x-1,j,k) = tLut.at(j)*pow(100/pLut.at(j), 0.286);
				}

			// bottom 	= noslip
			// top 		= free slip
			for( int k=0;k<res.z;++k )
				for( int i=0;i<res.x;++i )
				{
					f->lvalue(i,0,k) = tLut.at(0)*pow(100/pLut.at(0), 0.286);
					f->lvalue(i,res.y-1,k) = tLut.at(res.y-1)*pow(100/pLut.at(res.y-1), 0.286);
				}

			// frontBack = free slip
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					//f->lvalue(i,j,0) = tLut.at(j)*pow(100/pLut.at(j), 0.286);
					//f->lvalue(i,j,res.z-1) = tLut.at(j)*pow(100/pLut.at(j), 0.286);
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
					f->lvalue(i,0,k) = 33.3f;
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

			// frontBack = periodic?
			for( int j=0;j<res.y;++j )
				for( int i=0;i<res.x;++i )
				{
					//f->lvalue(i,j,0) =		0;
					//f->lvalue(i,j,res.z-1) =	0;
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
