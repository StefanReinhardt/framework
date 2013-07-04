#include "VortexConfinement.h"
#include <plugins/clouds/CloudData.h>


VortexConfinement::VortexConfinement()
{
}

void VortexConfinement::setField(QString name)
{
this->vortField = name;
}



void VortexConfinement::apply(SimObject::Ptr so)
{
	if (vortField == 0)
	{
		qCritical() << "vortex confinement: no field set!";
		return;
	}

	qCritical() << "apply: vortex confinement";

	vel_x = so->getSubData<VectorField>(vortField)->getScalarField(0);
	vel_y = so->getSubData<VectorField>(vortField)->getScalarField(1);
	vel_z = so->getSubData<VectorField>(vortField)->getScalarField(2);


	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);

	math::V3i res = vel_x->getResolution();
	res = math::V3i(res.x-1,res.y,res.z);


	// TODO:
	//Scalar Field to Vector if Vort in 3D

	// create vorticity field


	if(so->hasSubData("vorticity") )
	{
		vorticity = so->getSubData<VectorField>( "vorticity" );
	}
	else
	{
		vorticity =std::make_shared<VectorField>();
		vorticity->resize(res);
		so->addSubData( "vorticity", vorticity );
	}


	float nab_nx, nab_ny, nab_nz, mag_n, nx, ny, nz, f_x, f_y, f_z, w1,w2,w3;

	// TODO: index to res - 2 ??

	//Calculate vorticity magnitude field = n
	for( int k=1;k<res.z-3;++k )
		for (int i=1; i<res.x-3; i++)
			for (int j=1; j<res.y-3; j++)
			{
				//vorticity->lvalue(i,j,k)= abs(curl(i,j,k));
				// absolute value of w
				curl(i,j,k, true);
			}




	//Calculate vorticity Gradient N = (nabla n) / ||n||
	for( int k=2;k<res.z-3;++k )
		for( int j=2;j<res.y-3;++j )
			for( int i=2;i<res.x-3;++i )
			{
				//calc nabla n for x and y by central difference
				//(n = del |w|)
				nab_nx =( vorticity->getScalarField(0)->lvalue(i+1,j,k) -vorticity->getScalarField(0)->lvalue(i-1,j,k) ) / 2;
				nab_ny =( vorticity->getScalarField(0)->lvalue(i,j+1,k) -vorticity->getScalarField(0)->lvalue(i,j-1,k) ) / 2;
				nab_nz =( vorticity->getScalarField(0)->lvalue(i,j,k+1) -vorticity->getScalarField(0)->lvalue(i,j,k-1) ) / 2;

				//calculate magnitude of the vector (nab_nx , nab_ny)
				//add small value to prevent divide by zero
				//(|n|)
				mag_n = (float) sqrt(nab_nx*nab_nx + nab_ny*nab_ny + nab_nz*nab_nz) + 0.00000001f;

				// N = ( n/|n| )
				nx = nab_nx / mag_n;
				ny = nab_ny / mag_n;
				nz = nab_nz / mag_n;

				// F = N x vorticity
				curl(i,j,k,false);
				w1 = vorticity->getScalarField(0)->lvalue(i,j,k);
				w2 = vorticity->getScalarField(1)->lvalue(i,j,k);
				w3 = vorticity->getScalarField(2)->lvalue(i,j,k);

				f_x = ny * w3 - nz * w2;
				f_y = nz * w1 - nx * w3;
				f_z = nx * w2 - ny * w1;


				//vorticity->getScalarField(0)->lvalue(i,j,k) = f_x;
				//vorticity->getScalarField(1)->lvalue(i,j,k) = f_y;
				//vorticity->getScalarField(2)->lvalue(i,j,k) = f_z;

				// TODO:
				// (f_x1+f_x)/2



			}

	//Calculate vorticity Gradient N = (nabla n) / ||n||
	for( int k=2;k<res.z-3;++k )
		for( int j=2;j<res.y-3;++j )
			for( int i=2;i<res.x-3;++i )
			{

				vel_x->lvalue(i,j,k) +=  (vorticity->getScalarField(0)->lvalue(i-1,j,k)+vorticity->getScalarField(0)->lvalue(i,j,k))/2 * cd->vorticity;
				vel_y->lvalue(i,j,k) +=	 (vorticity->getScalarField(1)->lvalue(i,j-1,k)+vorticity->getScalarField(1)->lvalue(i,j,k))/2 * cd->vorticity;
				vel_z->lvalue(i,j,k) +=	 (vorticity->getScalarField(2)->lvalue(i,j,k-1)+vorticity->getScalarField(2)->lvalue(i,j,k))/2 * cd->vorticity;
			}

}


void VortexConfinement::curl(int i, int j, int k, bool abs){

	//evaluate velocity values at cell faces - then central diff

	// w1 =  (w_j+1 - w_j-1 - v_k+1 + v_k-1)/2h
	float w1 =	(vel_z->evaluate(math::V3f(i+0.5f,j+1.0f,k+0.5f))-vel_z->evaluate(math::V3f(i+0.5f,j,k+0.5f))
				-vel_y->evaluate(math::V3f(i+0.5f,j+0.5f,k+1.0f))+vel_y->evaluate(math::V3f(i+0.5f,j+0.5f,k)))/2;

	// w2 =  (u_k+1 - u_k-1 - w_i+1 + w_i-1)/2h

	float w2 =	(vel_x->evaluate(math::V3f(i+0.5f,j+0.5f,k+1.0f))-vel_x->evaluate(math::V3f(i+0.5f,j+0.5f,k))
				-vel_z->evaluate(math::V3f(i+1.0f,j+0.5f,k+0.5f))+vel_z->evaluate(math::V3f(i,j+0.5f,k+0.5f)))/2;

	// w2 =  (v_i+1 - v_i-1 - u_j+1 + u_ j-1)/2h
	float w3 =	(vel_y->evaluate(math::V3f(i+1.0f,j+0.5f,k+0.5f))-vel_y->evaluate(math::V3f(i,j+0.5f,k+0.5f))
				-vel_x->evaluate(math::V3f(i+0.5f,j+1.0f,k+0.5f))+vel_x->evaluate(math::V3f(i+0.5f,j,k+0.5f)))/2;

	if(abs)
		vorticity->getScalarField(0)->lvalue(i,j,k)= sqrt(w1*w1+w2*w2+w3*w3);
	else
	{
		vorticity->getScalarField(0)->lvalue(i,j,k) = w1;
		vorticity->getScalarField(1)->lvalue(i,j,k) = w2;
		vorticity->getScalarField(2)->lvalue(i,j,k) = w3;
	}


}






// Serialize
void VortexConfinement::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "vortField", vortField );
}


void VortexConfinement::load( QJsonObject &o )
{
	Operator::load( o );

	vortField = o["vortField"].toString();
}

