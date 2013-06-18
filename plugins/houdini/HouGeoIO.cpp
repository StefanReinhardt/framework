#include "HouGeoIO.h"


#include <core/Timer.h>

namespace houdini
{
	HouGeoAdapter*                  HouGeoIO::g_geo = 0;
	json::BinaryWriter*             HouGeoIO::g_writer = 0;

	HouGeo::Ptr HouGeoIO::import( std::istream *in )
	{
		json::JSONReader reader;
		json::JSONLogger logger;
		json::Parser p;


		qDebug() << "loading1";
		core::Timer timer;
		timer.start();
		if(!p.parse( in, &reader ))
		//if(!p.parse( in, &logger ))
			return HouGeo::Ptr();
		timer.stop();
		float time_parse = timer.elapsedSeconds();
		qDebug() << "parsing time: " << time_parse;

		//return HouGeo::Ptr();
		timer.reset();
		///*
		// now reader contains the json data (structured after the scheme of the file)
		// we will create an empty HouGeo and have it load its data from the json data
		HouGeo::Ptr houGeo = HouGeo::create();
		timer.start();
		houGeo->load( HouGeo::toObject(reader.getRoot().asArray()) );
		timer.stop();
		float time_load = timer.elapsedSeconds();

		qDebug() << "loading3";

		qDebug() << "loading time: " << time_load;
		qDebug() << "total time: " << (time_load + time_parse);

		return houGeo;
		//*/
		//return HouGeo::Ptr();
	}

	void HouGeoIO::makeLog( const std::string &path, std::ostream *out )
	{
		std::ifstream in( path.c_str(), std::ios_base::in | std::ios_base::binary );
		json::JSONLogger logger(*out);
		json::Parser p;
		p.parse( &in, &logger );
	}







	bool HouGeoIO::xport( std::ostream *out, HouGeoAdapter::Ptr geo, bool binary )
	{
		if( binary )
			g_writer = new json::BinaryWriter( out );
		else
			// todo: ascii writer...
			g_writer = new json::BinaryWriter( out );

		g_writer->jsonBeginArray();

		g_writer->jsonString( "pointcount" );
		g_writer->jsonInt( geo->pointcount() );

		g_writer->jsonString( "vertexcount" );
		g_writer->jsonInt( geo->vertexcount() );

		g_writer->jsonString( "primitivecount" );
		g_writer->jsonInt( geo->primitivecount() );

		// -- topology
		g_writer->jsonString( "topology" );
		g_writer->jsonBeginArray();
			if( geo->getTopology() )
				exportTopology( geo->getTopology() );
		g_writer->jsonEndArray();

		// -- attributes
		g_writer->jsonString( "attributes" );
		g_writer->jsonBeginArray();

			// -- point attributes
			g_writer->jsonString( "pointattributes" );
			g_writer->jsonBeginArray();
				std::vector<std::string> pointAttrNames;
				geo->getPointAttributeNames(pointAttrNames);
				for( std::vector<std::string>::iterator it = pointAttrNames.begin(); it != pointAttrNames.end(); ++it )
					exportAttribute( geo->getPointAttribute(*it) );
			g_writer->jsonEndArray(); // pointattributes


			// -- global attributes
			g_writer->jsonString( "globalattributes" );
			g_writer->jsonBeginArray();
				std::vector<std::string> globalAttrNames;
				geo->getGlobalAttributeNames(globalAttrNames);
				for( std::vector<std::string>::iterator it = globalAttrNames.begin(); it != globalAttrNames.end(); ++it )
					exportAttribute( geo->getGlobalAttribute(*it) );
			g_writer->jsonEndArray(); // globalattributes

		g_writer->jsonEndArray(); // attributes


		// -- primitives
		if( geo->primitivecount() > 0 )
		{
			g_writer->jsonString( "primitives" );
			g_writer->jsonBeginArray();
			for( int i=0;i<geo->primitivecount();++i )
			{
				HouGeoAdapter::Primitive::Ptr prim = geo->getPrimitive(i);

				if( std::dynamic_pointer_cast<HouGeoAdapter::VolumePrimitive>(prim) )
					exportPrimitive(std::dynamic_pointer_cast<HouGeoAdapter::VolumePrimitive>(prim));

			}
			g_writer->jsonEndArray(); // primitives
		}



		g_writer->jsonEndArray(); // /root

		// done
		delete g_writer;g_writer=0;

		return true;
	}






