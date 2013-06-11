#pragma once

#include "Data.h"

#include <QVariant>

#include <functional>

namespace core
{
	struct GraphNodeSocket : public Data
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<GraphNodeSocket> Ptr;
		typedef std::function<void ( GraphNodeSocket* )> UpdateCallback;

		enum Type
		{
			DATA,
			VALUE
		};
		enum State
		{
			DIRTY,
			CLEAN,
			UPDATING
		};
		enum Direction
		{
			INPUT,
			OUTPUT
		};


		GraphNodeSocket();
		GraphNodeSocket( const QString &name, Direction direction );

		const QString&                                   getName()const;
		Direction                                        getDirection()const;

		void                                             setData( Data::Ptr data );
		Data::Ptr                                        getData();
		template<typename T> std::shared_ptr<T>          getData();

		const QVariant&                                  getValue();
		template<typename T> void                        setValue( const T &value);
		QString                                          asString();
		int                                              asInt();
		void                                             setString( const QString &value );
		void                                             setInt( int value );


		void                                             update(); // makeclean

		virtual void                                     store( QJsonObject &o, QJsonDocument &doc )override;
		virtual void                                     load( QJsonObject &o )override;

	//private:
		friend struct GraphNode; // required for GraphNode deserialization
		friend struct Graph; // required for setting update callback when making connections

		void                                             updateFrom( GraphNodeSocket *src ); // updates this socket from remote socket

		QString                                          m_name;
		Type                                             m_type;
		State                                            m_state;
		Direction                                        m_direction;
		UpdateCallback                                   m_update;

		Data::Ptr                                        m_data;
		QVariant                                         m_value;
	};





	template<typename T>
	std::shared_ptr<T> GraphNodeSocket::getData()
	{
		return std::dynamic_pointer_cast<T>(getData());
	}

	template<typename T>
	void GraphNodeSocket::setValue( const T &value)
	{
		m_value.setValue<T>( value );
		m_type = VALUE;
	}
}
