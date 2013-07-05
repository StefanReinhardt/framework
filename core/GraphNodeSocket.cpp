#include "GraphNodeSocket.h"

#include "Core.h"

#include <QDebug>


namespace core
{

	GraphNodeSocket::GraphNodeSocket() : Data(), m_state(DIRTY), m_type(DATA)
	{
	}

	GraphNodeSocket::GraphNodeSocket(const QString &name, Direction direction ) : Data(), m_state(DIRTY), m_direction(direction), m_type(DATA)
	{
		setObjectName( name );
	}

	GraphNodeSocket::Direction GraphNodeSocket::getDirection()const
	{
		return m_direction;
	}


	const QVariant& GraphNodeSocket::getValue()
	{
		if( m_state == DIRTY )
			update();
		return m_value;
	}

	void GraphNodeSocket::setValue( const QVariant &value )
	{
		makeDirty();
		m_value = value;
		m_type = VALUE;
	}

	QString GraphNodeSocket::asString()
	{
		return getValue().toString();
	}


	int GraphNodeSocket::asInt()
	{
		return getValue().toInt();
	}

	float GraphNodeSocket::asFloat()
	{
		return getValue().toFloat();
	}


	void GraphNodeSocket::setString( const QString &value )
	{
		setValue<QString>(value);
	}

	void GraphNodeSocket::setInt( int value )
	{
		setValue<int>(value);
	}

	void GraphNodeSocket::setFloat( float value )
	{
		setValue<float>(value);
	}

	void GraphNodeSocket::setData( Data::Ptr data )
	{
		makeDirty();
		m_data = data;
	}


	Data::Ptr GraphNodeSocket::getData()
	{
		if(m_state == DIRTY)
			update();
		return m_data;
	}


	// makeclean
	void GraphNodeSocket::update()
	{
		qDebug() << "graphnodesocket makeClean " << objectName();
		m_state = UPDATING;
		if( m_update )
		{
			m_update( this );
		}
		m_state = CLEAN;
	}

	void GraphNodeSocket::makeDirty()
	{
		if( m_state == CLEAN )
		{
			qDebug() << "graphnodesocket dirty " << objectName();
			m_state = DIRTY;
			emit dirty();
		}
	}

	// updates this socket from remote socket
	void GraphNodeSocket::updateFrom( GraphNodeSocket *src )
	{
		qDebug() << "updating graphnodesocket " << objectName() << " from " << src->objectName();
		// TODO:switch(socketType)
		// case Data:
		switch( m_type )
		{
		case DATA:
			this->m_data = src->getData();break;
		case VALUE:
			this->m_value = src->getValue();break;
		}
	}


	void GraphNodeSocket::store( QJsonObject &o, QJsonDocument &doc )
	{
		Data::store(o,doc);
		o.insert( "socketname", QJsonValue(objectName()) );
		o.insert( "sockettype", QJsonValue(int(m_type)) );
		if( m_type == VALUE )
			o.insert( "socketvalue", instance()->serialize( m_value ) );
		o.insert( "socketdirection", QJsonValue(int(m_direction)) );
	}


	void GraphNodeSocket::load( QJsonObject &o )
	{
		Data::load(o);
		setObjectName( o["socketname"].toString() );
		m_type = Type(int(o["sockettype"].toDouble()));
		m_direction = Direction(int(o["socketdirection"].toDouble()));

		if( m_type == VALUE )
			instance()->deserialize( o["socketvalue"], m_value );

	}
}



