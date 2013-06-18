#include "HouGeo.h"






namespace houdini
{


	HouGeo::HouGeo() : HouGeoAdapter()
	{
	}

	sint64 HouGeo::pointcount()const
	{
		auto it = m_pointAttributes.cbegin();
		if( it != m_pointAttributes.cend() )
			return it->second->getNumElements();
		return 0;
	}

	sint64 HouGeo::vertexcount()const
	{
		if( m_topology )
			return m_topology->getNumIndices();
		return 0;
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

	void HouGeo::addPrimitive( ScalarField::Ptr field )
	{
		// add point which will encode the translation of the volume
		HouAttribute::Ptr pAttr = std::dynamic_pointer_cast<HouAttribute>(getPointAttribute( "P" ));

		// no point attribute yet?
		if( !pAttr )
		{
			// add point attribute
			pAttr = std::make_shared<HouAttribute>();
			pAttr->name = "P";
			pAttr->tupleSize = 4;
			pAttr->storage = HouAttribute::ATTR_STORAGE_FPREAL32;
			pAttr->type = HouAttribute::ATTR_TYPE_NUMERIC;
			setPointAttribute( "P", pAttr );
		}

		math::V3f center = field->localToWorld( math::V3f(0.5f) );
		int index = pAttr->addV4f( math::V4f(center.x, center.y, center.z, 1.0f) );


		if( !m_topology )
			m_topology = std::make_shared<HouTopology>();

		HouVolume::Ptr hvol = std::make_shared<HouVolume>();
		hvol->field = field;

		std::vector<int> indexList;
		indexList.push_back( index );
		hvol->vertex = m_topology->getNumIndices();
		m_topology->addIndices(indexList);


		m_primitives.push_back( hvol );
	}


	void HouGeo::setPointAttribute( const std::string &name, HouAttribute::Ptr attr )
	{
		m_pointAttributes[name] = attr;
	}

	// Attribute ==============================

	HouGeo::HouAttribute::HouAttribute() : Attribute()
	{
		name = "unnamed";
		numElements = 0;
	}

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
		if( !data.empty() )
			return HouGeoAdapter::RawPointer::create( &data[0] );
		return HouGeoAdapter::RawPointer::create( 0 );
	}

	int HouGeo::HouAttribute::getNumElements()const
	{
		return numElements;
	}


	int HouGeo::HouAttribute::addV4f( math::V4f value )
	{
		// TODO: check storage
		// TODO: check type

		if( tupleSize != 4 )
			qCritical() << "tupleSize does not match!";

		int elementSize = storageSize( storage )*tupleSize;
		data.resize( data.size() + elementSize );

		//math::V4f* test = &data[ 0 ];

		*((math::V4f *)(&data[ numElements * elementSize ])) = value;

		return numElements++;
	}




	// Topology ==============================

	void HouGeo::HouTopology::getIndices( std::vector<int> &indices )const
	{
		indices.clear();
		indices.insert( indices.begin(), indexBuffer.begin(), indexBuffer.end() );
	}

	void HouGeo::HouTopology::addIndices( std::vector<int> &indices )
	{
		indexBuffer.insert( indexBuffer.begin(), indices.begin(), indices.end() );
	}

	sint64 HouGeo::HouTopology::getNumIndices()const
	{
		return indexBuffer.size();
	}




