#include "GraphNode.h"
#include "Graph.h"
#include "Core.h"

namespace core
{

	GraphNode::GraphNode() : Data(), m_name("unnamed")
	{
	}

	QString GraphNode::getName()const
	{
		return m_name;
	}

	bool GraphNode::hasSocket( const QString &name )const
	{
		auto it = m_sockets.find( name );
		if( it != m_sockets.end() )
			return true;
		return false;
	}

	GraphNodeSocket::Ptr GraphNode::addInputSocket( const QString &name )
	{
		GraphNodeSocket::Ptr socket = std::make_shared<GraphNodeSocket>( name, GraphNodeSocket::INPUT );
		addSocket(socket);
		return socket;
	}

	GraphNodeSocket::Ptr GraphNode::addOutputSocket( const QString &name )
	{
		GraphNodeSocket::Ptr socket = std::make_shared<GraphNodeSocket>( name, GraphNodeSocket::OUTPUT );
		addSocket(socket);
		return socket;
	}

	void GraphNode::addSocket( GraphNodeSocket::Ptr socket )
	{
		const QString &name = socket->getName();
		qDebug() << metaObject()->className() << " adding socket " << name;

		if( socket->getDirection() == GraphNodeSocket::OUTPUT )
			socket->m_update = std::bind(&GraphNode::update, this, std::placeholders::_1 );

		m_sockets[ name ] = socket;
	}

	GraphNodeSocket::Ptr GraphNode::getSocket( const QString name )
	{
		auto it = m_sockets.find( name );
		if( it != m_sockets.end() )
			return it->second;
		return GraphNodeSocket::Ptr();
	}

	void GraphNode::update( GraphNodeSocket *socket )
	{
	}

	void GraphNode::store( QJsonObject &o, QJsonDocument &doc )
	{
		Data::store(o,doc);

		// name ---
		o.insert("nodename", QJsonValue(m_name));

		// sockets ----
		QJsonArray sockets = doc.array();
		for( auto it = m_sockets.begin(),end = m_sockets.end(); it != end; ++it )
			sockets.append( core::instance()->serialize( it->second ) );
		o.insert( "sockets", sockets );
	}


	void GraphNode::load( QJsonObject &o )
	{
		Data::load(o);

		// name ---
		m_name = o["nodename"].toString();

		// sockets ----
		QJsonArray sockets = o["sockets"].toArray();
		for( auto it = sockets.begin(), end = sockets.end(); it != end; ++it )
			addSocket( std::dynamic_pointer_cast<GraphNodeSocket>( core::instance()->deserialize(int((*it).toDouble())) ) );
	}

	// prints node to console (used for debugging)
	void GraphNode::print()const
	{
		qDebug() << "\t" << metaObject()->className();
		for( auto it = m_sockets.begin(), end = m_sockets.end(); it != end; ++it )
		{
			auto socket = it->second;
			qDebug() << "\t\t" << socket->getName();
		}
	}

}
