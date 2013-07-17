#include "Integrate2D.h"
#include <math/Math.h>
#include <plugins/primitives/Attribute.h>

Integrate2D::Integrate2D() : Operator()
{
}

void Integrate2D::apply( SimObject::Ptr so, float dt )
{
	// modify position from velocity
	Attribute::Ptr pAttr = so->getSubData<Attribute>( "P" );
	Attribute::Ptr velAttr = so->getSubData<Attribute>( "velocity" );
	if( pAttr && velAttr )
	{
		// for each particle...
		for( int i=0, numElements = velAttr->numElements();i<numElements;++i )
			// ...modify position from velocity
			pAttr->get<math::V3f>(i)+=dt*velAttr->get<math::V3f>(i);
	}
}

