
#pragma once

#include <plugins/sim/Operator.h>

class WaterContinuity2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<WaterContinuity2D> Ptr;
	WaterContinuity2D();

	virtual void                         apply( SimObject::Ptr so, float dt )override;
};
