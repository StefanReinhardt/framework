#include "Project2d.h"

#include <plugins/primitives/ScalarField.h>
#include <plugins/clouds/CloudData.h>

Project2d::Project2d() : Operator()
{	
}

void Project2d::apply( SimObject::Ptr so )
{

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);


	qDebug() << "Project2d::apply";

	// get velocity fields
	ScalarField::Ptr vel_x = so->getSubData<ScalarField>( "vel_x" );
	ScalarField::Ptr vel_y = so->getSubData<ScalarField>( "vel_y" );

	math::V3i res = vel_x->getResolution();

	ScalarField::Ptr div = std::make_shared<ScalarField>();
	ScalarField::Ptr q = std::make_shared<ScalarField>();
	div->resize(res);
	q->resize(res);

	qDebug() << "res = " << res.x << res.y << res.z;
	float h = 1.0f;


	//calculate divergence field
	//for( int k=1;k<res.z-1;++k )
	{
		int k=0;
		for( int j=1;j<res.y-1;++j )
			for( int i=1;i<res.x-1;++i )
			{
				div->lvalue(i,j,k) = h*(	vel_x->lvalue(i+1,j,k) - vel_x->lvalue(i,j,k)
										+ 	vel_y->lvalue(i,j+1,k) - vel_y->lvalue(i,j,k) );

				//initialize pressure for gauss seidel relaxation
				q->lvalue(i,j,k) = 0;
			}
	}

	cd->setBounds(0, div);
	cd->setBounds(0,q);
	//so->setSubData("div", div);


	//SOR - gauss seidel
	//solve for q

	float temp = 0;
	float w = 1.9f;
	float error = std::numeric_limits<float>::max();

	for(int l=0; error>0.04f; l++)
	//for( int k=1;k<res.z-1;++k )
	{
		error=0;
		int k = 0;

		for( int j=1;j<res.y-1;++j )
			for( int i=1;i<res.x-1;++i )
			{
				temp = (1-w) * q->lvalue(i,j,k) + w *
						(-div->lvalue(i,j,k) + q->lvalue(i-1,j,k) + q->lvalue(i+1,j,k) + q->lvalue(i,j-1,k) + q->lvalue(i,j+1,k)  )/4;
				error = std::max(error,abs(q->lvalue(i,j,k)-temp));
				q->lvalue(i,j,k)=temp;
			}
	}
	cd->setBounds(0,q);
	//so->setSubData("q", q);


	// Subtract (nabla)q from u and v (makes vel divergence free)
	// u-(nabla)qx = u-(q right - q left)/2h
	// v-(nabla)qy = v-(q down - q up)/2h
	//for( int k=1;k<res.z-1;++k )
	{
		int k=0;
		for( int j=1;j<res.y;++j )
			for( int i=1;i<res.x;++i )
			{
				vel_x->lvalue(i,j,k) -= ((q->lvalue(i,j,k) - q->lvalue(i-1,j,k)) / h);
				vel_y->lvalue(i,j,k) -= ((q->lvalue(i,j,k) - q->lvalue(i,j-1,k)) / h);
			}
	}
	cd->setBounds(1,vel_x);
	cd->setBounds(2,vel_y);
	//cd->setBounds(3,vel_z);


}
