

#include "Gravity2D.h"
#include <math/Math.h>
#include <plugins/primitives/Attribute.h>


Gravity2D::Gravity2D()
{
}



void Gravity2D::apply(SimObject::Ptr so, float dt)
{
	// gravity force vector
	math::V3f g(0.0f, -0.91f, 0.0f);

	// modify velocity
	Attribute::Ptr velAttr = so->getSubData<Attribute>( "velocity" );
	if( velAttr )
	{
		// for each particle...
		for( int i=0, numElements = velAttr->numElements();i<numElements;++i )
			// ...add force
			velAttr->get<math::V3f>(i)+=dt*g;
	}
}

