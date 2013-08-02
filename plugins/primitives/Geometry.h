#pragma once
#include <core/Data.h>
#include <math/math.h>
#include "Attribute.h"

#include <iostream>
#include <map>



struct Geometry : public core::Data
{
	typedef std::shared_ptr<Geometry> Ptr;

	enum PrimitiveType
	{
		POINT = 1,
		LINE = 2,
		TRIANGLE = 3,
		QUAD = 4
	};

	Geometry( PrimitiveType primType = POINT );

	void                                         clear(); // removes all attributes and primitives


	//
	// (Point-)Attribute management
	//
	Attribute::Ptr                               getAttr( const std::string &name );
	void                                         setAttr( const std::string &name, Attribute::Ptr attr );
	bool                                         hasAttr( const std::string &name );
	void                                         removeAttr( const std::string &name );
	void                                         getPointAttributeNames( std::vector<std::string> &names )const;

	//
	// primitive management
	//
	PrimitiveType                                primitiveType();
	void                                         setPrimitiveType( PrimitiveType primType ); // implies a clear
	unsigned int                                 numPrimitives();
	unsigned int                                 numPrimitiveVertices(); // Point=1; Line=2; Triangle=3; Quad=4
	const unsigned char*                         rawIndexPointer()const;
	unsigned int                                 addPoint( unsigned int vertex );
	unsigned int                                 addLine( unsigned int vId0, unsigned int vId1 );
	unsigned int                                 addTriangle( unsigned int vId0, unsigned int vId1, unsigned int vId2 );
	unsigned int                                 addQuad( unsigned int vId0, unsigned int vId1, unsigned int vId2, unsigned int vId3 );


private:
	std::map< std::string, Attribute::Ptr >      m_attributes;
	PrimitiveType                                m_primitiveType;
	std::vector<unsigned int>                    m_indexBuffer;
	bool                                         m_indexBufferIsDirty;
	unsigned int                                 m_numPrimitives;
	unsigned int                                 m_numPrimitiveVertices; // Point=1; Line=2; Triangle=3; Quad=4; polygon:*
};

// temp - convinience stuff should go somewhere else
Geometry::Ptr createPoints( const std::vector<math::V3f> &points = std::vector<math::V3f>() );
Geometry::Ptr createQuad( Geometry::PrimitiveType primType = Geometry::QUAD );
Geometry::Ptr createBox( const math::BoundingBox3f &bound = math::BoundingBox3f(math::Vec3f(-0.5f), math::Vec3f(0.5f)), Geometry::PrimitiveType primType = Geometry::QUAD );
