#pragma once

#include "GraphNode.h"
#include <map>


namespace core
{
	struct GraphNodeSocketConnection
	{
		GraphNodeSocketConnection()
		{
		}
		GraphNodeSocketConnection( GraphNodeSocket::Ptr src, GraphNodeSocket::Ptr dest ) : m_source(src), m_dest(dest)
		{
		}

		GraphNodeSocket::Ptr m_source;
		GraphNodeSocket::Ptr m_dest;
	};

	class Graph : public Data
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<Graph> Ptr;

		Graph();



		GraphNode::Ptr                             createNode( const QString &type, const QString &name = "" );
		bool                                       hasNode(const QString &name)const;
		GraphNode::Ptr                             getNode(const QString &name);
		void                                       addConnection(const QString &variableName, core::GraphNode::Ptr destNode, const QString &destSocketName );
		void                                       addConnection(core::GraphNode::Ptr srcNode, const QString &srcSocketName, core::GraphNode::Ptr destNode, const QString &destSocketName );
		void                                       addConnection(core::GraphNodeSocket::Ptr src, core::GraphNodeSocket::Ptr dest);
		void                                       render( GraphNode::Ptr node, int startFrame, int endFrame );
		void                                       print()const; // prints graph to console (used for debugging)

		virtual void                               store( QJsonObject &o, QJsonDocument &doc )override;
		virtual void                               load( QJsonObject &o )override;



	private:
		void                                       addNode( core::GraphNode::Ptr node );
		void                                       updateSocket( GraphNodeSocket* dest, GraphNodeSocket* src ); // fetches content from another socket

		std::vector<GraphNodeSocketConnection>     m_connections;
		std::map<QString, GraphNode::Ptr>          m_nodes;
		//std::vector<GraphNode::Ptr>                m_nodes;
	};
}
