#pragma once

#include <plugins/sim/Operator.h>
#include <core/Core.h>

class Buoyancy2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Buoyancy2D> Ptr;
	Buoyancy2D();


	void                                 setStrenght(float buoyancy);
	void                                 setGravity(float gravity);

private:
	virtual void                         applyImpl( SimObject::Ptr so, float dt )override;

	// overloads from Data
	virtual void                         store( QJsonObject &o, QJsonDocument &doc );
	virtual void                         load( QJsonObject &o );

	core::Timer                          timer;
	float                                m_dt;
	float                                m_buoyancy;
	float                                m_gravity;

};
