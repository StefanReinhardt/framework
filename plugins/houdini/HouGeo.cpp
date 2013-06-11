#include "HouGeo.h"






namespace houdini
{


	HouGeo::HouGeo() : HouGeoAdapter()
	{
		m_numPoints=0;
		m_numVertices=0;
	}

	sint64 HouGeo::pointcount()const
	{
		return m_numPoints;
	}

	sint64 HouGeo::vertexcount()const
	{
		return m_numVertices;
	}

	sint64 HouGeo::primitivecount()const
	{
		return m_primitives.size();
	}

	void HouGeo::getPointAttributeNames( std::vector<std::string> &names )const
	{
		for( auto it = m_pointAttributes.cbegin(); it != m_pointAttributes.cend(); ++it )
			names.push_back( it->second->getName() );
	}

	HouGeoAdapter::Attribute::Ptr HouGeo::getPointAttribute( const std::string &name )
	{
		auto it = m_pointAttributes.find(name);
		if(it != m_pointAttributes.end())
			return it->second;
		return HouGeoAdapter::Attribute::Ptr();
	}

	void HouGeo::getVertexAttributeNames( std::vector<std::string> &names )const
	{
		for( auto it = m_vertexAttributes.cbegin(); it != m_vertexAttributes.cend(); ++it )
			names.push_back( it->second->getName() );
	}

	HouGeoAdapter::Attribute::Ptr HouGeo::getVertexAttribute( const std::string &name )
	{
		auto it = m_vertexAttributes.find(name);
		if(it != m_vertexAttributes.end())
			return it->second;
		return HouGeoAdapter::Attribute::Ptr();
	}


	void HouGeo::getGlobalAttributeNames( std::vector<std::string> &names )const
	{
		for( auto it = m_globalAttributes.cbegin(); it != m_globalAttributes.cend(); ++it )
		{
			names.push_back( it->second->getName() );
		}
	}

	HouGeoAdapter::Attribute::Ptr HouGeo::getGlobalAttribute( const std::string &name )
	{
		auto it = m_globalAttributes.find(name);
		if(it != m_globalAttributes.end())
			return it->second;
		return HouGeoAdapter::Attribute::Ptr();
	}



	HouGeo::Primitive::Ptr HouGeo::getPrimitive( int index )
	{
		if( index < m_primitives.size() )
			return m_primitives[index];
		return HouGeo::Primitive::Ptr();
	}

	HouGeo::Topology::Ptr HouGeo::getTopology()
	{
		return m_topology;
	}


	HouGeo::Ptr HouGeo::create()
	{
		return HouGeo::Ptr( new HouGeo() );
	}



	// Attribute ==============================

	std::string HouGeo::HouAttribute::getName()const
	{
		return name;
	}

	HouGeoAdapter::Attribute::Type HouGeo::HouAttribute::getType()const
	{
		return type;
	}

	int HouGeo::HouAttribute::getTupleSize()const
	{
		return tupleSize;
	}

	HouGeoAdapter::Attribute::Storage HouGeo::HouAttribute::getStorage()const
	{
		return storage;
	}

	void HouGeo::HouAttribute::getPacking( std::vector<int> &packing )const
	{
	}

	HouGeoAdapter::RawPointer::Ptr HouGeo::HouAttribute::getRawPointer()
	{
		return HouGeoAdapter::RawPointer::create( &data[0] );
	}

	int HouGeo::HouAttribute::getNumElements()const
	{
		return numElements;
	}






