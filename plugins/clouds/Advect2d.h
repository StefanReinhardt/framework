#pragma once

#include <plugins/sim/Operator.h>
#include <plugins/primitives/ScalarField.h>

class Advect2d : public Operator
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Advect2d> Ptr;
	Advect2d();

	virtual void apply( SimObject::Ptr so )override;

	void	setType(int, QString);


	// overloads from Data
	virtual void                       store( QJsonObject &o, QJsonDocument &doc );
	virtual void                       load( QJsonObject &o );

private:
	int b;
	QString advectionField;

	//QString m_name;
	float dt;			// Timestep
};
