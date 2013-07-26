#include "Operator.h"









Operator::Operator() : core::Data(), m_enabled(true)
{

}

void Operator::setEnabled( bool enabled )
{
	m_enabled = enabled;
	if( m_enabled )
		qDebug() << this->metaObject()->className() << " enabled";
	else
		qDebug() << this->metaObject()->className() << " disabled";
}

bool Operator::isEnabled()const
{
	return m_enabled;
}

// dt in seconds
void Operator::apply( SimObject::Ptr so, float dt )
{
	if( m_enabled )
		applyImpl( so, dt );
}


void Operator::applyImpl( SimObject::Ptr so, float dt )
{
	// nop
}