	// a has to be the root of the array from hou geo
	void HouGeo::load( json::ObjectPtr o )
	{
		sint64 numVertices = 0;
		sint64 numPoints = 0;
		if( o->hasKey("pointcount") )
			numPoints = o->get<int>("pointcount", 0);
		if( o->hasKey("vertexcount") )
			numVertices = o->get<int>("vertexcount", 0);
		if( o->hasKey("attributes") )
		{
			json::ObjectPtr attributes = toObject(o->getArray("attributes"));
			if( attributes->hasKey("pointattributes") )
			{
				json::ArrayPtr pointAttributes = attributes->getArray("pointattributes");
				sint64 numPointAttributes = pointAttributes->size();
				for(int i=0;i<numPointAttributes;++i)
				{
					json::ArrayPtr pointAttribute = pointAttributes->getArray(i);
					HouAttribute::Ptr attr = loadAttribute( pointAttribute, numPoints );
					m_pointAttributes.insert( std::make_pair(attr->getName(), attr) );
				}
			}
			if( attributes->hasKey("vertexattributes") )
			{
				json::ArrayPtr vertexAttributes = attributes->getArray("vertexattributes");
				sint64 numVertexAttributes = vertexAttributes->size();
				for(int i=0;i<numVertexAttributes;++i)
				{
					json::ArrayPtr vertexAttribute = vertexAttributes->getArray(i);
					HouAttribute::Ptr attr = loadAttribute( vertexAttribute, numVertices );
					m_vertexAttributes.insert( std::make_pair(attr->getName(), attr) );
				}
			}
			if( attributes->hasKey("globalattributes") )
			{
				json::ArrayPtr globalAttributes = attributes->getArray("globalattributes");
				sint64 numGlobalAttributes = globalAttributes->size();
				for(int i=0;i<numGlobalAttributes;++i)
				{
					json::ArrayPtr globalAttribute = globalAttributes->getArray(i);
					// TODO: element count argument, how many?!
					HouAttribute::Ptr attr = loadAttribute( globalAttribute, 1 );
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
			json::ArrayPtr primitives = o->getArray("primitives");
			int numPrimitives = (int)primitives->size();
			for( int j=0;j<numPrimitives;++j )
			{
				json::ArrayPtr primitive = primitives->getArray(j);
				loadPrimitive( primitive );
			}
		}
	}


	HouGeo::HouAttribute::Ptr HouGeo::loadAttribute( json::ArrayPtr attribute, sint64 elementCount )
	{
		json::ObjectPtr attrDef = toObject(attribute->getArray(0));
		json::ObjectPtr attrData = toObject(attribute->getArray(1));

		HouGeo::HouAttribute::Ptr attr = std::make_shared<HouGeo::HouAttribute>();

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
			json::ObjectPtr values = toObject( attrData->getArray("values") );
			if( values->hasKey("rawpagedata") )
			{
				int elementsPerPage = values->get<int>("pagesize");
				std::vector<ubyte> attrPacking;
				if( values->hasKey("packing") )
				{
					json::ArrayPtr packingArray = values->getArray("packing");
					int psize = (int)packingArray->size();
					for( int i=0;i<psize;++i )
						attrPacking.push_back( packingArray->get<ubyte>(i) );
				}else
					attrPacking.push_back( attrTupleSize );

				json::ArrayPtr rawPageData = values->getArray("rawpagedata");

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


	void HouGeo::loadTopology( json::ObjectPtr o )
	{
		HouTopology::Ptr top = std::make_shared<HouTopology>();
		if( o->hasKey("pointref") )
		{
			json::ObjectPtr pointref = toObject( o->getArray("pointref") );
			if( pointref->hasKey("indices") )
			{
				json::ArrayPtr indices = pointref->getArray("indices");
				sint64 numElements = indices->size();
				for(int i=0;i<numElements;++i)
					top->indexBuffer.push_back( indices->get<int>(i) );
			}
		}
		m_topology = top;
	}

	void HouGeo::loadPrimitive( json::ArrayPtr primitive )
	{
		// we follow the scheme from houdini...

		// primitives have 2 arrays:
		//primitive definition
		json::ObjectPtr primdef = toObject(primitive->getArray(0));
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

	void HouGeo::loadVolumePrimitive( json::ObjectPtr volume )
	{
		HouVolume::Ptr vol = std::make_shared<HouVolume>();
		vol->field = std::make_shared<ScalarField>();

		if( volume->hasKey("res") )
		{
			json::ArrayPtr res = volume->getArray("res");
			vol->field->resize(res->get<int>(0), res->get<int>(1), res->get<int>(2));
		}
		if( volume->hasKey("vertex") && volume->hasKey("transform") )
		{
			json::ArrayPtr xform = volume->getArray("transform");

			// vertex indexes the indexbuffer (which lives with the topology)
			// the index buffer references the point (which lives with the point attribute)
			// from the point we can work out the translation of the volume

			// transform encodes the rotation and scale in an array of 9 values
			math::Matrix44d houLocalToWorldRotationScale = math::Matrix44d( xform->get<float>(0), xform->get<float>(1), xform->get<float>(2), 0.0,
																			xform->get<float>(3), xform->get<float>(4), xform->get<float>(5), 0.0,
																			xform->get<float>(6), xform->get<float>(7), xform->get<float>(8), 0.0,
																			0.0, 0.0, 0.0, 1.0);


			int v = m_topology->indexBuffer[volume->get<int>("vertex")];
			math::V3f p(0.0f);

			// TODO!!! - make this more generic - kind of hardcode
			{
				HouAttribute::Ptr pAttr = std::dynamic_pointer_cast<HouAttribute>( getPointAttribute("P") );
				switch(pAttr->storage)
				{
				// in case of 4 component vector, w component will be ignored...
				case Attribute::ATTR_STORAGE_FPREAL32:
					{
						p = *(math::V3f *)&m_pointAttributes.find("P")->second->data[ v*pAttr->tupleSize*sizeof(float) ];
					}break;
				case Attribute::ATTR_STORAGE_FPREAL64:
					{
						p = *(math::V3d *)&m_pointAttributes.find("P")->second->data[ v*pAttr->tupleSize*sizeof(double) ];
					}break;
				}
			}

			math::Matrix44d houLocalToWorldTranslation = math::Matrix44d::TranslationMatrix(p);

			math::Matrix44d localToWorld = math::Matrix44d::ScaleMatrix(2.0)*math::Matrix44d::TranslationMatrix(-1.0,-1.0,-1.0)*houLocalToWorldRotationScale*houLocalToWorldTranslation;

			vol->field->setLocalToWorld( localToWorld );
		}
		if( volume->hasKey("voxels") )
		{
			json::ObjectPtr voxels = toObject(volume->getArray("voxels"));
			if( voxels->hasKey("tiledarray") )
			{
				json::ObjectPtr tiledarray = toObject(voxels->getArray("tiledarray"));

				std::vector<int> compressionTypes;
				// 1 = rawfull
				// 2 = constant

				if( tiledarray->hasKey("compressiontypes") )
				{
					json::ArrayPtr ct = tiledarray->getArray("compressiontypes");
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
					json::ArrayPtr tiles = tiledarray->getArray("tiles");
					sint64 numTiles = tiles->size();
					math::Vec3i res = vol->field->getResolution();

					// looks like houdini uses some spatial tiling where each tile has
					// a resolution of 16x16x16 (or less on boundary tiles)
					// the whole resolution domain is split into such tiles
					// the first tile starts at 0,0,0 and the ordering is identical
					// to the ordering of voxel values (x being the fastest and z slowest)

					// get first invalid tileindex in each dimension
					math::Vec3i tileEnd;
					tileEnd.x = res.x / 16 + 1;
					tileEnd.y = res.y / 16 + 1;
					tileEnd.z = res.z / 16 + 1;

					// sanity check - number of tiles has to match
					if( (tileEnd.x*tileEnd.y*tileEnd.z)!=numTiles )
						throw std::runtime_error("HouGeo::loadVolumePrimitive problem");

					int currentTileIndex = 0;
					math::Vec3i voxelOffset; // start offset (in voxels) for current tile
					math::Vec3i numVoxels;   // number of voxels for current tile (may differ in each dimension)

					// we iterate all tiles starting from slowest to fastest (inner loop)
					// in each iteration we compute the amount of remaining voxels which is either 16 or smaller for boundary tiles
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

								json::ObjectPtr tile = toObject(tiles->getArray(currentTileIndex));
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
											json::ArrayPtr data = tile->getArray("data");
											int numElements = (int)data->size();

											if( (numVoxels.x*numVoxels.y*numVoxels.z)!=numElements )
												throw std::runtime_error("HouGeo::loadVolumePrimitive problem");

											float *volData = vol->field->getRawPointer();

											if( data->isUniform() && (data->m_uniformType == 2) )
											{
												int v = 0;
												for( int k=0;k<numVoxels.z;++k )
													for( int j=0;j<numVoxels.y;++j, v+=numVoxels.x )
														// copy a complete scanline directly
														memcpy( &volData[(tk*16+k)*res.x*res.y + (tj*16+j)*res.x + (ti*16)], &data->m_uniformdata[v*sizeof(float)], numVoxels.x*sizeof(float) );
											}else
											{
												int v = 0;
												for( int k=0;k<numVoxels.z;++k )
													for( int j=0;j<numVoxels.y;++j )
														for( int i=0;i<numVoxels.x;++i,++v )
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


	int HouGeo::HouVolume::getVertex()const
	{
		return vertex;
	}

	real32 HouGeo::HouVolume::getVoxel( int i, int j, int k )const
	{
		return field->sample(i, j, k);
	}
	
	math::Vec3i HouGeo::HouVolume::getResolution()const
	{
		return field->getResolution();
	}

	math::M44f HouGeo::HouVolume::getTransform()const
	{
		return field->m_localToWorld;
	}

	// returns raw pointer to the data
	HouGeoAdapter::RawPointer::Ptr HouGeo::HouVolume::getRawPointer()
	{
		return HouGeoAdapter::RawPointer::create(field->getRawPointer());
	}


	// HouGeo::HouPoly ==================================================
	void HouGeo::loadPolyPrimitive( json::ObjectPtr poly )
	{
		HouPoly::Ptr pol = std::make_shared<HouPoly>();

		if( !m_topology )
			throw std::runtime_error( "HouGeo::loadPolyPrimitive expects topology to be loaded already!" );

		if( poly->hasKey( "vertex" ) )
		{
			json::ArrayPtr vertex = poly->getArray("vertex");
			// these are indices into, well, indices...
			int numVertices = (int) vertex->size();
			pol->m_numPolys = 1;
			pol->m_perPolyVertexCount.push_back(numVertices);
			for(int i=0;i<numVertices;++i)
				pol->m_vertices.push_back( m_topology->indexBuffer[vertex->get<sint32>(i)] );
		}

		m_primitives.push_back( pol );
	}

	void HouGeo::loadPolyPrimitiveRun( json::ObjectPtr def, json::ArrayPtr run )
	{
		HouPoly::Ptr pol = std::make_shared<HouPoly>();
		pol->m_numPolys = (int) run->size();
		int vertex = 0;
		for( int i=0;i<pol->m_numPolys;++i )
		{
			json::ArrayPtr c = run->getArray(i)->getArray(0);
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
	json::ObjectPtr HouGeo::toObject( json::ArrayPtr a )
	{
		json::ObjectPtr o = json::Object::create();

		int numElements = (int)a->size();
		for( int i=0;i<numElements;i+=2 )
		{
			if( a->getValue(i).isString() )
			{
				std::string key = a->get<std::string>(i);
				json::Value value = a->getValue(i+1);
				o->append( key, value );
			}
		}

		return o;
	}








	




}  // namespace houdini
