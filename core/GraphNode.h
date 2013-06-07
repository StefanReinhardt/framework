#pragma once

#include "Data.h"


namespace core
{
	struct GraphNodeSocket
	{
		typedef std::shared_ptr<GraphNodeSocket> Ptr;
	};

	struct GraphNode : public Data
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<GraphNode> Ptr;

		GraphNode();
		virtual void                        update();








	private:
		std::vector<GraphNodeSocket> m_sockets;
	};
}