	// Topology ==============================
	HouGeoAdapter::RawPointer::Ptr HouGeo::HouTopology::getRawPointer()
	{
		return HouGeoAdapter::RawPointer::create(&indexBuffer[0]);
	}




/*










	// a has to be the root of the array from hou geo
	void HouGeo::load( core::json::ObjectPtr o )
	{
		if( o->hasKey("pointcount") )
			m_numPoints = o->get<int>("pointcount", 0);
		if( o->hasKey("vertexcount") )
			m_numVertices = o->get<int>("vertexcount", 0);
		if( o->hasKey("attributes") )
		{
			core::json::ObjectPtr attributes = toObject(o->getArray("attributes"));
			if( attributes->hasKey("pointattributes") )
			{
				core::json::ArrayPtr pointAttributes = attributes->getArray("pointattributes");
				sint64 numPointAttributes = pointAttributes->size();
				for(int i=0;i<numPointAttributes;++i)
				{
					core::json::ArrayPtr pointAttribute = pointAttributes->getArray(i);
					HouAttributePtr attr = loadAttribute( pointAttribute, pointcount() );
					m_pointAttributes.insert( std::make_pair(attr->getName(), attr) );
				}
			}
			if( attributes->hasKey("vertexattributes") )
			{
				core::json::ArrayPtr vertexAttributes = attributes->getArray("vertexattributes");
				sint64 numVertexAttributes = vertexAttributes->size();
				for(int i=0;i<numVertexAttributes;++i)
				{
					core::json::ArrayPtr vertexAttribute = vertexAttributes->getArray(i);
					HouAttributePtr attr = loadAttribute( vertexAttribute, vertexcount() );
					m_vertexAttributes.insert( std::make_pair(attr->getName(), attr) );
				}
			}
			if( attributes->hasKey("globalattributes") )
			{
				core::json::ArrayPtr globalAttributes = attributes->getArray("globalattributes");
				sint64 numGlobalAttributes = globalAttributes->size();
				for(int i=0;i<numGlobalAttributes;++i)
				{
					core::json::ArrayPtr globalAttribute = globalAttributes->getArray(i);
					// TODO: element count argument, how many?!
					HouAttributePtr attr = loadAttribute( globalAttribute, 1 );
					m_globalAttributes.insert( std::make_pair(attr->getName(), attr) );
				}
			}
		}
		if( o->hasKey("topology") )
		{
			loadTopology( toObject(o->getArray("topology")) );
		}
		if( o->hasKey("primitives") )
		{
			core::json::ArrayPtr primitives = o->getArray("primitives");
			int numPrimitives = (int)primitives->size();
			for( int j=0;j<numPrimitives;++j )
			{
				core::json::ArrayPtr primitive = primitives->getArray(j);
				loadPrimitive( primitive );
			}
		}
	}


	HouGeo::HouAttributePtr HouGeo::loadAttribute( core::json::ArrayPtr attribute, sint64 elementCount )
	{
		core::json::ObjectPtr attrDef = toObject(attribute->getArray(0));
		core::json::ObjectPtr attrData = toObject(attribute->getArray(1));

		HouGeo::HouAttributePtr attr = std::make_shared<HouGeo::HouAttribute>();

		std::string attrName = attrDef->get<std::string>("name");
		Attribute::Type attrType = Attribute::type(attrDef->get<std::string>("type"));
		Attribute::Storage attrStorage = Attribute::storage(attrData->get<std::string>("storage"));
		int attrTupleSize = attrData->get<int>("size");
		int attrComponentSize = Attribute::storageSize( attrStorage );

		//int dstTupleSize = (attrTupleSize == 4)?3:attrTupleSize;
		int dstTupleSize = attrTupleSize;
		int dstComponentSize = attrComponentSize;
		attr->data.resize( elementCount*dstTupleSize*dstComponentSize );
	
		if( attrData->hasKey("values") )
		{
			core::json::ObjectPtr values = toObject( attrData->getArray("values") );
			if( values->hasKey("rawpagedata") )
			{
				int elementsPerPage = values->get<int>("pagesize");
				std::vector<ubyte> attrPacking;
				if( values->hasKey("packing") )
				{
					core::json::ArrayPtr packingArray = values->getArray("packing");
					int psize = (int)packingArray->size();
					for( int i=0;i<psize;++i )
						attrPacking.push_back( packingArray->get<ubyte>(i) );
				}else
					attrPacking.push_back( attrTupleSize );

				core::json::ArrayPtr rawPageData = values->getArray("rawpagedata");

				// we need to repack - which when done in a generic way looks like a pain in the butt ======

				//TODO: when reading attributes: use numElements, not pointcount!
				attr->numElements = (int)rawPageData->size()/attrTupleSize;
				int elementsRemaining = attr->numElements;

				// process each page
				int pageIndex = 0;
				while( elementsRemaining>0 )
				{
					int lastElement = pageIndex*elementsPerPage;
					int numElements = std::min( elementsRemaining, elementsPerPage );

					ubyte packIndex = 0;
					for( std::vector<ubyte>::iterator it = attrPacking.begin(); it != attrPacking.end();++it )
					{
						ubyte pack = *it;
						int maxPack = std::min( (int)pack, std::max(0, dstTupleSize-packIndex) );

						if( maxPack == 0 )
							break;
						
						// now iterate over all elements and get values
						for( int i=0;i<numElements;++i )
						{
							int elementIndex = lastElement*attrTupleSize + i*pack;
							int destIndex = (lastElement+i)*dstTupleSize;

							// TODO: uniform arrays!
							for( int p=0;p<maxPack;++p )
								rawPageData->getValue(elementIndex+p).cpyTo( (char *)&(attr->data[(destIndex + packIndex + p)*dstComponentSize]) );

						}

						packIndex += pack;
					}


					elementsRemaining -= numElements;
					lastElement += numElements;
					++pageIndex;
				}

				attr->name = attrName;
				attr->type = attrType;
				attr->storage = attrStorage;
				attr->tupleSize = dstTupleSize;
			}
		}

		return attr;
	}


	void HouGeo::loadTopology( core::json::ObjectPtr o )
	{
		HouTopologyPtr top = std::make_shared<HouTopology>();
		if( o->hasKey("pointref") )
		{
			core::json::ObjectPtr pointref = toObject( o->getArray("pointref") );
			if( pointref->hasKey("indices") )
			{
				core::json::ArrayPtr indices = pointref->getArray("indices");
				sint64 numElements = indices->size();
				for(int i=0;i<numElements;++i)
					top->indexBuffer.push_back( indices->get<int>(i) );
			}
		}
		m_topology = top;
	}

	void HouGeo::loadPrimitive( core::json::ArrayPtr primitive )
	{
		// we follow the scheme from houdini...

		// primitives have 2 arrays:
		//primitive definition
		core::json::ObjectPtr primdef = toObject(primitive->getArray(0));
		std::string primitiveType ="";
		if( primdef->hasKey("type") )
			primitiveType = primdef->get<std::string>("type", "");


		// primitive
		if( primitiveType=="Volume" )
			loadVolumePrimitive( toObject(primitive->getArray(1)) );
		else
		if( primitiveType=="Poly" )
			loadPolyPrimitive( toObject(primitive->getArray(1)) );
		else
		if( (primitiveType=="run")&&(primdef->hasKey("runtype")) )
		{
			if( primdef->get<std::string>( "runtype" ) == "Poly" )
				loadPolyPrimitiveRun( primdef, primitive->getArray(1) );
		}

	}

	// HouGeo::HouVolume ==================================================

	void HouGeo::loadVolumePrimitive( core::json::ObjectPtr volume )
	{
		HouVolumePtr vol = std::make_shared<HouVolume>();
		vol->field = Field<float>::create();

		if( volume->hasKey("res") )
		{
			core::json::ArrayPtr res = volume->getArray("res");
			vol->field->resize(res->get<int>(0), res->get<int>(1), res->get<int>(2));
		}
		if( volume->hasKey("vertex") && volume->hasKey("transform") )
		{
			core::json::ArrayPtr xform = volume->getArray("transform");

			// vertex indexes the indexbuffer (which lives with the topology)
			// the index buffer references the point (which lives with the point attribute)
			// from the point we can work out the translation of the volume

			// transform encodes the rotation and scale in an array of 9 values
			math::Matrix44d houLocalToWorldRotationScale = math::Matrix44d( xform->get<float>(0), xform->get<float>(1), xform->get<float>(2), 0.0,
																			xform->get<float>(3), xform->get<float>(4), xform->get<float>(5), 0.0,
																			xform->get<float>(6), xform->get<float>(7), xform->get<float>(8), 0.0,
																			0.0, 0.0, 0.0, 1.0);


			// TODO!!! - make this more generic - kind of hardcode
			int v = m_topology->indexBuffer[volume->get<int>("vertex")];
			math::Vec3f p = *(math::Vec3f *)&m_pointAttributes.find("P")->second->data[ v*3*sizeof(float) ];

			math::Matrix44d houLocalToWorldTranslation = math::Matrix44d::TranslationMatrix(p);

			math::Matrix44d localToWorld = math::Matrix44d::ScaleMatrix(2.0)*math::Matrix44d::TranslationMatrix(-1.0,-1.0,-1.0)*houLocalToWorldRotationScale*houLocalToWorldTranslation;

			vol->field->setLocalToWorld( localToWorld );
		}
		if( volume->hasKey("voxels") )
		{
			core::json::ObjectPtr voxels = toObject(volume->getArray("voxels"));
			if( voxels->hasKey("tiledarray") )
			{
				core::json::ObjectPtr tiledarray = toObject(voxels->getArray("tiledarray"));

				std::vector<int> compressionTypes;
				// 1 = rawfull
				// 2 = constant

				if( tiledarray->hasKey("compressiontypes") )
				{
					core::json::ArrayPtr ct = tiledarray->getArray("compressiontypes");
					for( int cti=0;cti<ct->size();++cti )
					{
						//std::cout << ct->get<std::string>(cti) << std::endl;
						if( ct->get<std::string>(cti) == "raw" )
							compressionTypes.push_back( 0 );
						else
						if( ct->get<std::string>(cti) == "rawfull" )
							compressionTypes.push_back( 1 );
						else
						if( ct->get<std::string>(cti) == "constant" )
							compressionTypes.push_back( 2 );
						else
							compressionTypes.push_back( -1 );
					}

				}
				if( tiledarray->hasKey("tiles") )
				{
					core::json::ArrayPtr tiles = tiledarray->getArray("tiles");
					sint64 numTiles = tiles->size();
					// looks like houdini uses some spatial tiling where each tile has
					// a resolution of 16x16x16 (or less on boundary tiles)
					// the whole resolution domain is split into such tiles
					// the first tile starts at 0,0,0 and the ordering is identical
					// to the ordering of voxel values (x being the fastest and z slowest)

					math::Vec3i tileEnd;
					tileEnd.x = vol->field->getResolution().x / 16 + 1;
					tileEnd.y = vol->field->getResolution().y / 16 + 1;
					tileEnd.z = vol->field->getResolution().z / 16 + 1;

					if( (tileEnd.x*tileEnd.y*tileEnd.z)!=numTiles )
						throw std::runtime_error("HouGeo::loadVolumePrimitive problem");

					int currentTileIndex = 0;
					math::Vec3i voxelOffset;
					math::Vec3i numVoxels;
					math::Vec3i res = vol->field->getResolution();
					int remainK = res.z;
					for( int tk=0; tk<tileEnd.z;++tk )
					{
						voxelOffset.z = tk*16;
						numVoxels.z = std::min( 16,  remainK );

						int remainJ = res.y;
						for( int tj=0; tj<tileEnd.y;++tj )
						{
							voxelOffset.y = tj*16;
							numVoxels.y = std::min( 16,  remainJ );

							int remainI = res.x;
							for( int ti=0; ti<tileEnd.x;++ti, ++currentTileIndex )
							{
								voxelOffset.x = ti*16;
								numVoxels.x = std::min( 16,  remainI );

								core::json::ObjectPtr tile = toObject(tiles->getArray(currentTileIndex));
								int tileCompression = 1;
								if( tile->hasKey("compression") )
								{
									tileCompression = tile->get<sint32>("compression");
								}
								if( tile->hasKey("data") )
								{
									switch( tileCompression )
									{
									case 0: // raw
									case 1: // rawfull
										{
											core::json::ArrayPtr data = tile->getArray("data");
											int numElements = (int)data->size();

											if( (numVoxels.x*numVoxels.y*numVoxels.z)!=numElements )
												throw std::runtime_error("HouGeo::loadVolumePrimitive problem");

											float *volData = vol->field->getRawPointer();

											int v = 0;
											for( int k=0;k<numVoxels.z;++k )
												for( int j=0;j<numVoxels.y;++j )
													for( int i=0;i<numVoxels.x;++i,++v )
													{
														// TODO: get uniform array and make this more efficient
														volData[(tk*16+k)*res.x*res.y + (tj*16+j)*res.x + (ti*16+i)] = data->get<float>(v);
													}
										}break;
									case 2: // constant
										{
											float data = tile->get<float>("data");
											float *volData = vol->field->getRawPointer();
											int v = 0;
											for( int k=0;k<numVoxels.z;++k )
												for( int j=0;j<numVoxels.y;++j )
													for( int i=0;i<numVoxels.x;++i,++v )
														volData[(tk*16+k)*res.x*res.y + (tj*16+j)*res.x + (ti*16+i)] = data;

										}break;
									case -1:
									default:
										throw std::runtime_error("HouGeo::loadVolumePrimitive unknown compressiontype");
										break;
									};
								}
								remainI -=16;
							}
							remainJ -=16;
						}
						remainK -=16;
					}
				}
			}else // /tiledarray
			if( voxels->hasKey("constantarray") )
			{
				float constantValue = voxels->get<float>( "constantarray" );
				vol->field->fill(constantValue);
			}
		}

		m_primitives.push_back( vol );
	}


	
	math::Vec3i HouGeo::HouVolume::getResolution()const
	{
		return field->getResolution();
	}

	// returns raw pointer to the data
	HouGeoAdapter::RawPointerPtr HouGeo::HouVolume::getRawPointer()
	{
		return HouGeoAdapter::RawPointer::create(field->getRawPointer());
	}


	// HouGeo::HouPoly ==================================================
	void HouGeo::loadPolyPrimitive( core::json::ObjectPtr poly )
	{
		HouPolyPtr pol = std::make_shared<HouPoly>();

		if( !m_topology )
			throw std::runtime_error( "HouGeo::loadPolyPrimitive expects topology to be loaded already!" );

		if( poly->hasKey( "vertex" ) )
		{
			core::json::ArrayPtr vertex = poly->getArray("vertex");
			// these are indices into, well, indices...
			int numVertices = (int) vertex->size();
			pol->m_numPolys = 1;
			pol->m_perPolyVertexCount.push_back(numVertices);
			for(int i=0;i<numVertices;++i)
				pol->m_vertices.push_back( m_topology->indexBuffer[vertex->get<sint32>(i)] );
		}

		m_primitives.push_back( pol );
	}

	void HouGeo::loadPolyPrimitiveRun( core::json::ObjectPtr def, core::json::ArrayPtr run )
	{
		HouPolyPtr pol = std::make_shared<HouPoly>();
		pol->m_numPolys = (int) run->size();
		int vertex = 0;
		for( int i=0;i<pol->m_numPolys;++i )
		{
			core::json::ArrayPtr c = run->getArray(i)->getArray(0);
			int numVertices = (int)c->size();
			pol->m_perPolyVertexCount.push_back(numVertices);
			for( int j=0;j<numVertices; ++j, ++vertex )
				pol->m_vertices.push_back(m_topology->indexBuffer[c->get<sint32>(j)]);
		}
		m_primitives.push_back( pol );
	}

	int HouGeo::HouPoly::numPolys()const
	{
		return m_numPolys;
	}

	int HouGeo::HouPoly::numVertices( int poly )const
	{
		return m_perPolyVertexCount[poly];
	}

	int const *HouGeo::HouPoly::vertices()const
	{
		return &m_vertices[0];
	}


	// MISC =======================================================

	// turns json array into jsonObject (every first entry is key, every second is value)
	core::json::ObjectPtr HouGeo::toObject( core::json::ArrayPtr a )
	{
		core::json::ObjectPtr o = core::json::Object::create();

		int numElements = (int)a->size();
		for( int i=0;i<numElements;i+=2 )
		{
			if( a->getValue(i).isString() )
			{
				std::string key = a->get<std::string>(i);
				core::json::Value value = a->getValue(i+1);
				o->append( key, value );
			}
		}

		return o;
	}




*/




	




}  // namespace houdini
