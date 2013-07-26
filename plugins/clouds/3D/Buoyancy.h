#pragma once

#include <plugins/sim/Operator.h>

class Buoyancy : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Buoyancy> Ptr;
	Buoyancy();


	void                                setStrenght(float buoyancy);

private:

	virtual void                        applyImpl( SimObject::Ptr so, float dt )override;
	// overloads from Data
	virtual void                        store( QJsonObject &o, QJsonDocument &doc );
	virtual void                        load( QJsonObject &o );


	float                                m_dt;
	float                                m_buoyancy;

};
