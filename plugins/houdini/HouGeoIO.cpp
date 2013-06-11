#include "HouGeoIO.h"


namespace houdini
{
	HouGeo::Ptr HouGeoIO::import( std::istream *in )
	{
		/*
		core::json::JSONReader reader;
		core::json::JSONLogger logger;
		core::json::Parser p;
		if(!p.parse( in, &reader ))
		//if(!p.parse( in, &logger ))
			return HouGeoPtr();

		//return HouGeoPtr();
		// now reader contains the json data (structured after the scheme of the file)
		// we will create an empty HouGeo and have it load its data from the json data
		HouGeoPtr houGeo = HouGeo::create();

		houGeo->load( HouGeo::toObject(reader.getRoot().asArray()) );

		return houGeo;
		*/

		return HouGeo::Ptr();
	}
}
