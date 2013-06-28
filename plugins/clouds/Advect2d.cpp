#include "Advect2d.h"
#include <plugins/clouds/CloudData.h>
#include <core/Data.h>

Advect2d::Advect2d() : Operator()
{
	m_dt= 5.0f;
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

		if (advectionField=="qc")
		{
			qCritical() << "************************** " << advectionField;
			qCritical() << "field" << field->lvalue(50,50,0);
			qCritical() << "field_old" << field_old->lvalue(50,50,0);
		}


		// swap field
		std::swap( field, field_old );

		if (advectionField=="qc")
		{
			qCritical() << "+++ after swap"  << advectionField;
			qCritical() << "field" << field->lvalue(50,50,0);
			qCritical() << "field_old" << field_old->lvalue(50,50,0);
		}

		// ADVECT
		advect(field, field_old, f_v);

		if (advectionField=="qc")
		{
			qCritical() << "+++ after advect";
			qCritical() << "field" << field->lvalue(50,50,0);
			qCritical() << "field_old" << field_old->lvalue(50,50,0);
		}
		// since we swapped pointers we have to swap subdata as well
		so->setSubData( advectionField, field );
		so->setSubData( advectionField+"_old", field_old );

		if (advectionField=="qc")
		{
			qCritical() << "+++ after setSubdata";
			qCritical() << "field" << field->lvalue(50,50,0);
			qCritical() << "field_old" << field_old->lvalue(50,50,0);
		}



	}

}


void Advect2d::advect(ScalarField::Ptr field, ScalarField::Ptr field_old, VectorField::Ptr vecField)
{

	// perform backtracking


	if (advectionField=="qc")
	{
		qCritical() << "+++ after advect load";
		qCritical() << "field" << field->lvalue(50,50,0);
		qCritical() << "field_old" << field_old->lvalue(50,50,0);
	}

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

				if(i==50 && j==50 && advectionField=="qc")
				{
					qCritical() << "i "<<i << "j "<<j << "k "<<k << "evaluate=" <<  field_old->evaluate(math::V3f(x,y,z));;
					qCritical() << "x "<<x << "y "<<y << "z "<<z << "sample=" << field_old->lvalue(55,50,0);
					qCritical() << "i "<<i << "j "<<j << "k "<<k << "value="<<field->lvalue(i,j,k);
				}


			}
	if (advectionField=="qc")
	{
		qCritical() << "+++ after advect end";
		qCritical() << "field" << field->lvalue(50,50,0);
		qCritical() << "field_old" << field_old->lvalue(50,50,0);
	}


}



void Advect2d::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "advectionField", advectionField );
	o.insert( "vecField", vecField);
	o.insert( "m_periodic", m_periodic);
}



void Advect2d::load( QJsonObject &o )
{
	Operator::load( o );

	advectionField = o["advectionField"].toString();
	vecField = o["vecField"].toString();
	m_periodic = o["m_periodic"].toBool();
}

