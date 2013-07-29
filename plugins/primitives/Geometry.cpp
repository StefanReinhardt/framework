#include "Geometry.h"
#include <tuple>

Geometry::Geometry( Geometry::PrimitiveType primType ) : core::Data(), m_primitiveType(primType), m_indexBufferIsDirty(true), m_numPrimitives(0)
{
	switch( primType )
	{
	default:
	case POINT:m_numPrimitiveVertices = 1;break;
	case LINE:m_numPrimitiveVertices = 2;break;
	case TRIANGLE:m_numPrimitiveVertices = 3;break;
	case QUAD:m_numPrimitiveVertices = 4;break;
	//case POLYGON:m_numPrimitiveVertices = 0;break;
	}
}


// removes all attributes and primitives
void Geometry::clear()
{
	for( std::map< std::string, Attribute::Ptr >::iterator it = m_attributes.begin(); it != m_attributes.end(); ++it )
		it->second->clear();

	m_indexBuffer.clear();
	m_numPrimitives = 0;
	m_indexBufferIsDirty = true;
}

Attribute::Ptr Geometry::getAttr( const std::string &name )
{
	std::map<std::string, Attribute::Ptr>::iterator it = m_attributes.find(name);
	if(it != m_attributes.end())
		return it->second;
	return Attribute::Ptr();
}

void Geometry::setAttr( const std::string &name, Attribute::Ptr attr )
{
	m_attributes[name] = attr;
}

bool Geometry::hasAttr( const std::string &name )
{
	std::map<std::string, Attribute::Ptr>::iterator it = m_attributes.find(name);
	return(it != m_attributes.end());
}

void Geometry::removeAttr( const std::string &name )
{
	std::map<std::string, Attribute::Ptr>::iterator it = m_attributes.find(name);
	if( it != m_attributes.end() )
		m_attributes.erase( it );
}

void Geometry::getPointAttributeNames( std::vector<std::string> &names )const
{
	names.clear();
	for( std::map< std::string, Attribute::Ptr >::const_iterator it = m_attributes.begin(), end = m_attributes.end(); it != end; ++it )
		names.push_back( it->first );
}


Geometry::PrimitiveType Geometry::primitiveType()
{
	return m_primitiveType;
}

// implies a clear
void Geometry::setPrimitiveType( PrimitiveType primType )
{
	clear();
	m_primitiveType = primType;
}

unsigned int Geometry::numPrimitives()
{
	return m_numPrimitives;
}

// Point=1; Line=2; Triangle=3; Quad=4; polygon=*
unsigned int Geometry::numPrimitiveVertices()
{
	return m_numPrimitiveVertices;
}

const unsigned char* Geometry::rawIndexPointer()const
{
	return (const unsigned char*)(&m_indexBuffer[0]);
}

unsigned int Geometry::addPoint( unsigned int vId )
{
	m_indexBuffer.push_back(vId);
	m_indexBufferIsDirty = true;
	return m_numPrimitives++;
}

unsigned int Geometry::addLine( unsigned int vId0, unsigned int vId1 )
{
	m_indexBuffer.push_back(vId0);
	m_indexBuffer.push_back(vId1);
	m_indexBufferIsDirty = true;
	return m_numPrimitives++;
}

unsigned int Geometry::addTriangle( unsigned int vId0, unsigned int vId1, unsigned int vId2 )
{
	m_indexBuffer.push_back(vId0);
	m_indexBuffer.push_back(vId1);
	m_indexBuffer.push_back(vId2);
	m_indexBufferIsDirty = true;
	return m_numPrimitives++;
}

unsigned int Geometry::addQuad( unsigned int vId0, unsigned int vId1, unsigned int vId2, unsigned int vId3 )
{
	m_indexBuffer.push_back(vId0);
	m_indexBuffer.push_back(vId1);
	m_indexBuffer.push_back(vId2);
	m_indexBuffer.push_back(vId3);
	m_indexBufferIsDirty = true;
	return m_numPrimitives++;
}


//
// convinience creators ----
//

