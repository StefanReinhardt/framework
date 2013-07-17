#pragma once

#include <core/Data.h>
#include <vector>

class Attribute : public core::Data
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Attribute> Ptr;

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
	static Attribute::Ptr                    createM33f();
	static Attribute::Ptr                    createM44f();
	static Attribute::Ptr                    createV4f( int numElements = 0 );
	static Attribute::Ptr                    createV3f( int numElements = 0 );
	static Attribute::Ptr                    createV2f( int numElements = 0 );
	static Attribute::Ptr                    createFloat();
	static Attribute::Ptr                    createInt();
	static int                               componentSize( ComponentType ct );


private:
	std::vector<unsigned char>               m_data;
	ComponentType                            m_componentType;
	char                                     m_numComponents;
	char                                     m_elementSize; // size of element in byte
	int                                      m_numElements;
	bool                                     m_isDirty;





};







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
