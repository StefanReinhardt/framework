#include "SimObject.h"

#include <core/Core.h>




SimObject::SimObject() : core::Data()
{
	setTime( 1.0/core::getVariable("$FPS").toDouble() );
}



void SimObject::addSubData( const QString &name, core::Data::Ptr data )
{
	m_subData.push_back( std::make_pair( name, data ) );
}

void SimObject::setSubData(const QString &name , core::Data::Ptr data)
{
	for( auto it = m_subData.begin(), end=m_subData.end();it != end; ++it )
		if( it->first == name )
		{
			it->second = data;
			return;
		}
	addSubData( name, data );
}

bool SimObject::hasSubData( const QString &name )
{
	for( auto it = m_subData.begin(), end=m_subData.end();it != end; ++it )
		if( it->first == name )
			return true;
	return false;
}

// set timestamp for object in s
void SimObject::setTime( double newTime )
{
	m_time = newTime;
}

// returns timestamp for object in s
double SimObject::getTime()const
{
	return m_time;
}

// used for debugging
void SimObject::print()const
{
	qDebug() << "SimObject ====";
	qDebug() << "subData:";
	for( auto it = m_subData.begin(), end=m_subData.end();it != end; ++it )
	{
		qDebug() << "\t " << it->first << "(" << it->second->metaObject()->className() << ")";
	}
}

std::vector<QString> SimObject::getSubDataNames()
{
	std::vector<QString> subDataNames;
	qDebug() << "get SubData Names";
	for( auto it = m_subData.begin(), end=m_subData.end();it != end; ++it )
	{
		subDataNames.push_back(it->first);
		qDebug() << "\t " << it->first;
	}
	return subDataNames;
}


