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



	math::V3f                              evaluate( const math::V3f &wsP )const;

	void                                   resize( int x, int y, int z );
	void                                   resize( math::V3i resolution );

	void                                   setLocalToWorld( const math::M44f &localToWorld );
	void                                   setBound( const math::Box3f &bound );

	void                                   fill( math::V3f value ); // fills all voxels with the same value

	ScalarField::Ptr                       getScalarField( int component )const; // gives scalarfield for given component
private:
	ScalarField::Ptr                       m_fields[3];
	Sampling                               m_sampling;
};











