#pragma once

#include <plugins/sim/Operator.h>
#include <plugins/primitives/VectorField.h>
#include <plugins/primitives/ScalarField.h>
#include <core/Core.h>

class VortexConfinement2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<VortexConfinement2D> Ptr;
	VortexConfinement2D();

	virtual void                       applyImpl( SimObject::Ptr so, float dt )override;

	void                               setField(QString);
	void                               setStrength(float strength);
	float                              getStrength()const;
	void                               setOnCloudOnly(bool onCloud);

private:

	core::Timer                        timer;
	float                              m_strength;
	float                              m_dt;			// Timestep
	bool                               m_onCloudOnly;
	QString                            m_vortField;
	ScalarField::Ptr                   vel_x,vel_y,vel_z;

	float                              curl(int i, int j, int k);

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );
};
