#pragma once

#include <core/Data.h>

#include "Field.h"







class ScalarField : public core::Data, public Field<float>
{
public:
	typedef std::shared_ptr<ScalarField> Ptr;





};











