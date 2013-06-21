#include "VectorField.h"







VectorField::VectorField() : core::Data(), Field<math::V3f>()
{
}

// copy constructor
VectorField::VectorField( Ptr other ) : core::Data()
{
	resize(other->getResolution());
	setBound(other->bound());

	memcpy( &m_data[0], &other->m_data[0], sizeof(math::V3f)*m_resolution.x*m_resolution.y*m_resolution.z );
}



math::V3f VectorField::evaluateStaggered( const math::V3f &vsP )
{
	// TODO: change code such that data is stored at voxelfaces

	typedef float real_t;
	typedef math::Vec3<real_t> Vector;

	Vector vs = vsP;

	// voxels defined at cell centers
	vs.x -= (real_t)(0.5);
	vs.y -= (real_t)(0.5);
	vs.z -= (real_t)(0.5);

	real_t tx = vs.x - floor(vs.x);
	real_t ty = vs.y - floor(vs.y);
	real_t tz = vs.z - floor(vs.z);

	// lower left corner
	math::V3i c1;
	c1.x = (int)floor(vs.x);
	c1.y = (int)floor(vs.y);
	c1.z = (int)floor(vs.z);

	// upper right corner
	math::V3i c2 = c1+math::V3i(1);
	math::V3i res = getResolution();

	// clamp the indexing coordinates
	c1.x = std::max(0, std::min(c1.x, res.x-1));
	c2.x = std::max(0, std::min(c2.x, res.x-1));
	c1.y = std::max(0, std::min(c1.y, res.y-1));
	c2.y = std::max(0, std::min(c2.y, res.y-1));
	c1.z = std::max(0, std::min(c1.z, res.z-1));
	c2.z = std::max(0, std::min(c2.z, res.z-1));

	//lerp...
	return math::lerp( math::lerp( math::lerp( sample( c1.x, c1.y, c1.z ), sample( c2.x, c1.y, c1.z ), (real_t)tx ), math::lerp( sample( c1.x, c2.y, c1.z ), sample( c2.x, c2.y, c1.z ), (real_t)tx ), (real_t)ty ), math::lerp( math::lerp( sample( c1.x, c1.y, c2.z ), sample( c2.x, c1.y, c2.z ), (real_t)tx ), math::lerp( sample( c1.x, c2.y, c2.z ), sample( c2.x, c2.y, c2.z ), (real_t)tx ), (real_t)ty ), (real_t)tz );
}

// generates a scalarfield for given component
ScalarField::Ptr VectorField::getScalarField( int component )const
{
	ScalarField::Ptr sf = std::make_shared<ScalarField>();
	sf->resize(getResolution());
	sf->setBound(bound());

	// copy data for given component to new scalar field
	for( int k=0;k<m_resolution.z;++k )
		for( int j=0;j<m_resolution.y;++j )
			for( int i=0;i<m_resolution.x;++i )
				sf->lvalue(i, j, k) = sample( i, j, k )[component];

	return sf;
}
