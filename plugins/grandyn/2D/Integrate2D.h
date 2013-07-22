#pragma once

#include <plugins/sim/Operator.h>


class Integrate2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Integrate2D> Ptr;
	Integrate2D();

	virtual void                        apply( SimObject::Ptr so, float dt )override;
};

