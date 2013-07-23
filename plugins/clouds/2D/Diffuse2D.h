#pragma once

#include <plugins/sim/Operator.h>
#include <core/Core.h>

class Diffuse2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Diffuse2D>  Ptr;
	Diffuse2D();

	virtual void                       apply( SimObject::Ptr so, float dt )override;
	void                               setField(QString);
	void                               setDiffusion(float diff);



private:

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );

	core::Timer                        timer;
	float                              dt;
	QString                            m_diffusionField;
	float                              m_diff;
};
