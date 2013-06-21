#pragma once

#include <core/Data.h>

#include "ScalarField.h"







class VectorField : public core::Data, public Field<math::V3f>
{
	Q_OBJECT
public:
	typedef std::shared_ptr<VectorField> Ptr;

	VectorField();
	VectorField( Ptr other); // copy constructor


	math::V3f                              evaluateStaggered( const math::V3f &vsP );

	ScalarField::Ptr                       getScalarField( int component )const; // generates a scalarfield for given component



};











