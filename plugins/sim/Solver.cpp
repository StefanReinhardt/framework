#include "Solver.h"


#include <plugins/sim/SimObject.h>

#include <core/Core.h>






Solver::Solver() : core::GraphNode()
{
	addInputSocket( "input" );
	addInputSocket( "frame" );
	addOutputSocket( "output" );

	getSocket("frame")->setInt(1);
	m_frame = 1;
	m_timeStretch = 1.0f;			// multiplier to speed up or slow down simulation
}



void Solver::update(core::GraphNodeSocket *output)
{
	SimObject::Ptr so = getSocket("input")->getData<SimObject>();
	int frame = getSocket( "frame" )->asInt();
	float fps = core::getVariable("$FPS").toDouble();
	float dt = m_timeStretch * (1.0/fps);
	qDebug() << "Solver: update frame: " << frame;
	// Reset if frame == 1
	if (frame==1)
		m_frame = 1;

	while( m_frame < frame )
	{
		// apply operators
		for( auto it = m_operators.begin(), end=m_operators.end();it != end;++it )
		{
			qDebug() << "Solver: running op " << (*it).second;
			it->first->apply( so, dt );
		}

		// proceed
		m_frame++;
		so->setTime( m_frame/fps );
	}

	getSocket( "output" )->setData(so);
}

void Solver::addOperator( Operator::Ptr op, const QString &description )
{
	m_operators.push_back( std::make_pair(op, description) );
}

Operator::Ptr Solver::createOperator( const QString &type, const QString &description )
{
	Operator::Ptr op = core::create<Operator>(type);
	if( op )
	{
		qDebug() << "Solver: creating operator " << op->metaObject()->className();
		addOperator( op, description );
	}else
		qCritical() << "Solver: unable to create operator " << op->metaObject()->className();

	return op;
}


Operator::Ptr Solver::getOperator(const QString &description)
{
	for( auto it = m_operators.begin(),end = m_operators.end(); it != end; ++it )
	{
		if( description == it->second )
			return it->first;
	}
	return Operator::Ptr();
}

void Solver::setTimeStrech(float timeStretch)
{
	m_timeStretch = timeStretch;
}

void Solver::store( QJsonObject &o, QJsonDocument &doc )
{
	GraphNode::store(o, doc);

	// operators ---
	QJsonArray operators = doc.array();
	for( auto it = m_operators.begin(),end = m_operators.end(); it != end; ++it )
	{
		QJsonObject obj = doc.object();
		obj.insert( "data", core::instance()->serialize( it->first ) );
		obj.insert( "description", QJsonValue(it->second));
		operators.append( obj );
	}
	o.insert( "operators", operators );
	o.insert( "timeStretch", m_timeStretch );
}


void Solver::load( QJsonObject &o )
{
	GraphNode::load(o);

	m_timeStretch =                  o["timeStretch"].toDouble();

	// operators ---
	QJsonArray operators =           o["operators"].toArray();
	for( auto it = operators.begin(), end = operators.end(); it != end; ++it )
	{
		QJsonObject obj = (*it).toObject();
		addOperator( std::dynamic_pointer_cast<Operator>( core::instance()->deserialize(obj["data"]) ), obj["description"].toString() );
	}
}