	bool HouGeoIO::exportAttribute( HouGeoAdapter::Attribute::Ptr attr )
	{
		if( !attr )
				return false;

		std::string type;
		std::string storage;
		int size = attr->getTupleSize();
		std::string name = attr->getName();

		if( attr->getType() == HouGeoAdapter::Attribute::ATTR_TYPE_NUMERIC )
			type = "numeric";
		else if( attr->getType() == HouGeoAdapter::Attribute::ATTR_TYPE_STRING )
			type = "string";

		if( attr->getStorage() == HouGeoAdapter::Attribute::ATTR_STORAGE_FPREAL32 )
				storage = "fpreal32";
		else if( attr->getStorage() == HouGeoAdapter::Attribute::ATTR_STORAGE_FPREAL64 )
				storage = "fpreal64";
		else if( attr->getStorage() == HouGeoAdapter::Attribute::ATTR_STORAGE_INT32 )
				storage = "int32";


		// P attribute has to have 4 components, otherwise houdini will become unstable and eventually crash
		if( (name == "P")&&(size!=4)  )
			throw std::runtime_error( "HouGeoIO::exportAttribute: P attribute has to have 4 components, otherwise houdini will become unstable and eventually crash" );

		g_writer->jsonBeginArray();


		// attribute definition ------------
		g_writer->jsonBeginArray();

		g_writer->jsonString( "name" );
		g_writer->jsonString( name );

		g_writer->jsonString( "type" );
		g_writer->jsonString(type);

		g_writer->jsonEndArray(); // definition

		// attribute values ------------
		g_writer->jsonBeginArray(); // value data

		g_writer->jsonString( "size" );
		g_writer->jsonInt( size );

		g_writer->jsonString( "storage" );
		g_writer->jsonString( storage );


		g_writer->jsonString( "values" );
		g_writer->jsonBeginArray();

			g_writer->jsonString( "size" );
			g_writer->jsonInt( attr->getTupleSize() );

			g_writer->jsonString( "storage" );
			g_writer->jsonString( storage );

			g_writer->jsonString( "pagesize" );
			g_writer->jsonInt( 1024 );


			//g_writer->jsonString( "packing" );
			//std::vector<int> packing;
			//attr->getPacking( packing );
			//g_writer->jsonUniformArray( packing );

			//std::cout << "CHECK " << name << std::endl;
			//std::cout << "CHECK " << attr->getNumElements() << std::endl;
			//std::cout << "CHECK " << attr->getTupleSize() << std::endl;
			//attr->getRawPointer();

			g_writer->jsonString( "rawpagedata" );
			if( attr->getStorage() == HouGeoAdapter::Attribute::ATTR_STORAGE_FPREAL32 )
				g_writer->jsonUniformArray<real32>(  (const real32*) attr->getRawPointer()->ptr, attr->getNumElements()*attr->getTupleSize() );
			else if( attr->getStorage() == HouGeoAdapter::Attribute::ATTR_STORAGE_FPREAL64 )
				g_writer->jsonUniformArray<real64>( (const real64*) attr->getRawPointer()->ptr, attr->getNumElements()*attr->getTupleSize() );
			else if( attr->getStorage() == HouGeoAdapter::Attribute::ATTR_STORAGE_INT32 )
				g_writer->jsonUniformArray<sint32>( (const sint32*) attr->getRawPointer()->ptr, attr->getNumElements()*attr->getTupleSize() );

		g_writer->jsonEndArray(); // ?



		g_writer->jsonEndArray(); // /value data



		g_writer->jsonEndArray(); // attribute

		return true;
	}

	// export topo
	// TODO: datatype is int ~~~~
	bool HouGeoIO::exportTopology( HouGeoAdapter::Topology::Ptr topo )
	{
		//std::vector<int> indices;
		std::vector<sint16> indices;
		//topo->getIndices(indices);
		indices.push_back(0);

		g_writer->jsonString( "pointref" );
		g_writer->jsonBeginArray();
			g_writer->jsonString( "indices" );
			g_writer->jsonUniformArray<sint16>(&indices[0], indices.size());
			//g_writer->jsonBeginArray();
			/*
			for( auto it = indices.begin(), end = indices.end(); it != end; ++it )
			{
				qDebug() << *it;
				g_writer->jsonInt64( *it );
			}
			g_writer->jsonEndArray();
			*/
		g_writer->jsonEndArray();

		return true;
	}


