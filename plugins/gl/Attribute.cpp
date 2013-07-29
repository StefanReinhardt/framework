#include "Attribute.h"

#include "core/types.h"


namespace gl
{

	Attribute::Attribute() : m_isDirty(true)
	{
	}


	Attribute::Attribute( ::Attribute::Ptr attr ) : m_attr(attr), m_isDirty(true)
	{
		switch(attr->componentType())
		{
		case ::Attribute::SINT32:
			{
				m_glComponentType = GL_INT;
			}break;
		default:
		case ::Attribute::REAL32:
			{
				m_glComponentType = GL_FLOAT;
			}break;
			/*
		case SAMPLER:
			{
				m_glComponentType = SAMPLER;
				m_componentSize=sizeof(int);
			}break;
			*/
		};

		// prepare VBO
		glGenBuffers(1, &m_bufferId);
	}

	Attribute::~Attribute()
	{
		glDeleteBuffers(1, &m_bufferId);
	}

	int Attribute::numElements()const
	{
		return m_attr->numElements();
	}

	void Attribute::bindAsAttribute( int index )
	{
		int numElements = m_attr->numElements();
		if(m_isDirty && numElements)
		{
			// activate and specify pointer to vertex array
			// should be done only when attribute has been updated
			glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
			glBufferData(GL_ARRAY_BUFFER, m_attr->elementSize()*numElements, m_attr->rawPointer(), GL_STATIC_DRAW);
			m_isDirty = false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
		glEnableVertexAttribArray(index);
		if( m_glComponentType == GL_FLOAT )
			glVertexAttribPointer(index, m_attr->numComponents(), m_glComponentType, false, 0, 0);
		else
			glVertexAttribIPointer(index, m_attr->numComponents(), m_glComponentType, 0, 0);

	}
	void Attribute::unbindAsAttribute( int index )
	{
		// deactivate vertex arrays after drawing
		glDisableVertexAttribArray(index);
	}

	void Attribute::bindAsUniform( int index )
	{
		switch( m_attr->numComponents() )
		{
		case 1:
			if( m_glComponentType == GL_FLOAT )
			{
				//printf("setting uniform tmp: %f at uniform location %i\n", *((float *)getRawPointer()), index );
				glUniform1fv( index, m_attr->numElements(), (float *)m_attr->rawPointer());
			}
			else if( m_glComponentType == GL_INT )
			{
				glUniform1iv( index, m_attr->numElements(), (int*)m_attr->rawPointer());
			}
			/*else if( m_internalComponentType == SAMPLER )
			{
				int *texIds = (int*)getRawPointer();
				int textureUnits[32];
				int num = numElements();
				for( int i=0;i<num;++i )
				{
					glActiveTexture(GL_TEXTURE0+g_nextTextureUnit);
					glBindTexture(m_textureTarget, *texIds);
					textureUnits[i] = g_nextTextureUnit;
					++texIds;
					++g_nextTextureUnit;
				}
				glUniform1iv( index, num, textureUnits);
			}*/
			break;
		case 2:
			if( m_glComponentType == GL_FLOAT )
				glUniform2fv( index, m_attr->numElements(), (float *)m_attr->rawPointer());
			break;
		case 3:
			if( m_glComponentType == GL_FLOAT )
				glUniform3fv( index, m_attr->numElements(), (float *)m_attr->rawPointer());
			break;
		case 4:
			if( m_glComponentType == GL_FLOAT )
				glUniform4fv( index, m_attr->numElements(), (float *)m_attr->rawPointer());
			break;
		case 9:
			glUniformMatrix3fv( index, m_attr->numElements(), false, (float *)m_attr->rawPointer() );
			break;
		case 16:
			glUniformMatrix4fv( index, m_attr->numElements(), false, (float *)m_attr->rawPointer() );
			break;
		};
	}
	void Attribute::unbindAsUniform( int index )
	{
		// ?
		// for better texture unit management tell the currently used texture unit is not used anymore
	}

/*
	Attribute::Attribute( ComponentType componentType, char numComponents ) :
		m_componentType(componentType),
		m_numComponents(numComponents),
		m_numElements(0),
		m_isDirty(true),
		m_elementSize( componentSize(componentType)*numComponents )
	{

	}



	int Attribute::numComponents()const
	{
		return m_numComponents;
	}

	Attribute::ComponentType Attribute::componentType()const
	{
		return m_componentType;
	}

	const void *Attribute::rawPointer()const
	{
		return &m_data[0];
	}

	void *Attribute::rawPointer()
	{
		return &m_data[0];
	}

	void Attribute::clear()
	{
		m_data.clear();
		m_numElements = 0;
		m_isDirty = true;
	}











*/

	//
	// convinience creators ======================
	//
	Attribute::Ptr Attribute::createM33f()
	{
		return std::make_shared<Attribute>( ::Attribute::createM33f() );
	}

	Attribute::Ptr Attribute::createM44f()
	{
		return std::make_shared<Attribute>( ::Attribute::createM44f() );
	}

	Attribute::Ptr Attribute::createV4f( int numElements )
	{
		return std::make_shared<Attribute>( ::Attribute::createV4f(numElements) );
	}

	Attribute::Ptr Attribute::createV3f( int numElements )
	{
		return std::make_shared<Attribute>( ::Attribute::createV3f(numElements) );
	}

	Attribute::Ptr Attribute::createV2f( int numElements )
	{
		return std::make_shared<Attribute>( ::Attribute::createV2f(numElements) );
	}

	Attribute::Ptr Attribute::createFloat( int numElements )
	{
		return std::make_shared<Attribute>( ::Attribute::createFloat(numElements) );
	}

	Attribute::Ptr Attribute::createInt()
	{
		return std::make_shared<Attribute>( ::Attribute::createInt() );
	}

/*
	int Attribute::componentSize( Attribute::ComponentType ct )
	{
		switch(ct)
		{
		case SINT32:return sizeof(sint32);
		case REAL32:return sizeof(real32);
		case REAL64:return sizeof(real64);
		default:
			throw std::runtime_error( "unknown component type" );
		};
		return -1;
	}
*/
}
