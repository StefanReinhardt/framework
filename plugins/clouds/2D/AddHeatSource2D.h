#pragma once

#include <core/Core.h>
#include <plugins/sim/Operator.h>

class AddHeatSource2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<AddHeatSource2D> Ptr;
	AddHeatSource2D();

	enum Input
	{
		BOTTOM,
		LEFT,
		RIGHT
	};

	virtual void                        apply( SimObject::Ptr so )override;

	void                                setAnimationSpeed(float speed);
	void                                setContrast(float contrast);
	void                                setEmitterSize(float size);
	void                                setFrequence(float frequency);
	void                                setStrenght(float strenght);
	void                                setOffset(float offset);
	void                                setInputFace(Input input);
	void                                setQvEmission(bool emitQv);
	void                                setPtEmission(bool emitPt);
	void                                setVelEmission(bool emitVel);


private:
	core::Timer                          timer;

	float                                m_frequency;
	float                                m_emitterSize;
	float                                m_contrast;
	float                                m_offset;
	float                                m_dt;
	float                                m_animationSpeed;
	float                                m_strenght;
	Input                                m_input;
	bool                                 m_emitQv;
	bool                                 m_emitPt;
	bool                                 m_emitVel;

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );

};
