#pragma once

#include <core/GraphNode.h>
#include "Operator.h"





class Solver : public core::GraphNode
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Solver> Ptr;

	Solver();

	Operator::Ptr                                     createOperator( const QString &type, const QString &description );
	void                                              addOperator( Operator::Ptr op, const QString &description );


	// GraphNode
	virtual void                                      update(core::GraphNodeSocket *output)override;

	// Data
	virtual void                                      store( QJsonObject &o, QJsonDocument &doc )override;
	virtual void                                      load( QJsonObject &o )override;

private:
	std::vector<std::pair<Operator::Ptr, QString>>    m_operators;
	int                                               m_frame;
};
