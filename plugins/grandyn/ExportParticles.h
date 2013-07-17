#pragma once
#include <core/GraphNode.h>


struct ExportParticles : public core::GraphNode
{
	Q_OBJECT
public:
	typedef std::shared_ptr<ExportParticles> Ptr;

	ExportParticles();
	virtual void update(core::GraphNodeSocket *output)override;
};
