#pragma once

#include <plugins/sim/Operator.h>
#include <plugins/primitives/VectorField.h>
#include <plugins/primitives/ScalarField.h>

class VortexConfinement : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<VortexConfinement> Ptr;
	VortexConfinement();

	virtual void                       apply( SimObject::Ptr so )override;

	void                               setField(QString);
	void                               setStrenght(float strenght);
	void                               setOnCloudOnly(bool onCloud);



private:
	//QString m_name;
	float                               m_dt;			// Timestep
	float                               m_strenght;
	bool                                m_onCloudOnly;
	QString                             vortField;

	VectorField::Ptr                    vorticity;
	ScalarField::Ptr                    vel_x,vel_y,vel_z;

	void                               curl(int i, int j, int k);

	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );

};
