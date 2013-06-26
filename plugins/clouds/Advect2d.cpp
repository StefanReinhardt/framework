#include "Advect2d.h"
#include <plugins/clouds/CloudData.h>
#include <core/Data.h>

Advect2d::Advect2d() : Operator()
{
	m_dt= 1.0f;
	m_periodic=false;
}

void Advect2d::setType(QString field, QString vecField, bool periodic)
{
	this->m_periodic = periodic;
	this->advectionField = field;
	this->vecField = vecField;
}

void Advect2d::apply( SimObject::Ptr so)
{
	qDebug() << "Advect2d::apply";

	// get field to advect
	Data::Ptr f = so->getSubData<Data>(advectionField);
	VectorField::Ptr f_v = so->getSubData<VectorField>(vecField);

	if(std::dynamic_pointer_cast<VectorField>(f))
	{
		VectorField::Ptr field = std::dynamic_pointer_cast<VectorField>(f);

		// create field_OLD
		VectorField::Ptr field_old;
		if(so->hasSubData(advectionField+"_old") )
			field_old = so->getSubData<VectorField>( advectionField+"_old" );
		else
		{
			field_old =std::make_shared<VectorField>(field);
			so->addSubData( advectionField+"_old", field_old );
		}

		// swap fields
		std::swap( field, field_old );

		for(int i=0; i<3; ++i)
		{
			advect(field->getScalarField(i), field_old->getScalarField(i), f_v);
		}

		// since we swapped pointers we have to swap subdata as well
		so->setSubData( advectionField, field );
		so->setSubData( advectionField+"_old", field_old );


	}

	else if(std::dynamic_pointer_cast<ScalarField>(f))
	{
		ScalarField::Ptr field = std::dynamic_pointer_cast<ScalarField>(f);

		// create field_OLD
		ScalarField::Ptr field_old;
		if( so->hasSubData(advectionField+"_old") )
			field_old = so->getSubData<ScalarField>( advectionField+"_old" );
		else
		{
			field_old = std::make_shared<ScalarField>( field );
			so->addSubData( advectionField+"_old", field_old );
		}

		// swap field
		std::swap( field, field_old );

		// ADVECT
		advect(field, field_old, f_v);

		// since we swapped pointers we have to swap subdata as well
		so->setSubData( advectionField, field );
		so->setSubData( advectionField+"_old", field_old );

	}

}


void Advect2d::advect(ScalarField::Ptr field, ScalarField::Ptr field_old, VectorField::Ptr vecField)
{
	// perform backtracking

	math::V3i res= field->getResolution();

	for( int k=0;k<res.z;++k )
		for( int j=0;j<res.y;++j )
			for( int i=0;i<res.x;++i )
			{
				float x = i+field->m_sampleLocation.x;
				float y = j+field->m_sampleLocation.y;
				float z = k;//+sampleLoc.z;

				// Backtrace Particles
				// if backtrace starts on value then sample instead of evaluate
				if(field->m_sampleLocation.x == 0)
					x= x - m_dt * vecField->getScalarField(0)->sample(i,j,k);
				else
					x = x - m_dt * vecField->getScalarField(0)->evaluate(math::V3f(x,y,z));

				if(field->m_sampleLocation.y == 0)
					y = y - m_dt * vecField->getScalarField(1)->sample(i,j,k);
				else
					y = y - m_dt * vecField->getScalarField(1)->evaluate(math::V3f(x,y,z));

				//if(field->m_sampleLocation.z == 0)
				z = k; //k -  &dt * vel_z->sample(i,j,k);

				//Evaluate Field at backtraced position
				field->lvalue(i,j,k) = field_old->evaluate(math::V3f(x,y,z));
			}

}



void Advect2d::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "advectionField", advectionField );
	o.insert( "vecField", vecField);
}



void Advect2d::load( QJsonObject &o )
{
	Operator::load( o );

	advectionField = o["advectionField"].toString();
	vecField = o["vecField"].toString();
}

