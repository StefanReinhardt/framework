#pragma once

#include <plugins/sim/Operator.h>

class WaterContinuity : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<WaterContinuity> Ptr;
	WaterContinuity();

	virtual void apply( SimObject::Ptr so )override;

	// overloads from Data
	//virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	//virtual void                       load( QJsonObject &o );
};
