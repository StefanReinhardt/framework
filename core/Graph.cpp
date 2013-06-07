#include "Graph.h"

#include "Core.h"


namespace core
{

	Graph::Graph() : Data()
	{
	}


	void Graph::addNode( core::GraphNode::Ptr node )
	{
		qDebug() << "Graph: adding node " << node->metaObject()->className();
		m_nodes.push_back( node );
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

	}

	void Graph::store( QJsonObject &o, QJsonDocument &doc )
	{
		Data::store(o, doc);

		// nodes -------------
		QJsonArray jsonNodes = doc.array();

		for( auto node : m_nodes )
		{
			int id = instance()->serialize( node );
			jsonNodes.append(QJsonValue(id));
		}

		o.insert("nodes", jsonNodes);

		// connections ------------
	}

	// prints graph to console (used for debugging)
	void Graph::print()const
	{
		qDebug() << "Graph ========================================";
		qDebug() << "Nodes (" << m_nodes.size() << "):";
		for( auto node : m_nodes )
			qDebug() << "\t" << node->metaObject()->className();
	}

}
