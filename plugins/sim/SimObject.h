#pragma once
#include <core/Data.h>


struct SimObject : public core::Data
{
	Q_OBJECT
public:
	typedef std::shared_ptr<SimObject> Ptr;

	SimObject();
};
