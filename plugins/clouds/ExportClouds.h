#pragma once
#include <core/GraphNode.h>


struct ExportClouds : public core::GraphNode
{
	Q_OBJECT
public:
	typedef std::shared_ptr<ExportClouds> Ptr;

	ExportClouds();
	virtual void                  update(core::GraphNodeSocket *output)override;
};
