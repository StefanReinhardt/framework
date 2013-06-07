#include "Core.h"

#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStringList>
#include <QDebug>

extern core::Plugin::Ptr getPlugin_primitives( core::Core::Ptr core );
extern core::Plugin::Ptr getPlugin_houdini( core::Core::Ptr core );

namespace core
{
	Core::Ptr g_core = Core::Ptr();

	void init()
	{
		g_core = std::make_shared<Core>();		
		g_core->loadPlugins();
	}

	void shutdown()
	{
		g_core = Core::Ptr();
	}

	Graph::Ptr  load( const QString &filename )
	{
		return g_core->load( filename );
	}

	void save( const QString &filename, Graph::Ptr graph )
	{
		g_core->save( filename, graph );
	}

	Data::Ptr create( const QString &type )
	{
		return g_core->createData( type );
	}

	Core::Ptr instance()
	{
		return g_core;
	}

	// Core =====================================================
	Graph::Ptr Core::load( const QString &filename )
	{
		// open new file
		qDebug() << "loading file " << filename;
		QFile file;
		file.setFileName(filename);
		file.open(QIODevice::ReadOnly | QIODevice::Text);

		QJsonDocument sd = QJsonDocument::fromJson(file.readAll());
		QJsonObject root = sd.object();

		// load data =================
		QJsonObject jsonDataList = root.value("data").toObject();

		// we go over the list in 2 passes.
		// first pass: create all instances
		// second pass: deserialize each instance

		// create all instances -------------
		QStringList keys = jsonDataList.keys();
		for( auto key : keys )
		{
			int id = key.toInt();
			QString type = jsonDataList[key].toObject()["type"].toString();
			auto data = createData( type );
			m_deserializeMap[id] = data;
		}

		// deserialise each instance ----------
		for( auto key : keys )
		{
			int id = key.toInt();
			auto data = m_deserializeMap[id];
			data->load( jsonDataList[key].toObject() );
		}




		Graph::Ptr graph = std::dynamic_pointer_cast<Graph>(deserialize( (int)root["graph"].toDouble() ));

		// clean up
		m_serializeMap.clear();
		m_deserializeMap.clear();

		// get and return the graph data item
		return graph;
	}

	void Core::save( const QString &filename, Graph::Ptr graph )
	{
		// open new file
		qDebug() << "saving file " << filename;
		QFile file;
		file.setFileName(filename);
		file.open(QIODevice::WriteOnly | QIODevice::Text);


		m_serializeDoc = QJsonDocument();


		QJsonObject root = m_serializeDoc.object();



		//
		int graphid = serialize( graph );


		// put all serialised objects into a map id->jsonobject
		QJsonObject data = m_serializeDoc.object();

		for( auto serialized : m_serializeMap )
		{
			int id = serialized.second.first;
			QJsonObject &obj = serialized.second.second;
			data.insert( QString::number( id ), obj );
		}

		root.insert("data", data);

		// we store the id of the graph object
		root.insert("graph", QJsonValue(graphid));


		m_serializeDoc.setObject(root);
		file.write( m_serializeDoc.toJson() );


		// clean up
		m_serializeMap.clear();
		m_deserializeMap.clear();
		m_serializeDoc = QJsonDocument();
	}

	Core::Core()
	{
	}

	Data::Ptr Core::createData( const QString &type )
	{
		auto it = m_factories.find( type );
		if( it != m_factories.end() )
		{
			qDebug() << "creating data " << type;
			return it->second->createInstance();
		}
		qCritical() << "unable to create data " << type;
		return Data::Ptr();
	}

	void Core::addDataFactory( DataFactory::Ptr df )
	{
		qDebug() << "adding datafactory " << df->typeString();
		m_factories[df->typeString()] = df;
	}

	void Core::loadPlugins()
	{
		qDebug() << "\nloading plugins...";

		// core
		addDataFactory( DataFactoryT<Graph>::create(Graph::staticMetaObject.className(), "organizes interconnected graph nodes") );

		// TODO: these should be loaded from dlls
		m_plugins.push_back(getPlugin_primitives( g_core ));
		m_plugins.push_back(getPlugin_houdini( g_core ));
	}

	// used during load/save -------------------
	int Core::serialize( Data::Ptr data )
	{
		auto it = m_serializeMap.find( data );
		if( it != m_serializeMap.end() )
			return it->second.first;


		// id and json object into which we will serialize data
		int id = (int)m_serializeMap.size();
		QJsonObject obj = m_serializeDoc.object();

		// we have to insert the new entry now so that map size increases and nested calls
		// to serialize will generate new ids
		m_serializeMap.insert(std::make_pair( data, std::make_pair( id, obj ) ));

		qDebug() << "serializing " << data->metaObject()->className() << " (" << id << ")";

		// serialize data into object - we have to use []operator because otherwise we wont serialize into
		// the correct obj instance
		data->store( m_serializeMap[data].second, m_serializeDoc );

		return id;
	}


	Data::Ptr Core::deserialize( int id )
	{
		auto it = m_deserializeMap.find( id );
		if( it != m_deserializeMap.end() )
			return it->second;
		qCritical() << "deserialize wasnt able to find data for id " << id;
		return Data::Ptr();
	}



}













