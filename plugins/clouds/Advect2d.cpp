#include "Advect2d.h"



#include <plugins/clouds/CloudData.h>

Advect2d::Advect2d() : Operator()
{
	dt= 1.0f;
	qDebug() << "============================== no attribute to advect";

}

void Advect2d::setType(int b, QString name)
{
	this->b = b;
	this->advectionField = name;
}

void Advect2d::apply( SimObject::Ptr so)
{
	qDebug() << "Advect2d::apply";

	// get field to advect
	ScalarField::Ptr field = so->getSubData<ScalarField>(advectionField);

	// get velocity scalarfield ---
	VectorField::Ptr vel = so->getSubData<VectorField>("velocity");

	ScalarField::Ptr vel_x = vel->getScalarField(0);
	ScalarField::Ptr vel_y = vel->getScalarField(1);

	// get temporary scalarfield for update ---
	ScalarField::Ptr field_old;
	if( so->hasSubData("field_old") )
		field_old = so->getSubData<ScalarField>( "field_old" );
	else
	{
		// initialize field_old field from field
		field_old = std::make_shared<ScalarField>( field );
		so->addSubData( "field_old", field_old );
	}

	// make current field -> field_old
	std::swap( field, field_old );

	// perform backtracking
	math::V3i res = field->getResolution();
	for( int k=0;k<res.z;++k )
		for( int j=0;j<res.y;++j )
			for( int i=0;i<res.x;++i )
			{
				float x = i - dt * vel_x->sample(i,j,k);
				float y = j - dt * vel_y->sample(i,j,k);
				float z = k; //k -  &dt * vel_z->sample(i,j,k);

				field->lvalue(i,j,k) = field_old->evaluate(math::V3f(x,y,z));
			}


	// since we swapped pointers we have to swap subdata as well
	so->setSubData( advectionField, field );
	so->setSubData( "field_old", field_old );


	// Boundary settings
	if(std::dynamic_pointer_cast<CloudData>(so))
	{
		CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
		qDebug() << "sucessfully casted SO to cloudData";

		//cd->setBounds(0, field);

		//cd->setBounds(2, vel_y);
		//cd->setBounds(3, vel_z);

	}


}



void Advect2d::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "advectionField", advectionField );
}


void Advect2d::load( QJsonObject &o )
{
	Operator::load( o );

	advectionField = o["advectionField"].toString();
}

