#include "Graph.h"

#include "Core.h"


namespace core
{

	Graph::Graph() : Data()
	{
	}

	GraphNode::Ptr Graph::createNode( const QString &type, const QString &name )
	{
		GraphNode::Ptr node = create<core::GraphNode>(type);
		if( node )
		{
			qDebug() << "Graph: creating node " << node->metaObject()->className();
			if( name.isEmpty() )
			{
				// generate node name
				QString newName;
				int i = 1;
				do
				{
					newName = QString("%1%2").arg(type, QString::number(i));
				}while( hasNode(newName) );
				node->m_name = newName;
			}
			else
				node->m_name = name;
			addNode( node );
		}else
			qCritical() << "Graph: unable to create node " << node->metaObject()->className();

		return node;
	}

	bool Graph::hasNode( const QString &name )const
	{
		auto it = m_nodes.find( name );
		if( it != m_nodes.end() )
			return true;
		return false;
	}

	GraphNode::Ptr Graph::getNode(const QString &name)
	{
		auto it = m_nodes.find( name );
		if( it != m_nodes.end() )
			return it->second;
		return GraphNode::Ptr();
	}

	void Graph::addNode( core::GraphNode::Ptr node )
	{
		qDebug() << "Graph: adding node " << node->getName() << "(" << node->metaObject()->className() << ")";
		//m_nodes.push_back( node );
		m_nodes[node->getName()] = node;
	}

	void Graph::addConnection(const QString &variableName, core::GraphNode::Ptr destNode, const QString &destSocketName )
	{
		GraphNodeSocket::Ptr srcSocket = getVariableSocket(variableName);
		GraphNodeSocket::Ptr destSocket = destNode->getSocket(destSocketName);

		if( srcSocket && destSocket )
		{
			addConnection( srcSocket, destSocket );
		}else
			qCritical() << "Graph::addConnection: one of destined sockets doesnt exist.";
	}

	void Graph::addConnection( core::GraphNode::Ptr srcNode, const QString &srcSocketName, core::GraphNode::Ptr destNode, const QString &destSocketName )
	{
		GraphNodeSocket::Ptr srcSocket = srcNode->getSocket(srcSocketName);
		GraphNodeSocket::Ptr destSocket = destNode->getSocket(destSocketName);

		if( srcSocket && destSocket )
		{
			addConnection( srcSocket, destSocket );
		}else
			qCritical() << "Graph::addConnection: one of destined sockets doesnt exist.";
	}

	void Graph::addConnection(core::GraphNodeSocket::Ptr src, core::GraphNodeSocket::Ptr dest)
	{
		qDebug() << "adding connection";

		// connect signal/slots for dirty propagation
		//bool result = connect( src.get(), SIGNAL(dirty()), dest.get(), SLOT(makeDirty()) );
		bool result = connect( src.get(), &core::GraphNodeSocket::dirty, dest.get(), &core::GraphNodeSocket::makeDirty );
		if( result == true )
			qCritical() << "connection successfull";
		else
			qCritical() << "connection failed";

		// set updateCallback on destNode such that it pulls information from source socket
		dest->m_update = std::bind(&Graph::updateSocket, this, std::placeholders::_1, src.get() );

		m_connections.push_back( GraphNodeSocketConnection(src, dest) );

	}

	// fetches content from another socket
	void Graph::updateSocket(GraphNodeSocket *dest, GraphNodeSocket *src )
	{
		dest->updateFrom(src);
	}

	void Graph::render( core::GraphNode::Ptr node, int startFrame, int endFrame )
	{
		instance()->setFrame(startFrame);
		// iterate all frames and evaluate node
		for( int i=startFrame;i<=endFrame; ++i )
		{
			node->update();
			instance()->stepFrame();
		}
	}

	void Graph::load( QJsonObject &o )
	{
		Data::load(o);

		// nodes --------------
		QJsonArray jsonNodes = o["nodes"].toArray();
		for( auto jsonNode : jsonNodes )
		{
			int id = (int)jsonNode.toDouble();
			GraphNode::Ptr node = std::dynamic_pointer_cast<GraphNode>( core::instance()->deserialize(id) );
			addNode( node );
		}

		// connections ---------
		QJsonArray jsonConnections = o["connections"].toArray();
		for( auto jsonConnection : jsonConnections )
		{
			QString type = jsonConnection.toObject()["type"].toString();
			GraphNodeSocket::Ptr srcSocket, destSocket;
			if( type == QString("out->in") )
			{
				int src_id = (int)jsonConnection.toObject()["src"].toDouble();
				int dest_id = (int)jsonConnection.toObject()["dest"].toDouble();
				srcSocket = std::dynamic_pointer_cast<GraphNodeSocket>( core::instance()->deserialize(src_id) );
				destSocket = std::dynamic_pointer_cast<GraphNodeSocket>( core::instance()->deserialize(dest_id) );
			}else
			if( type == QString("variable->in") )
			{
				int dest_id = (int)jsonConnection.toObject()["dest"].toDouble();
				srcSocket = getVariableSocket( jsonConnection.toObject()["src"].toString() );
				destSocket = std::dynamic_pointer_cast<GraphNodeSocket>( core::instance()->deserialize(dest_id) );
			}
			addConnection( srcSocket, destSocket );
		}
	}

	void Graph::store( QJsonObject &o, QJsonDocument &doc )
	{
		Data::store(o, doc);

		// nodes -------------
		QJsonArray jsonNodes = doc.array();
		for( auto node : m_nodes )
		{
			int id = instance()->serialize( node.second );
			jsonNodes.append(QJsonValue(id));
		}
		o.insert("nodes", jsonNodes);

		// connections ------------
		QJsonArray jsonConnections = doc.array();
		for( auto it = m_connections.begin(), end = m_connections.end(); it != end; ++it )
		{
			GraphNodeSocketConnection &c = *it;
			QJsonObject jsonConnection = doc.object();

			if( isVariableSocket( c.m_source ) )
			{
				jsonConnection.insert( "type", QJsonValue( QString("variable->in") ));
				jsonConnection.insert( "src", QJsonValue( c.m_source->getName() ) );
				jsonConnection.insert( "dest", QJsonValue( instance()->serialize(c.m_dest) ) );
			}else
			{
				jsonConnection.insert( "type", QJsonValue( QString("out->in") ));
				jsonConnection.insert( "src", QJsonValue( instance()->serialize(c.m_source) ) );
				jsonConnection.insert( "dest", QJsonValue( instance()->serialize(c.m_dest) ) );
			}
			jsonConnections.append( QJsonValue(jsonConnection) );
		}
		o.insert("connections", jsonConnections);
	}

	// prints graph to console (used for debugging)
	void Graph::print()const
	{
		qDebug() << "Graph ========================================";
		qDebug() << "Nodes (" << m_nodes.size() << "):";
		for( auto node : m_nodes )
			node.second->print();
	}

}