// TODO: make into grid
Geometry::Ptr createQuad( Geometry::PrimitiveType primType )
{
	Geometry::Ptr result = std::make_shared<Geometry>(primType);

	// unique points
	std::vector<math::Vec3f> pos;
	pos.push_back( math::Vec3f(-1.0f,-1.0f,-3.0f) );
	pos.push_back( math::Vec3f(1.0f,-1.0f,-3.0f) );
	pos.push_back( math::Vec3f(1.0f,1.0f,-3.0f) );
	pos.push_back( math::Vec3f(-1.0f,1.0f,-3.0f) );

	if( primType == Geometry::QUAD )
	{
		// quads
		std::vector< std::tuple<int, int, int, int> > quads;
		quads.push_back( std::make_tuple(3, 2, 1, 0) );

		// split per face (because we have uv shells)
		Attribute::Ptr positions = Attribute::createV3f();
		Attribute::Ptr uv = Attribute::createV2f();

		for( std::vector< std::tuple<int, int, int, int> >::iterator it = quads.begin(); it != quads.end(); ++it )
		{
			std::tuple<int, int, int, int> &quad = *it;
			int i0, i1, i2, i3;

			i0 = positions->appendElement( pos[std::get<0>(quad)] );
			uv->appendElement( math::Vec2f(0.0f, 1.0f) );
			i1 = positions->appendElement( pos[std::get<1>(quad)] );
			uv->appendElement( math::Vec2f(1.0f, 1.0f) );
			i2 = positions->appendElement( pos[std::get<2>(quad)] );
			uv->appendElement( math::Vec2f(1.0f, 0.0f) );
			i3 = positions->appendElement( pos[std::get<3>(quad)] );
			uv->appendElement( math::Vec2f(0.0f, 0.0f) );

			result->addQuad(i0, i1, i2, i3);
		}

		result->setAttr( "P", positions);
		result->setAttr( "UV", uv);
	}else
	if( primType == Geometry::LINE )
	{
	}

	return result;
}


Geometry::Ptr createBox( const math::BoundingBox3f &bound, Geometry::PrimitiveType primType )
{
	Geometry::Ptr result = std::make_shared<Geometry>(primType);

	// unique points
	std::vector<math::Vec3f> pos;
	pos.push_back( math::Vec3f(bound.minPoint.x,bound.minPoint.y,bound.maxPoint.z) );
	pos.push_back( math::Vec3f(bound.minPoint.x,bound.maxPoint.y,bound.maxPoint.z) );
	pos.push_back( math::Vec3f(bound.maxPoint.x,bound.maxPoint.y,bound.maxPoint.z) );
	pos.push_back( math::Vec3f(bound.maxPoint.x,bound.minPoint.y,bound.maxPoint.z) );

	pos.push_back( math::Vec3f(bound.minPoint.x,bound.minPoint.y,bound.minPoint.z) );
	pos.push_back( math::Vec3f(bound.minPoint.x,bound.maxPoint.y,bound.minPoint.z) );
	pos.push_back( math::Vec3f(bound.maxPoint.x,bound.maxPoint.y,bound.minPoint.z) );
	pos.push_back( math::Vec3f(bound.maxPoint.x,bound.minPoint.y,bound.minPoint.z) );

	if( primType == Geometry::QUAD )
	{
		// quads
		std::vector< std::tuple<int, int, int, int> > quads;
		quads.push_back( std::make_tuple(3, 2, 1, 0) );
		quads.push_back( std::make_tuple(4, 5, 6, 7) );
		quads.push_back( std::make_tuple(7, 6, 2, 3) );
		quads.push_back( std::make_tuple(1, 5, 4, 0) );
		quads.push_back( std::make_tuple(6, 5, 1, 2) );
		quads.push_back( std::make_tuple(4, 7, 3, 0) );

		// split per face (because we have uv shells)
		Attribute::Ptr positions = Attribute::createV3f();
		Attribute::Ptr uv = Attribute::createV2f();


		for( std::vector< std::tuple<int, int, int, int> >::iterator it = quads.begin(); it != quads.end(); ++it )
		{
			std::tuple<int, int, int, int> &quad = *it;
			int i0, i1, i2, i3;

			i0 = positions->appendElement( pos[std::get<0>(quad)] );
			uv->appendElement( math::Vec2f(0.0f, 0.0f) );
			i1 = positions->appendElement( pos[std::get<1>(quad)] );
			uv->appendElement( math::Vec2f(1.0f, 0.0f) );
			i2 = positions->appendElement( pos[std::get<2>(quad)] );
			uv->appendElement( math::Vec2f(1.0f, 1.0f) );
			i3 = positions->appendElement( pos[std::get<3>(quad)] );
			uv->appendElement( math::Vec2f(0.0f, 1.0f) );

			result->addQuad(i0, i1, i2, i3);
		}

		result->setAttr( "P", positions);
		result->setAttr( "UV", uv);
	}else
	if( primType == Geometry::LINE )
	{
		/*
		// points
		AttributePtr positions = Attribute::createVec3f();
		for( std::vector<math::Vec3f>::iterator it = pos.begin(), end=pos.end(); it != end; ++it)
			positions->appendElement( *it );
		result->setAttr( "P", positions);
		//lines
		result->addLine( 0, 1 );
		result->addLine( 1, 2 );
		result->addLine( 2, 3 );
		result->addLine( 3, 0 );

		result->addLine( 4, 5 );
		result->addLine( 5, 6 );
		result->addLine( 6, 7 );
		result->addLine( 7, 4 );

		result->addLine( 0, 4 );
		result->addLine( 1, 5 );
		result->addLine( 2, 6 );
		result->addLine( 3, 7 );
		*/
	}

	return result;
}
