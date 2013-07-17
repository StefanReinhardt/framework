#pragma once

#include <plugins/sim/Operator.h>

class Buoyancy : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Buoyancy> Ptr;
	Buoyancy();

	virtual void                        apply( SimObject::Ptr so, float dt )override;

	void                                setStrenght(float buoyancy);

private:
	float                                m_dt;
	float                                m_buoyancy;

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );

};
