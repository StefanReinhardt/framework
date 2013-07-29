#pragma once

#include <plugins/sim/Operator.h>

class Gravity2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Gravity2D> Ptr;

	Gravity2D();
	virtual void                         applyImpl( SimObject::Ptr so, float dt )override;
};

