#pragma once

#include <plugins/sim/Operator.h>

class Project2d : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Project2d> Ptr;
	Project2d();

	virtual void apply( SimObject::Ptr so )override;

	void setField(QString);

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );

private:
	//QString m_name;
	float dt;			// Timestep
	QString projectionField;
};
