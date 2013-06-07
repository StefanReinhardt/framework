//
//
// Why dont we use QMetaObject straight away? Because we want to use std::make_shared
// which creates smartptr and class instance next to each other in memory
// which is desired for performance reasons (caching)
//



#pragma once

#include "Data.h"
#include <memory>
#include <functional>
#include <QString>




namespace core
{
	typedef std::function<Data::Ptr(void)> DataConstructor;
	struct DataFactory
	{
		typedef std::shared_ptr<DataFactory> Ptr;
		DataFactory( const QString &dataType, const QString &description, DataConstructor constructor );
		virtual ~DataFactory();

		Data::Ptr                        createInstance()const;
		QString                          typeString()const;

	private:
		const QString                    m_dataType;
		const QString                    m_description;
		const DataConstructor            m_constructor;
	};


	template<typename T>
	struct DataFactoryT : public DataFactory
	{
		DataFactoryT( const QString &dataType, const QString &description );
		static DataFactory::Ptr create( const QString &dataType, const QString &description = "" );
		static Data::Ptr constructor();
	};

	template<typename T>
	DataFactoryT<T>::DataFactoryT( const QString &dataType, const QString &description  ) :
		DataFactory( dataType, description, constructor )
	{
	}

	template<typename T>
	DataFactory::Ptr DataFactoryT<T>::create( const QString &dataType, const QString &description  )
	{
		return std::make_shared< DataFactoryT<T> >( dataType, description );
	}

	template<typename T>
	Data::Ptr DataFactoryT<T>::constructor()
	{
		Data::Ptr data = std::make_shared<T>();
		return data;
	}

}
