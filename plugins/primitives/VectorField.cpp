#include "VectorField.h"






VectorField::VectorField( Sampling sampling ) : core::Data(), m_sampling(sampling)
{
	m_fields[0] = std::make_shared<ScalarField>();
	m_fields[1] = std::make_shared<ScalarField>();
	m_fields[2] = std::make_shared<ScalarField>();

	if( m_sampling == FACE )
	{
		// adjust sample location for each component
		for( int i=0;i<3;++i )
		{
			math::V3f sampleLocation( 0.5f );
			sampleLocation[i] = 0.0f;
			m_fields[i]->m_sampleLocation = sampleLocation;
		}
	}
}

VectorField::VectorField( VectorField::Ptr other ) : core::Data()
{
	m_sampling = other->m_sampling;

	for( int i=0;i<3;++i )
		m_fields[i] = std::make_shared<ScalarField>(other->getScalarField(i));
}



math::V3f VectorField::evaluate( const math::V3f &wsP )const
{
	math::V3f result;

	for( int i=0;i<3;++i )
		result[i] = m_fields[i]->evaluate( m_fields[i]->worldToVoxel(wsP) );

	return result;
}

// generates a scalarfield for given component
ScalarField::Ptr VectorField::getScalarField( int component )const
{
	return m_fields[component];
}


float VectorField::getLength(int i, int j, int k)
{
	if(m_sampling==CENTER)
	{
		return sqrt(m_fields[0]->lvalue(i,j,k)*m_fields[0]->lvalue(i,j,k)
				+	m_fields[1]->lvalue(i,j,k)*m_fields[1]->lvalue(i,j,k)
				+	m_fields[2]->lvalue(i,j,k)*m_fields[2]->lvalue(i,j,k));
	}
	else if(m_sampling==FACE)
	{
		return sqrt(pow((m_fields[0]->lvalue(i,j,k)+m_fields[0]->lvalue(i+1,j,k))/2, 2 )
				+	pow((m_fields[1]->lvalue(i,j,k)+m_fields[1]->lvalue(i,j+1,k))/2, 2 )
				+	pow((m_fields[2]->lvalue(i,j,k)+m_fields[2]->lvalue(i,j,k+1))/2, 2 ));
	}
	else
		return 0;
}

void VectorField::resize( int x, int y, int z )
{
	resize(math::V3i(x, y, z));
}


void VectorField::resize( math::V3i resolution )
{
	switch(m_sampling)
	{
	case CENTER:
		{
			// if samples are stored at cell centers all components have same resolution
			for( int i=0;i<3;++i )
				m_fields[i]->resize( resolution );
		}break;
	case FACE:
		{
			// for staggered grids fields have a different resolution in one component
			for( int i=0;i<3;++i )
			{
				math::V3i res = resolution;
				res[i] += 1;
				m_fields[i]->resize( res );
			}
		}break;
	};

}


void VectorField::setBound( const math::Box3f &bound )
{

	switch(m_sampling)
	{
	case CENTER:
		{
			for( int i=0;i<3;++i )
				m_fields[i]->setBound(bound);
		}break;
	case FACE:
		{
			// for staggered grids fields have a different resolution in one component
			// we take this into account when computing bounding boxes in order to keep uniform voxels uniform

			for( int i=0;i<3;++i )
			{
				// get minimum bound by substracing half a voxel on each side
				math::V3f vs_h = m_fields[i]->getVoxelSize()*0.5f;

				math::Box3f b( bound.minPoint + vs_h, bound.maxPoint - vs_h );

				// only current component remains original size since there is a voxel more
				b.minPoint[i] = bound.minPoint[i];
				b.maxPoint[i] = bound.maxPoint[i];

				m_fields[i]->setBound( b );
			}
		}break;
	};
}


// fills all voxels with the same value
void VectorField::fill( math::V3f value )
{
	for( int i=0;i<3;++i )
		m_fields[i]->fill(value[i]);
}
