#pragma once

#include <core/Data.h>

#include "ScalarField.h"







class VectorField : public core::Data
{
	Q_OBJECT
public:
	typedef std::shared_ptr<VectorField> Ptr;

	enum Sampling
	{
		CENTER,
		FACE
	};

	VectorField( Sampling sampling = CENTER );
	VectorField( Ptr other );



	math::V3f								evaluate( const math::V3f &wsP )const;

	void									resize( int x, int y, int z );
	void									resize( math::V3i resolution );

	void									setBound( const math::Box3f &bound ); // TODO: update bound seperately (keep voxels uniform size)

	void									fill( math::V3f value ); // fills all voxels with the same value

	ScalarField::Ptr						getScalarField( int component )const; // gives scalarfield for given component
	float									getLength(int i, int j, int k); // gives Magnitude of vector at given grid position
private:
	ScalarField::Ptr						m_fields[3];
	Sampling								m_sampling;
};











