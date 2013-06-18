#pragma once

#include "GraphNodeSocket.h"
#include <map>





namespace core
{

	class GraphNode : public Data
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<GraphNode> Ptr;

		GraphNode();

		QString                                          getName()const;
		GraphNodeSocket::Ptr                             addInputSocket( const QString &name );
		GraphNodeSocket::Ptr                             addOutputSocket( const QString &name );
		GraphNodeSocket::Ptr                             getSocket( const QString name );
		bool                                             hasSocket( const QString &name )const;

		virtual void                                     update( GraphNodeSocket *output = 0 );




		virtual void                                     store( QJsonObject &o, QJsonDocument &doc )override;
		virtual void                                     load( QJsonObject &o )override;
		void                                             print()const; // prints node to console (used for debugging)



	public slots:
		void                                             makeDirty(); // makes all output slots dirty

	private:
		friend class Graph;

		void                                             addSocket( GraphNodeSocket::Ptr socket );
		void                                             updateAll( GraphNodeSocket* ); // adapter for GraphNodeSocket:: m_update functional (will ignore given ptr and just call update)


		QString                                          m_name; // name which is unique within scope of graph
		std::map<QString, GraphNodeSocket::Ptr>          m_sockets; // for fast name based lookup
	};
}
