#pragma once
#include <memory>
#include <vector>

#include "glew/gl.h"

#include <plugins/primitives/Attribute.h>

namespace gl
{
	struct Attribute
	{
		typedef std::shared_ptr<Attribute> Ptr;

		Attribute();
		Attribute( ::Attribute::Ptr attr );
		virtual ~Attribute();

		// opengl interaction
		void                                     bindAsAttribute( int index );
		void                                     unbindAsAttribute( int index );
		virtual void                             bindAsUniform( int index );
		virtual void                             unbindAsUniform( int index );


		// wrapper functions
		::Attribute::Ptr                         getWrapped();
		void                                     clear();
		int                                      numElements()const;
		/*
		int                                      numComponents()const;
		ComponentType                            componentType()const;
		const void*                              rawPointer()const;
		void*                                    rawPointer();
		*/
		template<typename T> unsigned int        appendElement( const T &value ); // assumes sizeof(T) == m_elementSize
		template<typename T> T&                  get( unsigned int index ); // assumes sizeof(T) == m_elementSize
		template<typename T> void                set( unsigned int index, const T &value ); // assumes sizeof(T) == m_elementSize
		//void                                     resize(int numElements);



		// convinience creators -------------
		static Attribute::Ptr                    createM33f();
		static Attribute::Ptr                    createM44f();
		static Attribute::Ptr                    createV4f( int numElements = 0 );
		static Attribute::Ptr                    createV3f( int numElements = 0 );
		static Attribute::Ptr                    createV2f( int numElements = 0 );
		static Attribute::Ptr                    createFloat( int numElements = 0 );
		static Attribute::Ptr                    createInt();

/*
		enum ComponentType
		{
			SINT32 = 1,
			REAL32 = 2,
			REAL64 = 3
		};


		Attribute( ComponentType componentType = REAL32, char numComponents=3 );




		void                                     clear();
		int                                      numElements()const;
		int                                      numComponents()const;
		ComponentType                            componentType()const;
		const void*                              rawPointer()const;
		void*                                    rawPointer();
		template<typename T> unsigned int        appendElement( const T &value ); // assumes sizeof(T) == m_elementSize
		template<typename T> T&                  get( unsigned int index ); // assumes sizeof(T) == m_elementSize


		//
		// convinience creators
		//
		static Attribute::Ptr                    createMat33();
		static Attribute::Ptr                    createMat44();
		static Attribute::Ptr                    createVec4f( int numElements = 0 );
		static Attribute::Ptr                    createVec3f( int numElements = 0 );
		static Attribute::Ptr                    createVec2f( int numElements = 0 );
		static Attribute::Ptr                    createFloat();
		static Attribute::Ptr                    createInt();
		static int                               componentSize( ComponentType ct );
*/

	private:
/*
		std::vector<unsigned char>               m_data;
		ComponentType                            m_componentType;
		char                                     m_numComponents;
		char                                     m_elementSize; // size of element in byte
		int                                      m_numElements;
		bool                                     m_isDirty;
*/


		::Attribute::Ptr                         m_attr; // reference to the real attribute which holds the data
		unsigned int                             m_bufferId; // id of opengl buffer
		int                                      m_glComponentType; // GL_INT, GL_FLOAT, ...
		bool                                     m_isDirty; // indicates update on gpu requiredbool


	};



	template<typename T>
	unsigned int Attribute::appendElement( const T &value )
	{
		m_isDirty = true;
		return m_attr->appendElement<T>( value );
	}

	// assumes sizeof(T) == m_elementSize
	template<typename T>
	T& Attribute::get( unsigned int index )
	{
		return m_attr->get<T>(index);
	}

	// assumes sizeof(T) == m_elementSize
	template<typename T>
	void Attribute::set( unsigned int index, const T &value )
	{
		m_isDirty = true;
		m_attr->set<T>( index, value );
	}




/*
	template<typename T>
	unsigned int Attribute::appendElement( const T &value )
	{
		unsigned int pos = (unsigned int) m_data.size();
		m_data.resize( pos + sizeof(T) );
		*((T *)&m_data[pos]) = value;
		m_isDirty = true;
		return m_numElements++;
	}

	// assumes sizeof(T) == m_elementSize
	template<typename T>
	T& Attribute::get( unsigned int index )
	{
		T *data = (T*)&m_data[index * m_elementSize];
		return *data;
	}
*/
}
