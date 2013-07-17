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
	return std::make_shared<Attribute>( Attribute::REAL32, 4 );
}

Attribute::Ptr Attribute::createV3f( int numElements )
{
	return std::make_shared<Attribute>( Attribute::REAL32, 3 );
}

Attribute::Ptr Attribute::createV2f( int numElements )
{
	return std::make_shared<Attribute>( Attribute::REAL32, 2 );
}

Attribute::Ptr Attribute::createFloat()
{
	return std::make_shared<Attribute>( Attribute::REAL32, 1 );
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
