#pragma once

#include <plugins/sim/Operator.h>

class Advect2d : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Advect2d> Ptr;

	Advect2d();

	virtual void apply( SimObject::Ptr so )override;

private:
	//QString m_name;
};
