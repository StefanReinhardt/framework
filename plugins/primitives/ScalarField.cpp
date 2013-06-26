#include "ScalarField.h"







ScalarField::ScalarField() : core::Data(), Field<float>()
{
}

// copy constructor
ScalarField::ScalarField( Ptr other ) : core::Data()
{
	resize(other->getResolution());
	setBound(other->bound());
	m_sampleLocation = other->m_sampleLocation;

	memcpy( &m_data[0], &other->m_data[0], sizeof(float)*m_resolution.x*m_resolution.y*m_resolution.z );
}