	bool HouGeoIO::exportPrimitive( HouGeoAdapter::VolumePrimitive::Ptr volume )
	{
		math::V3i res = volume->getResolution();

		g_writer->jsonBeginArray();

		// primitive type
		g_writer->jsonBeginArray();
			g_writer->jsonString("type");
			g_writer->jsonString("Volume");
		g_writer->jsonEndArray();

		g_writer->jsonBeginArray();
			g_writer->jsonString("vertex");
			g_writer->jsonInt32(volume->getVertex());

			g_writer->jsonString("transform");
			math::M44f houLocalToWorldTranslation = math::M44f::TranslationMatrix(volume->getTransform().getTranslation());
			math::M44f houLocalToWorldRotationScale = math::M44f::ScaleMatrix(0.5f)*math::M44f::TranslationMatrix(1.0,1.0,1.0)*volume->getTransform()*houLocalToWorldTranslation.inverted();
			math::M33f transform( houLocalToWorldRotationScale.ma[0], houLocalToWorldRotationScale.ma[1], houLocalToWorldRotationScale.ma[2],
						houLocalToWorldRotationScale.ma[4], houLocalToWorldRotationScale.ma[5], houLocalToWorldRotationScale.ma[6],
						houLocalToWorldRotationScale.ma[8], houLocalToWorldRotationScale.ma[9], houLocalToWorldRotationScale.ma[10]);
			g_writer->jsonUniformArray<real32>( transform.ma, 9 );

			g_writer->jsonString("res");
			g_writer->jsonUniformArray<sint32>( &res.x, 3 );

			g_writer->jsonString("border");
			g_writer->jsonBeginMap();
				g_writer->jsonKey("type");
				g_writer->jsonString("constant");
				g_writer->jsonKey("value");
				g_writer->jsonReal32(0.0f);
			g_writer->jsonEndMap();

			g_writer->jsonString("compression");
			g_writer->jsonBeginMap();
				g_writer->jsonKey("tolerance");
				g_writer->jsonReal32(0.0f);
			g_writer->jsonEndMap();

			g_writer->jsonString("voxels");
			g_writer->jsonBeginArray();
				g_writer->jsonString("tiledarray");
				g_writer->jsonBeginArray();
					g_writer->jsonString("version");
					g_writer->jsonInt32( 1 );

					g_writer->jsonString("compressiontype");
					// !!! uniform string array?
					g_writer->jsonBeginArray();
						g_writer->jsonString("raw");
						g_writer->jsonString("rawfull");
						g_writer->jsonString("constant");
						g_writer->jsonString("fpreal16");
						g_writer->jsonString("FP32Range");
					g_writer->jsonEndArray();

					g_writer->jsonString("tiles");
					g_writer->jsonBeginArray();

						// get first invalid tileindex in each dimension
						math::Vec3i tileEnd;
						tileEnd.x = res.x / 16 + 1;
						tileEnd.y = res.y / 16 + 1;
						tileEnd.z = res.z / 16 + 1;

						math::Vec3i voxelOffset; // start offset (in voxels) for current tile
						math::Vec3i numVoxels;   // number of voxels for current tile (may differ in each dimension)
						std::vector<real32> data;
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
								for( int ti=0; ti<tileEnd.x;++ti )
								{
									voxelOffset.x = ti*16;
									numVoxels.x = std::min( 16,  remainI );

									// write tile ---
									g_writer->jsonBeginArray();
										g_writer->jsonString("compression");
										g_writer->jsonInt32(0);
										g_writer->jsonString("data");
										data.clear();
										math::V3i voxelEnd( voxelOffset.x + numVoxels.x, voxelOffset.y + numVoxels.y, voxelOffset.z + numVoxels.z );
										for( int k=voxelOffset.z;k<voxelEnd.z;++k )
											for( int j=voxelOffset.y;j<voxelEnd.y;++j )
												for( int i=voxelOffset.x;i<voxelEnd.x;++i )
													data.push_back(volume->getVoxel( i, j, k ));
										g_writer->jsonUniformArray(data);
									g_writer->jsonEndArray();

									remainI -=16;
								}
								remainJ -=16;
							}
							remainK -=16;
						}

					g_writer->jsonEndArray();

				g_writer->jsonEndArray();

			g_writer->jsonEndArray();

			g_writer->jsonString("visualization");
			g_writer->jsonBeginMap();
				g_writer->jsonKey("mode");
				g_writer->jsonString("smoke");
				g_writer->jsonKey("iso");
				g_writer->jsonReal32(0.0f);
				g_writer->jsonKey("density");
				g_writer->jsonReal32(1.0f);
			g_writer->jsonEndMap();

			g_writer->jsonString("taperx");
			g_writer->jsonReal32(1.0f);

			g_writer->jsonString("tapery");
			g_writer->jsonReal32(1.0f);



		g_writer->jsonEndArray();

		g_writer->jsonEndArray();
		return true;
	}

}










