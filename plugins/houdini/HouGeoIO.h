#pragma once
#include "HouGeo.h"



namespace houdini
{
	struct HouGeoIO
	{
		//static void      exportGeo( const std::string &path, Geometry::Ptr geo );
		//static Geometry::Ptr                  loadGeo( const std::string &path );
		static HouGeo::Ptr                      import( std::istream *in );
	private:
		//static bool           exportHouGeo( std::ostream *out, HouGeoAdapter *_geo );
		//static bool              exportAttribute( HouGeoAdapter::AttributePtr attr );
		//static HouGeoAdapter                                                    *geo;
		//static core::json::BinaryWriter                                            *writer;
	};
}
