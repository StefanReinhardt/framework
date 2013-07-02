#pragma once

#include <plugins/sim/Operator.h>

class AddHeatSource : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<AddHeatSource> Ptr;
	AddHeatSource();

	virtual void apply( SimObject::Ptr so )override;

	// overloads from Data
	//virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	//virtual void                       load( QJsonObject &o );
private:
	//QString			m_source;
	//QString			m_field;
};
