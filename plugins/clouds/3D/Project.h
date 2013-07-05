#pragma once

#include <plugins/sim/Operator.h>

class Project : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Project> Ptr;
	Project();

	virtual void apply( SimObject::Ptr so )override;

	void setField(QString);

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );

private:
	//QString m_name;
	float m_dt;			// Timestep
	QString projectionField;
};
