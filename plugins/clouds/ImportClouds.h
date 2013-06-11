#pragma once
#include <core/GraphNode.h>


struct ImportClouds : public core::GraphNode
{
	Q_OBJECT
public:
	typedef std::shared_ptr<ImportClouds> Ptr;

	ImportClouds();
	virtual void                  update( core::GraphNodeSocket *output )override;
};
