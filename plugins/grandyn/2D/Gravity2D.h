#pragma once

#include <plugins/sim/Operator.h>

class Gravity2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Gravity2D> Ptr;
	Gravity2D();

	virtual void apply( SimObject::Ptr so )override;

	// overloads from Data
	//virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	//virtual void                       load( QJsonObject &o );
private:
};

