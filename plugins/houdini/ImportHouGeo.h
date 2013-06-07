#pragma once
#include <core/GraphNode.h>


struct ImportHouGeo : public core::GraphNode
{
	Q_OBJECT
public:
	typedef std::shared_ptr<ImportHouGeo> Ptr;

	ImportHouGeo();
	virtual void                  update()override;
};
