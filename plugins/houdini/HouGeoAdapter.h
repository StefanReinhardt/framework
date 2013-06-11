// TODO:
// packing
// attribute varname mapping
// ascii support

#pragma once

#include <iostream>
#include <memory>
#include <vector>

//#include "Geometry.h"
//#include "core/json.h"
#include <math/Math.h>

#include "types.h"



namespace houdini
{
	struct HouGeoAdapter
	{
		typedef std::shared_ptr<HouGeoAdapter> Ptr;

		struct RawPointer
		{
			typedef std::shared_ptr<RawPointer> Ptr;

			~RawPointer();
			static Ptr           create( void *ptr, bool freeOnDestruction = false );
			void                                                                *ptr;
			bool                                                   freeOnDestruction;
		};

		struct Attribute
		{
			typedef std::shared_ptr<Attribute> Ptr;

			virtual ~Attribute();
			enum Type
			{
				ATTR_TYPE_INVALID  = 0,
				ATTR_TYPE_NUMERIC  = 1,
				ATTR_TYPE_STRING   = 2
			};
			enum Storage
			{
				ATTR_STORAGE_INVALID  = 0,
				ATTR_STORAGE_FPREAL32 = 1,
				ATTR_STORAGE_FPREAL64 = 2,
				ATTR_STORAGE_INT32 = 3
			};
			virtual std::string                                            getName()const;
			virtual Type                                                   getType()const;
			virtual int                                               getTupleSize()const;
			virtual Storage                                             getStorage()const;
			virtual void                     getPacking( std::vector<int> &packing )const;
			virtual int                                             getNumElements()const;
			virtual RawPointer::Ptr                                       getRawPointer();
			static Type                               type( const std::string &typeName );
			static Storage                      storage( const std::string &storageName );
			static int                                     storageSize( Storage storage );
		};

		struct Topology
		{
			typedef std::shared_ptr<Topology> Ptr;

			virtual ~Topology();
			virtual RawPointer::Ptr                                       getRawPointer(); // indexBuffer
		};

		struct Primitive
		{
			typedef std::shared_ptr<Primitive> Ptr;

			virtual ~Primitive(){}
			enum Type
			{
				PRIM_VOLUME,
				PRIM_POLY
			};
		};

		struct VolumePrimitive : public Primitive
		{
			typedef std::shared_ptr<VolumePrimitive> Ptr;

			virtual math::Vec3i                                      getResolution()const;
			virtual RawPointer::Ptr                                       getRawPointer(); // returns raw pointer to the data
		};

		struct PolyPrimitive : public Primitive
		{
			typedef std::shared_ptr<PolyPrimitive> Ptr;

			virtual int                                                   numPolys()const;
			virtual int                                      numVertices( int poly )const;
			virtual int const                                            *vertices()const;
		};


		virtual sint64                                                  pointcount()const;
		virtual sint64                                                 vertexcount()const;
		virtual sint64                                              primitivecount()const;
		virtual void       getPointAttributeNames( std::vector<std::string> &names )const;
		virtual Attribute::Ptr               getPointAttribute( const std::string &name );
		virtual void      getGlobalAttributeNames( std::vector<std::string> &names )const;
		virtual Attribute::Ptr              getGlobalAttribute( const std::string &name );
		virtual Primitive::Ptr                                  getPrimitive( int index );
		virtual Topology::Ptr                                               getTopology();
	};



	//Geometry::Ptr convertHouGeoPrimitive( HouGeoPtr houGeo, int prim = -1 );
}


