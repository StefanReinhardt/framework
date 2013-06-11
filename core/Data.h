#pragma once

#include <memory>
#include <string>
#include <vector>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

namespace core
{
	class Data : public QObject
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<Data> Ptr;
		virtual ~Data();

		virtual void                       store( QJsonObject &o, QJsonDocument &doc );
		virtual void                       load( QJsonObject &o );
	};
}
