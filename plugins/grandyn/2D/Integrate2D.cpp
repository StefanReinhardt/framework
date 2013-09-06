#include "Integrate2D.h"
#include <math/Math.h>
#include <plugins/primitives/Attribute.h>

Integrate2D::Integrate2D() : Operator()
{
}

void Integrate2D::applyImpl( SimObject::Ptr so, float dt )
{
	float damping = 0.9f;
	float roughness = 0.1f;

	// modify position from velocity
	Attribute::Ptr pAttr = so->getSubData<Attribute>( "P" );
	Attribute::Ptr velAttr = so->getSubData<Attribute>( "velocity" );
	if( pAttr && velAttr )
	{
		// for each particle...
		for( int i=0, numElements = velAttr->numElements();i<numElements;++i )
		{
			///*
			math::V3f oldPos = pAttr->get<math::V3f>(i);
			math::V3f &vel = velAttr->get<math::V3f>(i);

			// euler step
			math::V3f newPos;


			// collision detection with ground plane
			float distance = dt*vel.getLength();
			// plane normal
			math::V3f n = math::V3f(0.0f, 1.0f, 0.0f);
			math::Ray3f ray( oldPos, vel.normalized(), 0.0f, distance );
			math::V3f ip;
			if(math::intersectionRayPlane(ray, n, 0.0f, ip ))
			{
				// reflect particle at groundplane
				vel = math::reflect( -vel, n+math::V3f(roughness*(math::g_randomNumber.randomFloat()-0.5f), 0.0f, roughness*(math::g_randomNumber.randomFloat()-0.5f)) )*damping;
				newPos = ip+vel*( distance-(ip-oldPos).getLength() );
			}else
				newPos = oldPos + dt*vel;

			if( newPos.y < 0.0f )
				newPos.y = 0.0f;
			//*/

			/*
			math::V3f oldPos = pAttr->get<math::V3f>(i);
			newPos = oldPos + dt*vel;

			if(newPos.x < world.min.x) {
				newPos.x = world.min.x;
				this.vel.x *= -1;
				this.vel = this.vel.scale(conservation);
			}
			else if(newPos.x > world.max.x - this.size) {
				newPos.x = world.max.x - this.size;
				this.vel.x *= -1;
				this.vel = this.vel.scale(conservation);
			}
			if(newPos.y < world.min.y) {
				newPos.y = world.min.y;
				this.vel.y *= -1;
				this.vel = this.vel.scale(conservation);
			}
			else if(newPos.y > world.max.y - this.size) {
				newPos.y = world.max.y - this.size;
				this.vel.y *= -1;
				this.vel = this.vel.scale(conservation);
			}
			*/


			pAttr->set<math::V3f>(i, newPos);
		}
	}
}

