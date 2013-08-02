#include "Geometry.h"



namespace gl
{
	Geometry::Geometry() : m_indexBufferIsDirty(true)
	{
	}


	Geometry::Geometry( ::Geometry::Ptr geo ) : m_geo(geo), m_indexBufferIsDirty(true)
	{
		switch( geo->primitiveType() )
		{
		default:
		case ::Geometry::POINT: m_primitiveTypeGL = GL_POINTS;break;
		case ::Geometry::LINE: m_primitiveTypeGL = GL_LINES;break;
		case ::Geometry::TRIANGLE: m_primitiveTypeGL = GL_TRIANGLES;break;
		case ::Geometry::QUAD: m_primitiveTypeGL = GL_QUADS;break;
		};

		// create gl wrappers for each attribute
		std::vector<std::string> names;
		geo->getPointAttributeNames(names);
		for( auto name : names )
			m_attributes[name] = std::make_shared<Attribute>( geo->getAttr(name) );

		// initialize indexbuffer
		glGenBuffers(1, &m_indexBufferId);
	}

	Geometry::~Geometry()
	{
		glDeleteBuffers(1, &m_indexBufferId);
	}

	bool Geometry::hasAttr( const std::string &name )
	{
		std::map<std::string, Attribute::Ptr>::iterator it = m_attributes.find(name);
		return(it != m_attributes.end());
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
		m_geo->setAttr(name, attr->getWrapped() );
	}

	// removes all attributes and primitives
	void Geometry::clear()
	{
		for( std::map< std::string, Attribute::Ptr >::iterator it = m_attributes.begin(); it != m_attributes.end(); ++it )
			it->second->clear();
		m_geo->clear();
		m_indexBufferIsDirty = true;
	}


	void Geometry::bindIndexBuffer()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
		int numPrims = m_geo->numPrimitives();
		if( m_indexBufferIsDirty && numPrims )
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numPrims*m_geo->numPrimitiveVertices()*sizeof(unsigned int), m_geo->rawIndexPointer(), GL_STATIC_DRAW);
			m_indexBufferIsDirty = false;
		}
	}

	unsigned int Geometry::numPrimitives()
	{
		return m_geo->numPrimitives();
	}


	// Point=1; Line=2; Triangle=3; Quad=4
	unsigned int Geometry::numPrimitiveVertices()
	{
		return m_geo->numPrimitiveVertices();
	}

	int Geometry::primitiveTypeGL()const
	{
		return m_primitiveTypeGL;
	}



	unsigned int Geometry::addPoint( unsigned int vertex )
	{
		m_indexBufferIsDirty = true;
		return m_geo->addPoint(vertex);
	}




}

/*

using namespace core;

Geometry::Geometry( Geometry::PrimitiveType primType ):m_primitiveType(primType)
{
}




void Geometry::setAttr( const std::string &name, Attribute::Ptr attr )
{
	m_attributes[name] = attr;
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
// convinience creators
//
Geometry::Ptr Geometry::addCube( Geometry::Ptr result, const math::BoundingBox3f &bound, Geometry::PrimitiveType primType )
{
	result->setPrimitiveType(primType);
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
		Attribute::Ptr positions = Attribute::createVec3f();
		Attribute::Ptr uv = Attribute::createVec2f();


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
//		// points
//		AttributePtr positions = Attribute::createVec3f();
//		for( std::vector<math::Vec3f>::iterator it = pos.begin(), end=pos.end(); it != end; ++it)
//			positions->appendElement( *it );
//		result->setAttr( "P", positions);
//		//lines
//		result->addLine( 0, 1 );
//		result->addLine( 1, 2 );
//		result->addLine( 2, 3 );
//		result->addLine( 3, 0 );

//		result->addLine( 4, 5 );
//		result->addLine( 5, 6 );
//		result->addLine( 6, 7 );
//		result->addLine( 7, 4 );

//		result->addLine( 0, 4 );
//		result->addLine( 1, 5 );
//		result->addLine( 2, 6 );
//		result->addLine( 3, 7 );
	}

	return result;
}
*/
