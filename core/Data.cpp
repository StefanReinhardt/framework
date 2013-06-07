#include "Data.h"

#include <QDebug>

namespace core
{
	Data::~Data()
	{
	}

	void Data::load( QJsonObject &o )
	{
	}

	void Data::store( QJsonObject &o, QJsonDocument &doc )
	{
		o.insert( "type", QJsonValue( QString::fromUtf8(metaObject()->className()) ) );
	}
}
