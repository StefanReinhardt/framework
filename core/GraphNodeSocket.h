#pragma once

#include "Data.h"

#include <math/Math.h>

#include <QVariant>

#include <functional>

namespace core
{
	class GraphNodeSocket : public Data
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

		Direction                                        getDirection()const;

		void                                             setData( Data::Ptr data );
		Data::Ptr                                        getData();
		template<typename T> std::shared_ptr<T>          getData();
		template<typename T> std::shared_ptr<T>          getDataWithoutUpdate();

		const QVariant&                                  getValue();
		void                                             setValue( const QVariant &value );
		template<typename T> void                        setValue( const T &value);
		QString                                          asString();
		int                                              asInt();
		float                                            asFloat();
		math::V3f                                        asV3f();
		math::V3i                                        asV3i();
		void                                             setString( const QString &value );
		void                                             setInt( int value );
		void                                             setFloat( float value );
		void                                             setV3f( math::V3f value );
		void                                             setV3i( math::V3i value );


		bool                                             isDirty()const;
		void                                             update(); // makeclean

		virtual void                                     store( QJsonObject &o, QJsonDocument &doc )override;
		virtual void                                     load( QJsonObject &o )override;

	signals:
		void                                             dirty();
	public slots:
		void                                             makeDirty();
	private:
		friend class GraphNode; // required for GraphNode deserialization
		friend class Graph; // required for setting update callback when making connections

		void                                             updateFrom( GraphNodeSocket *src ); // updates this socket from remote socket

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
	std::shared_ptr<T> GraphNodeSocket::getDataWithoutUpdate()
	{
		return std::dynamic_pointer_cast<T>(m_data);
	}

	template<typename T>
	void GraphNodeSocket::setValue( const T &value )
	{
		QVariant variant;
		variant.setValue<T>( value );
		setValue(variant);
	}
}
