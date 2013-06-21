#pragma once
#include <core/Data.h>
#include <QString>
#include <vector>


class SimObject : public core::Data
{
	Q_OBJECT
public:
	typedef std::shared_ptr<SimObject> Ptr;

	SimObject();


	void                                                    addSubData(const QString &name , core::Data::Ptr data);
	void                                                    setSubData(const QString &name , core::Data::Ptr data);
	template<typename T> std::shared_ptr<T>                 getSubData( const QString &name );
	bool                                                    hasSubData( const QString &name );


	void                                                    print()const; // used for debugging

	std::vector<QString>									getSubDataNames();

private:
	std::vector<std::pair<QString, Data::Ptr>>              m_subData;

};





template<typename T>
std::shared_ptr<T> SimObject::getSubData( const QString &name )
{
	for( auto it = m_subData.begin(), end = m_subData.end(); it != end; ++it )
		if( it->first == name )
			return std::dynamic_pointer_cast<T>(it->second);
	return std::shared_ptr<T>();
}
