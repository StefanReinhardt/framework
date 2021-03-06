#pragma once

#include <plugins/sim/Operator.h>

class AddHeatSource : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<AddHeatSource> Ptr;
	AddHeatSource();

	virtual void                        applyImpl( SimObject::Ptr so, float dt )override;

	void                                setAnimationSpeed(float speed);
	void                                setContrast(float contrast);
	void                                setEmitterSize(float size);
	void                                setFrequence(float frequency);
	void                                setTemperature(float temp);
	void                                setOffset(float offset);


private:

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
