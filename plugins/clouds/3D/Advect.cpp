#include "Advect.h"
#include <plugins/clouds/CloudData.h>
#include <core/Data.h>

#include <core/Core.h>

Advect::Advect() : Operator()
{
	m_dt= 1.00f;
	m_periodic=false;
}

void Advect::setType(QString field, QString vecField, bool periodic)
{
	this->m_periodic = periodic;
	this->advectionField = field;
	this->vecField = vecField;
}

void Advect::apply( SimObject::Ptr so, float dt)
{

	if(advectionField==0 || vecField==0)
	{
		qCritical() << "Advect: no fields set!";
		return;
	}

	qDebug() << "Advect:: apply";

	// get field to advect
	Data::Ptr f = so->getSubData<Data>(advectionField);
	VectorField::Ptr f_v = so->getSubData<VectorField>(vecField);

	// set Timestep
	if(std::dynamic_pointer_cast<CloudData>(so))
	{
		CloudData::Ptr cd = std::dynamic_pointer_cast<CloudData>(so);
		m_dt = cd->m_parms.m_dt;
	}

	// IF Vector field is advected
	if(std::dynamic_pointer_cast<VectorField>(f))
	{
		VectorField::Ptr field = std::dynamic_pointer_cast<VectorField>(f);
		// create field_OLD
		VectorField::Ptr field_old;
		if(so->hasSubData(advectionField+"_old") )
		{
			field_old = so->getSubData<VectorField>( advectionField+"_old" );
		}
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
		{
			field_old = so->getSubData<ScalarField>( advectionField+"_old" );
		}
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



void Advect::advect(ScalarField::Ptr field, ScalarField::Ptr field_old, VectorField::Ptr vecField)
{
	//TODO:
	// Worldspace transformation

	// perform backtracking
	math::V3i res= field->getResolution();

	for( int k=0;k<res.z;++k )
		for( int j=0;j<res.y;++j )
			for( int i=0;i<res.x;++i )
			{
				float x = i+field->m_sampleLocation.x;
				float y = j+field->m_sampleLocation.y;
				float z = k+field->m_sampleLocation.z;

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


				if(field->m_sampleLocation.z == 0)
					z = z - m_dt * vecField->getScalarField(2)->sample(i,j,k);
				else
					z = z - m_dt * vecField->getScalarField(2)->evaluate(math::V3f(x,y,z));

				// Boundary conditions
				if(m_periodic)
				{
					// then repeat

					float intpart;
					//x = (((int)(x))%(int)(res.x-2)) == 0 ?  res.x-2 + modf(x, &i)  :  (((int)(x))%(int)(res.x-2)) +modf(x, &i);
					//y = (((int)(y))%(int)(res.y-2)) == 0 ?  res.y-2 + modf(y, &i)  :  (((int)(y))%(int)(res.y-2)) +modf(y, &i);
					//z = (((int)(z))%(int)(res.z-2)) == 0 ?  res.z-2 + modf(z, &i)  :  (((int)(z))%(int)(res.z-2)) +modf(z, &i);

					x = (int)x % res.x + modf(x, &intpart);
					y = (int)y % res.y + modf(y, &intpart);
					z = (int)z % res.z + modf(z, &intpart);

				}

				else
				{
					//qCritical() << "x vorher "<< x;
					// clamp to Boundary cell
					x = std::max(0.0f,std::min(x,(float)(res.x-1.0f)));
					y = std::max(0.0f,std::min(y,(float)(res.y-1.0f)));
					z = std::max(0.0f,std::min(z,(float)(res.z-1.0f)));
					//qCritical() << "x nachher "<< x;

				}

				//Evaluate Field at backtraced position
				field->lvalue(i,j,k) = field_old->evaluate(math::V3f(x,y,z));
			}

}



void Advect::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "advectionField", advectionField );
	o.insert( "vecField", vecField);
	o.insert( "m_periodic", m_periodic);
}



void Advect::load( QJsonObject &o )
{
	Operator::load( o );

	advectionField = o["advectionField"].toString();
	vecField = o["vecField"].toString();
	m_periodic = o["m_periodic"].toBool();
}

