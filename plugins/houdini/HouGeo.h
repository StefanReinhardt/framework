#pragma once

#include "HouGeoAdapter.h"
//#include "Field.h"

#include <map>
#include <string>


namespace houdini
{


	// HouGeo ============================================================
	struct HouGeo : public HouGeoAdapter
	{
		typedef std::shared_ptr<HouGeo> Ptr;

		struct HouAttribute : public Attribute
		{
			typedef std::shared_ptr<HouAttribute> Ptr;
			virtual std::string                                            getName()const;
			virtual Type                                                   getType()const;
			virtual int                                               getTupleSize()const;
			virtual Storage                                             getStorage()const;
			virtual void                     getPacking( std::vector<int> &packing )const;
			virtual int                                             getNumElements()const;
			virtual RawPointer::Ptr                                       getRawPointer();

			std::string                                                              name;
			int                                                                 tupleSize;
			Storage                                                               storage;
			Type                                                                     type;
			std::vector<char>                                                        data;
			int                                                               numElements;
		};

		struct HouTopology : public Topology
		{
			typedef std::shared_ptr<HouTopology> Ptr;
			virtual RawPointer::Ptr                                       getRawPointer(); // indexBuffer

			std::vector<int>                                                  indexBuffer;
		};


		struct HouVolume : public VolumePrimitive
		{
			typedef std::shared_ptr<HouVolume> Ptr;
			virtual math::Vec3i                                      getResolution()const;
			virtual RawPointer::Ptr                                       getRawPointer(); // returns raw pointer to the data

			//Field<float>::Ptr                                                       field;
		};

		struct HouPoly : public PolyPrimitive
		{
			typedef std::shared_ptr<HouPoly> Ptr;
			virtual int                                                   numPolys()const;
			virtual int                                      numVertices( int poly )const;
			virtual int const                                            *vertices()const;
			int                                                                m_numPolys;
			std::vector<int>                                         m_perPolyVertexCount; // olds number of vertices for each polygon
			std::vector<int>                                                   m_vertices; // vertex indicess for each vertex
		};





		HouGeo();

		static HouGeo::Ptr                                                           create();


		// inherited from HouGeoAdapter
		virtual sint64                                                      pointcount()const;
		virtual sint64                                                     vertexcount()const;
		virtual sint64                                                  primitivecount()const;
		virtual void           getPointAttributeNames( std::vector<std::string> &names )const;
		virtual Attribute::Ptr                   getPointAttribute( const std::string &name );
		virtual void          getVertexAttributeNames( std::vector<std::string> &names )const;
		virtual Attribute::Ptr                  getVertexAttribute( const std::string &name );
		virtual void          getGlobalAttributeNames( std::vector<std::string> &names )const;
		virtual Attribute::Ptr                  getGlobalAttribute( const std::string &name );
		virtual Primitive::Ptr                                      getPrimitive( int index );
		virtual Topology::Ptr                                                   getTopology();



/*

		void                                                  load( core::json::ObjectPtr o ); // a has to be the root of the array from hou geo
		HouAttribute::Ptr  loadAttribute( core::json::ArrayPtr attribute, sint64 elementCount );
		void                                          loadTopology( core::json::ObjectPtr o );
		void                                  loadPrimitive( core::json::ArrayPtr primitive );
		void                              loadVolumePrimitive( core::json::ObjectPtr volume );
		void                                  loadPolyPrimitive( core::json::ObjectPtr poly );
		void      loadPolyPrimitiveRun( core::json::ObjectPtr def, core::json::ArrayPtr run );
*/

		//static core::json::ObjectPtr                       toObject( core::json::ArrayPtr a ); // turns json array into jsonObject (every first entry is key, every second is value)
	private:
		int                                                                       m_numPoints;
		int                                                                     m_numVertices;
		std::vector<Primitive::Ptr>                                              m_primitives;
		std::map<std::string, HouAttribute::Ptr>                            m_pointAttributes;
		std::map<std::string, HouAttribute::Ptr>                           m_vertexAttributes;
		std::map<std::string, HouAttribute::Ptr>                           m_globalAttributes;
		HouTopology::Ptr                                                           m_topology;
	};










/*

	// ssdfsf ======================================================
	struct AttributeAdapter : public HouGeoAdapter::Attribute
	{
		AttributeAdapter( std::string _name, core::Attribute::Ptr _attr ) : HouGeoAdapter::Attribute(), name(_name), attr(_attr)
		{
		}

		virtual std::string getName()const
		{
			return name;
		}

		virtual Type getType()const
		{
			return HouGeoAdapter::Attribute::ATTR_TYPE_NUMERIC;
		}

		virtual int getTupleSize()const
		{
			// pattr gets special treatment because 3 component attribute will make houdini unstable (it has problems when P has 3 components only)
			if( name == "P" )
				return 4;
			return attr->numComponents();
		}

		virtual int getNumElements()const
		{
			return attr->numElements();
		}

		virtual HouGeoAdapter::Attribute::Storage getStorage()const
		{
			return HouGeoAdapter::Attribute::ATTR_STORAGE_FPREAL32;
		}

		virtual void getPacking( std::vector<int> &packing )const
		{
			packing.clear();
			//for( int i=0;i<getTupleSize();++i )
			//	packing.push_back(1);
			//packing.push_back(getTupleSize());
		}

		virtual HouGeoAdapter::RawPointerPtr getRawPointer()
		{
			// pattr gets special treatment because 3 component attribute will make houdini unstable (it has problems when P has 3 components only)
			if( name == "P" )
			{
				int numElements = attr->numElements();
				float *data = (float *)malloc( sizeof(float)*4*attr->numElements() );
				float *ptr = data;
				for( int i = 0; i<numElements; ++i )
				{
					math::Vec3f &p = attr->get<math::Vec3f>(i);
					//std::cout << p.x << " " << p.y << " " << p.z << std::endl;
					*ptr++ = p.x;
					*ptr++ = p.y;
					*ptr++ = p.z;
					*ptr++ = 1.0f;
				}

				return HouGeoAdapter::RawPointer::create( data, true );
			}
			return HouGeoAdapter::RawPointer::create(attr->rawPointer());
		}



		std::string                             name;
		core::Attribute::Ptr                    attr;
	};


	struct GeoAdapter : public HouGeoAdapter
	{
		GeoAdapter( Geometry::Ptr g ) : HouGeoAdapter(), geo(g)
		{
		}

		virtual sint64 pointcount()const
		{
			return geo->getAttr("P")->numElements();
		}

		virtual void getPointAttributeNames( std::vector<std::string> &names )const
		{
			geo->getPointAttributeNames(names);
		}


		virtual HouGeoAdapter::AttributePtr getPointAttribute( const std::string &name )
		{
			if( geo->hasAttr(name) )
				return HouGeoAdapter::AttributePtr( new AttributeAdapter( name, geo->getAttr(name) ) );
			return HouGeoAdapter::AttributePtr();
		}


		Geometry::Ptr geo;
	};
*/
}  // namespace houdini
