#include "GraphNodeSocket.h"
#include <QDebug>

namespace core
{

	GraphNodeSocket::GraphNodeSocket() : Data(), m_name("unnamed"), m_state(DIRTY)
	{
	}

	GraphNodeSocket::GraphNodeSocket(const QString &name, Direction direction ) : Data(), m_name(name), m_state(DIRTY), m_direction(direction)
	{
	}

	const QString& GraphNodeSocket::getName()const
	{
		return m_name;
	}

	GraphNodeSocket::Direction GraphNodeSocket::getDirection()const
	{
		return m_direction;
	}

	void GraphNodeSocket::setData( Data::Ptr data )
	{
		//if( m_state == CLEAN )
		// m_state = DIRTY;
		//todo: emit isDirty;
		m_data = data;
	}


	Data::Ptr GraphNodeSocket::getData()
	{
		if( m_state == DIRTY )
			update();
		return m_data;
	}


	// makeclean
	void GraphNodeSocket::update()
	{
		m_state = UPDATING;
		if( m_update )
			m_update( this );
		m_state = CLEAN;
	}

	// updates this socket from remote socket
	void GraphNodeSocket::updateFrom( GraphNodeSocket *src )
	{
		// TODO:switch(socketType)
		// case Data:
		this->m_data = src->getData();
	}


	void GraphNodeSocket::store( QJsonObject &o, QJsonDocument &doc )
	{
		Data::store(o,doc);
		o.insert( "socketname", QJsonValue(getName()) );
		o.insert( "socketdirection", QJsonValue(int(getDirection())) );
	}


	void GraphNodeSocket::load( QJsonObject &o )
	{
		Data::load(o);
		m_name = o["socketname"].toString();
		m_direction = Direction(int(o["socketdirection"].toDouble()));
	}
}


