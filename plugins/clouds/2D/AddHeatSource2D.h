#pragma once

#include <core/Core.h>
#include <plugins/sim/Operator.h>

class AddHeatSource2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<AddHeatSource2D> Ptr;
	AddHeatSource2D();

	virtual void apply( SimObject::Ptr so )override;

	void                                setAnimationSpeed(float speed);
	void                                setContrast(float contrast);
	void                                setEmitterSize(float size);
	void                                setFrequence(float frequency);
	void                                setTemperature(float temp);
	void                                setOffset(float offset);


private:
	core::Timer                          timer;

	float                                m_frequency;
	float                                m_emitterSize;
	float                                m_contrast;
	float                                m_offset;
	float                                m_dt;
	float                                m_animationSpeed;
	float                                m_tempInput;

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );

};
