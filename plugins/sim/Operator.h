#pragma once

#include <core/Data.h>
#include "SimObject.h"






class Operator : public core::Data
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Operator> Ptr;

	Operator();

	virtual void apply( SimObject::Ptr so );
};
