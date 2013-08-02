#pragma once

#include <iostream>
#include <map>

#include <core/Data.h>
#include <math/math.h>
#include <plugins/primitives/Geometry.h>

#include "Attribute.h"


namespace gl
{
	class Geometry // : public core::Data
	{
	public:
		typedef std::shared_ptr<Geometry> Ptr;


		Geometry();
		Geometry( ::Geometry::Ptr geo );
		~Geometry();
		/*
		enum PrimitiveType
		{
			POINT = 1,
			LINE = 2,
			TRIANGLE = 3,
			QUAD = 4
		};

		Geometry( PrimitiveType primType = POINT );

		*/
		void                                         clear(); // removes all attributes and primitives

		// opengl ---
		void                                         bindIndexBuffer();
		int                                          primitiveTypeGL()const;

		// (Point-)Attribute management ---
		Attribute::Ptr                               getAttr( const std::string &name );
		void                                         setAttr( const std::string &name, Attribute::Ptr attr );
		bool                                         hasAttr( const std::string &name );
		//void                                         removeAttr( const std::string &name );
		//void                                         getPointAttributeNames( std::vector<std::string> &names )const;


		// primitive management ---
		unsigned int                                 numPrimitives();
		unsigned int                                 numPrimitiveVertices(); // Point=1; Line=2; Triangle=3; Quad=4
		unsigned int                                 addPoint( unsigned int vertex );
		/*
		//

		//
		PrimitiveType                                primitiveType();
		void                                         setPrimitiveType( PrimitiveType primType ); // implies a clear
		unsigned int                                 numPrimitives();
		unsigned int                                 numPrimitiveVertices(); // Point=1; Line=2; Triangle=3; Quad=4
		const unsigned char*                         rawIndexPointer()const;
		unsigned int                                 addLine( unsigned int vId0, unsigned int vId1 );
		unsigned int                                 addTriangle( unsigned int vId0, unsigned int vId1, unsigned int vId2 );
		unsigned int                                 addQuad( unsigned int vId0, unsigned int vId1, unsigned int vId2, unsigned int vId3 );

		// temp - convinience stuff should go somewhere else
		static Geometry::Ptr addCube( Geometry::Ptr geo, const math::BoundingBox3f &bound = math::BoundingBox3f(math::Vec3f(-0.5f), math::Vec3f(0.5f)), Geometry::PrimitiveType primType = Geometry::QUAD );


*/
	private:
		std::map< std::string, Attribute::Ptr >      m_attributes; //gl attributes
		/*

		std::vector<unsigned int>                    m_indexBuffer;
		bool                                         m_indexBufferIsDirty;
		unsigned int                                 m_numPrimitives;
		unsigned int                                 m_numPrimitiveVertices; // Point=1; Line=2; Triangle=3; Quad=4; polygon:*
		*/
		::Geometry::Ptr                              m_geo;
		unsigned int                                 m_indexBufferId;
		bool                                         m_indexBufferIsDirty;
		int                                          m_primitiveTypeGL;
	};


}
