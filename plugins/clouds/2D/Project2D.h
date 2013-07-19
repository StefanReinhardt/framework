#pragma once

#include <plugins/sim/Operator.h>
#include <core/Core.h>

class Project2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Project2D>  Ptr;
	Project2D();

	virtual void                       apply( SimObject::Ptr so, float dt )override;

	void                               setField(QString);

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );

private:

	core::Timer                        timer;

	float                              dt;
	QString                            projectionField;
};
