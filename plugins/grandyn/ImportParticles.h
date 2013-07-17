#pragma once
#include <core/GraphNode.h>


class ImportParticles : public core::GraphNode
{
	Q_OBJECT
public:
	typedef std::shared_ptr<ImportParticles> Ptr;

	ImportParticles();
	virtual void                  update( core::GraphNodeSocket *output )override;
};
