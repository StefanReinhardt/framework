#pragma once

#include <plugins/sim/Operator.h>
#include <core/Core.h>
#include <plugins/primitives/ScalarField.h>

class CloudControl2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<CloudControl2D> Ptr;
	CloudControl2D();
	CloudControl2D(SimObject::Ptr so);

	virtual void                         apply( SimObject::Ptr so, float dt )override;
	core::Timer                          timer;

	// overloads from Data
	//virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	//virtual void                       load( QJsonObject &o );



};
