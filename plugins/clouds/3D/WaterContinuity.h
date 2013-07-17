#pragma once

#include <plugins/sim/Operator.h>

class WaterContinuity : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<WaterContinuity> Ptr;
	WaterContinuity();

	virtual void                         apply( SimObject::Ptr so, float dt )override;
};
