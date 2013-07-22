#include "Project.h"

#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>
#include <plugins/clouds/CloudData.h>

Project::Project() : Operator()
{
}
void Project::setField(QString name)
{
	this->projectionField = name;
}


void Project::apply( SimObject::Ptr so, float dt )
{
	qDebug() << "Project::apply";

	CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
	VectorField::Ptr vel = so->getSubData<VectorField>(projectionField);

	math::V3i res = cd->getResolution();

	ScalarField::Ptr div = std::make_shared<ScalarField>();
	ScalarField::Ptr q = std::make_shared<ScalarField>();
	div->resize(res);
	q->resize(res);


	float h = 1.0f;

	//calculate divergence field
	for( int k=0;k<res.z-1;++k )
		for( int j=0;j<res.y-1;++j )
			for( int i=0;i<res.x-1;++i )
			{
				/*
				div->lvalue(i,j,k) = h*(	vel->getScalarField(0)->lvalue(i+1,j,k) - vel->getScalarField(0)->lvalue(i,j,k)
										+ 	vel->getScalarField(1)->lvalue(i,j+1,k) - vel->getScalarField(1)->lvalue(i,j,k) );
				*/
				div->lvalue(i,j,k) = h*(	vel->getScalarField(0)->lvalue(i+1,j,k) - vel->getScalarField(0)->lvalue(i,j,k)
										+ 	vel->getScalarField(1)->lvalue(i,j+1,k) - vel->getScalarField(1)->lvalue(i,j,k)
										+ 	vel->getScalarField(2)->lvalue(i,j,k+1) - vel->getScalarField(2)->lvalue(i,j,k) );

				//initialize pressure for gauss seidel relaxation
				q->lvalue(i,j,k) = 0;
			}


	cd->setBounds(0, div);
	cd->setBounds(0,q);


	//SOR - gauss seidel
	//solve for q

	float temp = 0;
	float w = 1.8f;
	float error = std::numeric_limits<float>::max();

	for(int l=0; error>0.01f; l++)
	//for(int l=0; l<20; l++)
	{
		error = 0;
		for( int k=1;k<res.z-1;++k )
				for( int j=1;j<res.y-1;++j )
					for( int i=1;i<res.x-1;++i )
					{
						/*
						temp = (1-w) * q->lvalue(i,j,k) + w *
								(-div->lvalue(i,j,k) + q->lvalue(i-1,j,k) + q->lvalue(i+1,j,k) + q->lvalue(i,j-1,k) + q->lvalue(i,j+1,k)  )/4;
						*/
						temp = (1-w) * q->lvalue(i,j,k) + w *
								(-h*div->lvalue(i,j,k) + q->lvalue(i-1,j,k) + q->lvalue(i+1,j,k) + q->lvalue(i,j-1,k) + q->lvalue(i,j+1,k) + q->lvalue(i,j,k-1) + q->lvalue(i,j,k+1)  )/6;



						error = std::max(error,abs(q->lvalue(i,j,k)-temp));
						q->lvalue(i,j,k)=temp;
					}
	}

	cd->setBounds(0,q);

	// Subtract (nabla)q from u and v (makes vel divergence free)
	// u-(nabla)qx = u-(q right - q left)/2h
	// v-(nabla)qy = v-(q down - q up)/2h
	for( int k=1;k<res.z;++k )
		for( int j=1;j<res.y;++j )
			for( int i=1;i<res.x;++i )
			{
				vel->getScalarField(0)->lvalue(i,j,k) -= ((q->lvalue(i,j,k) - q->lvalue(i-1,j,k)) / h);
				vel->getScalarField(1)->lvalue(i,j,k) -= ((q->lvalue(i,j,k) - q->lvalue(i,j-1,k)) / h);
				vel->getScalarField(2)->lvalue(i,j,k) -= ((q->lvalue(i,j,k) - q->lvalue(i,j,k-1)) / h);
			}

	cd->setBounds(1,vel->getScalarField(0));
	cd->setBounds(2,vel->getScalarField(1));
	cd->setBounds(3,vel->getScalarField(2));





	// divergence field for Debugging
	for( int k=1;k<res.z-1;++k )
		for( int j=1;j<res.y-1;++j )
			for( int i=1;i<res.x-1;++i )
			{
				div->lvalue(i,j,k) = h*(	vel->getScalarField(0)->lvalue(i+1,j,k) - vel->getScalarField(0)->lvalue(i,j,k)
										+ 	vel->getScalarField(1)->lvalue(i,j+1,k) - vel->getScalarField(1)->lvalue(i,j,k)
										+ 	vel->getScalarField(2)->lvalue(i,j,k+1) - vel->getScalarField(2)->lvalue(i,j,k) );
			}

	cd->setBounds2D(0,div);
	if(so->hasSubData("div"))
			so->setSubData("div",div);
	else
			so->addSubData("div", div);
}




void Project::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "projectionField", projectionField );
}


void Project::load( QJsonObject &o )
{
	Operator::load( o );

	projectionField = o["projectionField"].toString();
}

