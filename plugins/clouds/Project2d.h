#pragma once

#include <plugins/sim/Operator.h>

class Project2d : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Project2d> Ptr;
	Project2d();

	virtual void apply( SimObject::Ptr so )override;

private:
	//QString m_name;
	float dt;			// Timestep
};
