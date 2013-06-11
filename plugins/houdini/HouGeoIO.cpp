#include "HouGeoIO.h"

#include <QDebug>

namespace houdini
{
	HouGeo::Ptr HouGeoIO::import( std::istream *in )
	{
		json::JSONReader reader;
		json::JSONLogger logger;
		json::Parser p;

		qDebug() << "HouGeoIO::import CHECK0000000! ";

		if(!p.parse( in, &reader ))
		//if(!p.parse( in, &logger ))
			return HouGeo::Ptr();
		/*
		//return HouGeoPtr();
		// now reader contains the json data (structured after the scheme of the file)
		// we will create an empty HouGeo and have it load its data from the json data
		HouGeoPtr houGeo = HouGeo::create();

		houGeo->load( HouGeo::toObject(reader.getRoot().asArray()) );

		return houGeo;
		*/

		qDebug() << "HouGeoIO::import CHECK! ";

		return HouGeo::Ptr();
	}
}
