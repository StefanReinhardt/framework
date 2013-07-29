#include "Attribute.h"

#include <core/types.h>


Attribute::Attribute( ComponentType componentType, char numComponents ) : core::Data(),
	m_componentType(componentType),
	m_numComponents(numComponents),
	m_numElements(0),
	m_isDirty(true),
	m_elementSize( componentSize(componentType)*numComponents )
{

}

int Attribute::numElements()const
{
	return m_numElements;
}

int Attribute::numComponents()const
{
	return m_numComponents;
}

int Attribute::elementSize()const
{
	return m_elementSize;
}

Attribute::ComponentType Attribute::componentType()const
{
	return m_componentType;
}

const void *Attribute::rawPointer()const
{
	return &m_data[0];
}

void *Attribute::rawPointer()
{
	return &m_data[0];
}

void Attribute::clear()
{
	m_data.clear();
	m_numElements = 0;
	m_isDirty = true;
}

void Attribute::resize( int numElements )
{
	if( numElements == m_numElements )
		return;
	m_numElements = numElements;
	m_data.resize(m_numElements*m_elementSize);
}












//
// convinience creators ======================
//
Attribute::Ptr Attribute::createM33f()
{
	return std::make_shared<Attribute>( Attribute::REAL32, 9 );

}

Attribute::Ptr Attribute::createM44f()
{
	return std::make_shared<Attribute>( Attribute::REAL32, 16 );
}

Attribute::Ptr Attribute::createV4f( int numElements )
{
	Attribute::Ptr attr = std::make_shared<Attribute>( Attribute::REAL32, 4 );
	attr->resize(numElements);
	return attr;
}

Attribute::Ptr Attribute::createV3f( int numElements )
{
	Attribute::Ptr attr = std::make_shared<Attribute>( Attribute::REAL32, 3 );
	attr->resize(numElements);
	return attr;
}

Attribute::Ptr Attribute::createV2f( int numElements )
{
	Attribute::Ptr attr = std::make_shared<Attribute>( Attribute::REAL32, 2 );
	attr->resize(numElements);
	return attr;
}

Attribute::Ptr Attribute::createFloat(int numElements)
{
	Attribute::Ptr attr = std::make_shared<Attribute>( Attribute::REAL32, 1 );
	attr->resize(numElements);
	return attr;
}

Attribute::Ptr Attribute::createInt()
{
	return std::make_shared<Attribute>( Attribute::SINT32, 1 );
}


int Attribute::componentSize( Attribute::ComponentType ct )
{
	switch(ct)
	{
	case SINT32:return sizeof(sint32);
	case REAL32:return sizeof(real32);
	case REAL64:return sizeof(real64);
	default:
		throw std::runtime_error( "unknown component type" );
	};
	return -1;
}

Attribute::ComponentType Attribute::componentType( const QString &ct )
{
	if( ct == "fpreal32" )
		return REAL32;
	else
	if( ct == "fpreal64" )
		return REAL64;
	else
	if( ct == "int32" )
		return SINT32;
	return INVALID;
}
