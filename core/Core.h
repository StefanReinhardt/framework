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
#include "Timer.h"


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


	// global variables =====================
	// used for current frame, etc.
	void                          setVariable( const QString &key, const QVariant &value );
	bool                          hasVariable( const QString &key );
	bool                          isVariableSocket( GraphNodeSocket::Ptr socket ); // used for serialization
	QVariant                      getVariable( const QString &key );
	GraphNodeSocket::Ptr          getVariableSocket( const QString &key );
	QString                       expand( const QString &path );



	// Core ===========================
	class Core // public QObject
	{
	public:
		typedef std::shared_ptr<Core> Ptr;
		Core();

		// plugin management ---
		void                                                   loadPlugins();
		Data::Ptr                                              createData( const QString &type );
		void                                                   addDataFactory( DataFactory::Ptr df );

		// timeline ---
		int                                                    getCurrentFrame()const;
		void                                                   setFrame( int frame );
		void                                                   stepFrame();

		// load/save ---
		Graph::Ptr                                             load(const QString &_filename );
		void                                                   save(const QString &filename, Graph::Ptr graph );
		QJsonValue                                             serialize( Data::Ptr data );
		Data::Ptr                                              deserialize( QJsonValue obj );
		QJsonValue                                             serialize( const QVariant &variant );
		void                                                   deserialize(QJsonValue value, QVariant &variant );
	private:
		std::vector<Plugin::Ptr>                               m_plugins;
		std::map<QString, DataFactory::Ptr>                    m_factories;

		std::map<int, Data::Ptr>                               m_deserializeMap;
		QJsonObject                                            m_deserializeJsonData;
		std::map<Data::Ptr, std::pair<int, QJsonObject> >      m_serializeMap;
		QJsonDocument                                          m_serializeDoc;


	};

	Core::Ptr instance();
}
