#pragma once

#include <plugins/sim/Operator.h>

class AddSource : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<AddSource> Ptr;
	AddSource();

	virtual void apply( SimObject::Ptr so )override;

	void setFields(QString field, QString source);

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );
private:
	QString			m_source;
	QString			m_field;
};
