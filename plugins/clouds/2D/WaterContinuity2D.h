
#pragma once

#include <plugins/sim/Operator.h>

class WaterContinuity2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<WaterContinuity2D> Ptr;
	WaterContinuity2D();

	virtual void apply( SimObject::Ptr so )override;

	// overloads from Data
	//virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	//virtual void                       load( QJsonObject &o );
private:
	float dt;			// Timestep
};
