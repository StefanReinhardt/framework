#pragma once

#include <plugins/sim/Operator.h>
#include <core/Core.h>

class Balloon : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Balloon>  Ptr;
	Balloon();

	virtual void                       applyImpl( SimObject::Ptr so, float dt )override;

	void                              setPosition(float x, float y);
	void                              setReleaseFrame(int release);


private:
	float                              m_posy, m_posx;
	int                                m_release;

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );

};
