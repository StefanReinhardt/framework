#pragma once

#include "GraphNode.h"


namespace core
{
	struct GraphNodeSocketConnection
	{
		GraphNodeSocket::Ptr m_source;
		GraphNodeSocket::Ptr m_dest;
	};

	struct Graph : public Data
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<Graph> Ptr;

		Graph();




		void                                       addNode( core::GraphNode::Ptr node );
		void                                       print()const; // prints graph to console (used for debugging)

		virtual void                               store( QJsonObject &o, QJsonDocument &doc )override;
		virtual void                               load( QJsonObject &o )override;



	private:
		std::vector<GraphNodeSocketConnection>     m_connections;
		std::vector<GraphNode::Ptr>                m_nodes;
	};
}
