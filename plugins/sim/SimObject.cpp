#include "SimObject.h"






SimObject::SimObject() : core::Data()
{

}



void SimObject::addSubData( const QString &name, core::Data::Ptr data )
{
	m_subData.push_back( std::make_pair( name, data ) );
}





