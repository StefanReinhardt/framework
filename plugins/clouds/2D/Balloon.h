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



private:
	math::V3f                          m_vsP; // balloon position in voxelspace
	int                                m_release;

	// overloads from Data
	//virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	//virtual void                       load( QJsonObject &o );

};
