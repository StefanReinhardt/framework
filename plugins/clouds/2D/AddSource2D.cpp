#include "AddSource2D.h"
#include <core/Data.h>
#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>


AddSource::AddSource()
{
}

void AddSource::setFields(QString field, QString source)
{
	this->m_field = field;
	this->m_source = source;
}


void AddSource::apply(SimObject::Ptr so)
{
	// TODO:
	// - sampling with wold space transformations


	/*
	Data::Ptr f_d = so->getSubData<Data>(m_field);
	Data::Ptr s_d = so->getSubData<Data>(m_source);


	// Scalar field
	if(std::dynamic_pointer_cast<ScalarField>(f_d))
		if(std::dynamic_pointer_cast<ScalarField>(s_d))
		{
			ScalarField::Ptr f_s = std::dynamic_pointer_cast<ScalarField>(f_d);
			ScalarField::Ptr s_s = std::dynamic_pointer_cast<ScalarField>(s_d);

			math::Vec3i res = f_s->getResolution();

			for( int k=0;k<res.z;++k )
				for( int j=0;j<res.y;++j )
					for( int i=0;i<res.x;++i )
						f_s->lvalue(i,j,k) += s_s->lvalue(i,j,k);
		}

	// Vector field
	else if(std::dynamic_pointer_cast<VectorField>(f_d))
		if(std::dynamic_pointer_cast<VectorField>(s_d))
		{
			VectorField::Ptr f_v = std::dynamic_pointer_cast<VectorField>(f_d);
			VectorField::Ptr s_v = std::dynamic_pointer_cast<VectorField>(s_d);

			for(int i=0; i<3; ++i)
			{
				math::Vec3i res = f_v->getScalarField(i)->getResolution();

				for( int k=0;k<res.z;++k )
					for( int j=0;j<res.y;++j )
						for( int i=0;i<res.x;++i )
						{
							f_v->getScalarField(0)->lvalue(i,j,k) += s_v->getScalarField(0)->lvalue(i,j,k);
							f_v->getScalarField(1)->lvalue(i,j,k) += s_v->getScalarField(1)->lvalue(i,j,k);
							f_v->getScalarField(2)->lvalue(i,j,k) += s_v->getScalarField(2)->lvalue(i,j,k);
						}
			}
		}
*/
}


void AddSource::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "m_field", m_field);
	o.insert( "m_source", m_source);
}



void AddSource::load( QJsonObject &o )
{
	Operator::load( o );

	m_source = o["m_source"].toString();
	m_field = o["m_field"].toString();
}


