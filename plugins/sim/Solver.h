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
	Operator::Ptr                                     getOperator(const QString &description);
	template <typename T> std::shared_ptr<T>          getOperator(const QString &description);

	void                                              setTimeStrech(float timeStretch);


	// GraphNode
	virtual void                                      update(core::GraphNodeSocket *output)override;

	// Data
	virtual void                                      store( QJsonObject &o, QJsonDocument &doc )override;
	virtual void                                      load( QJsonObject &o )override;

private:
	std::vector<std::pair<Operator::Ptr, QString>>    m_operators;
	int                                               m_frame;
	float                                             m_timeStretch;
};


template <typename T>
std::shared_ptr<T> Solver::getOperator(const QString &description)
{
	return std::dynamic_pointer_cast<T>( getOperator(description) );
}
