// Advection
//
// * advects a Scalar or a 3DVector field along a 3DVector field.
// * with setType we can set those fields and define, whether the bounds are periodic or not.
//



#pragma once

#include <plugins/sim/Operator.h>
#include <plugins/primitives/ScalarField.h>
#include <plugins/primitives/VectorField.h>
#include <core/Core.h>

class Advect2D : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Advect2D>   Ptr;
	Advect2D();



	void	                            setType(QString field, QString vecField, bool periodic);

private:
	virtual void                        applyImpl( SimObject::Ptr so, float dt )override;
	void                                advect(ScalarField::Ptr field, ScalarField::Ptr field_old, VectorField::Ptr vecField);
	// overloads from Data
	virtual void                        store( QJsonObject &o, QJsonDocument &doc );
	virtual void                        load( QJsonObject &o );


	QString                             advectionField;
	QString                             vecField;
	core::Timer                         timer;
	bool                                m_periodic;
	float                               m_dt;  // Timestep

};
