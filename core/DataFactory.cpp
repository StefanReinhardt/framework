#include "DataFactory.h"




namespace core
{
	DataFactory::DataFactory(const QString &dataType, const QString &description, DataConstructor constructor ) :
		m_dataType(dataType),
		m_description(description),
		m_constructor(constructor)
	{
	}

	DataFactory::~DataFactory()
	{
	}

	QString DataFactory::typeString()const
	{
		return m_dataType;
	}

	Data::Ptr DataFactory::createInstance()const
	{
		Data::Ptr data = m_constructor();
		return data;
	}
}

