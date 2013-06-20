#pragma once

#include <core/Data.h>

#include "Field.h"







class ScalarField : public core::Data, public Field<float>
{
	Q_OBJECT
public:
	typedef std::shared_ptr<ScalarField> Ptr;

	ScalarField();
	ScalarField( Ptr other); // copy constructor





};











