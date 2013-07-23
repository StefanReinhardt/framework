#include "Diffuse2D.h"

#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>
#include <plugins/clouds/CloudData.h>

Diffuse2D::Diffuse2D() : Operator()
{
	m_diff = 0.0;
}


void Diffuse2D::apply( SimObject::Ptr so, float dt )
{
	timer.start();

	qDebug() << "Diffuse2D::apply";

	ScalarField::Ptr field = so->getSubData<ScalarField>(m_diffusionField);
	math::V3i res = field->getResolution();


	ScalarField::Ptr field_old;
	if(so->hasSubData(m_diffusionField+"_old") )
	{
		field_old = so->getSubData<ScalarField>( m_diffusionField+"_old" );
	}
	else
	{
		field_old =std::make_shared<ScalarField>(field);
		so->addSubData( m_diffusionField+"_old", field_old );
	}

	// swap fields
	std::swap( field, field_old );

	float a=dt*m_diff*res.x*res.y;

	int k=0;
	for( int j=1;j<res.y-1;++j )
		for( int i=1;i<res.x-1;++i )
		{
			field->lvalue(i,j,k) =  ( field_old->lvalue(i,j,k)+
																a*( field->lvalue(i-1,j,k) + field->lvalue(i+1,j,k) +
																	field->lvalue(i,j-1,k) + field->lvalue(i,j+1,k) ))
																														/ (1+4*a);
		}

	// since we swapped pointers we have to swap subdata as well
	so->setSubData( m_diffusionField, field );
	so->setSubData( m_diffusionField+"_old", field_old );

	timer.stop();
	qCritical() << "Diffusion:" << core::getVariable("$F").toString() << ":" << timer.elapsedSeconds();
}



void Diffuse2D::setField(QString name)
{
	m_diffusionField = name;
}
void Diffuse2D::setDiffusion(float diff)
{
	m_diff = diff;
}


void Diffuse2D::store( QJsonObject &o, QJsonDocument &doc )
{
	Operator::store( o, doc );

	o.insert( "diffusionField", m_diffusionField );
	o.insert( "diffusion", m_diff );
}


void Diffuse2D::load( QJsonObject &o )
{
	Operator::load( o );

	m_diffusionField = o["diffusionField"].toString();
	m_diff = o["diffusion"].toDouble();
}

