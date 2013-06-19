#include "SimObject.h"






SimObject::SimObject() : core::Data()
{

}



void SimObject::addSubData( const QString &name, core::Data::Ptr data )
{
	m_subData.push_back( std::make_pair( name, data ) );
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




