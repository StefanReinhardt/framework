#pragma once

#include <plugins/sim/Operator.h>

class AddHeatSource2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<AddHeatSource2D> Ptr;
	AddHeatSource2D();

	virtual void apply( SimObject::Ptr so )override;

	// overloads from Data
	//virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	//virtual void                       load( QJsonObject &o );
private:
	float                                m_scale;
	float                                m_contrast;
	float                                m_add;
	float                                m_dt;

	//QString			m_field;
};
