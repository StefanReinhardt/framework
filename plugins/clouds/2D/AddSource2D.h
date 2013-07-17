#pragma once

#include <plugins/sim/Operator.h>

class AddSource2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<AddSource2D> Ptr;
	AddSource2D();

	virtual void                       apply( SimObject::Ptr so, float dt )override;
	void                               setFields(QString field, QString source);

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );
private:
	QString			m_source;
	QString			m_field;
};
