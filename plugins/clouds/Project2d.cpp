#include "Project2d.h"

#include <plugins/primitives/ScalarField.h>

Project2d::Project2d() : Operator()
{	
}

void Project2d::apply( SimObject::Ptr so )
{
	qDebug() << "Project2d::apply";

	// get velocity fields
	ScalarField::Ptr vel_x = so->getSubData<ScalarField>( "vel_x" );
	ScalarField::Ptr vel_y = so->getSubData<ScalarField>( "vel_y" );

	// get temporary scalarfield for update ---
	//ScalarField::Ptr density_old;
	//density_old = so->getSubData<ScalarField>( "density_old" );
	//density_old = std::make_shared<ScalarField>( density );


	math::V3i res = vel_x->getResolution();



	ScalarField::Ptr div = std::make_shared<ScalarField>();
	ScalarField::Ptr q = std::make_shared<ScalarField>();
	div->resize(res);
	q->resize(res);

	float h = 1.0f;

	//calculate divergence field
	for( int k=1;k<res.z-1;++k )
		for( int j=1;j<res.y-1;++j )
			for( int i=1;i<res.x-1;++i )
			{
				div->lvalue(i,j,k) = h*(	vel_x->lvalue(i+1,j,k) - vel_x->lvalue(i,j,k)
										+ 	vel_x->lvalue(i,j+1,k) - vel_y->lvalue(i,j,k) );

				//initialize pressure for gauss seidel relaxation
				q->lvalue(i,j,k) = 0;
			}


	// SOR - Relaxation
	float temp = 0;
	float w = 1.9f;
	float error = 1.5f;

	for(int l=0; error>0.00001f; l++)
		for( int k=1;k<res.z-1;++k )
			for( int j=1;j<res.y-1;++j )
				for( int i=1;i<res.x-1;++i )
				{
					temp = (1-w) * q->lvalue(i,j,k) + w *
							(-div->lvalue(i,j,k) + q->lvalue(i-1,j,k) + q->lvalue(i+1,j,k) + q->lvalue(i,j-1,k) + q->lvalue(i,j+1,k)  )/4;
					error = abs(q->lvalue(i,j,k)-temp);
					q->lvalue(i,j,k)=temp;
				}


	for( int k=1;k<res.z-1;++k )
		for( int j=1;j<res.y-1;++j )
			for( int i=1;i<res.x-1;++i )
			{
				vel_x->lvalue(i,j,k) -= (q->lvalue(i,j,k) - q->lvalue(i-4,j,k)) / h;
				vel_y->lvalue(i,j,k) -= (q->lvalue(i,j,k) - q->lvalue(i,j-1,k)) / h;
			}


}
