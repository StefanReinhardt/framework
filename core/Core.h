#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

#include <QDebug>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>

#include "Plugin.h"
#include "DataFactory.h"
#include "Graph.h"


namespace core
{
	// c-style API ===========================
	void                          init();
	void                          shutdown();
	Graph::Ptr                    load(const QString &filename );
	void                          save(const QString &filename, Graph::Ptr graph );
	Data::Ptr                     create( const QString &type );

	template<typename T>
	std::shared_ptr<T> create( const QString &type )
	{
		return std::dynamic_pointer_cast<T>( create( type ) );
	}

	// Core ===========================
	class Core // public QObject
	{
	public:
		typedef std::shared_ptr<Core> Ptr;
		Core();

		void                                                   loadPlugins();
		Data::Ptr                                              createData( const QString &type );
		void                                                   addDataFactory( DataFactory::Ptr df );

		// used during load/save
		Graph::Ptr                                             load(const QString &filename );
		void                                                   save(const QString &filename, Graph::Ptr graph );
		int                                                    serialize( Data::Ptr data );
		Data::Ptr                                              deserialize( int id );
	private:
		std::vector<Plugin::Ptr>                               m_plugins;
		std::map<QString, DataFactory::Ptr>                    m_factories;

		std::map<int, Data::Ptr>                               m_deserializeMap;
		std::map<Data::Ptr, std::pair<int, QJsonObject> >      m_serializeMap;
		QJsonDocument                                          m_serializeDoc;
	};

	Core::Ptr instance();
}
