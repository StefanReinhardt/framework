#include "Clouds.ui.h"
#include <frontend/DoubleValidator.h>

#include <plugins/sim/Solver.h>

#include "2D/VortexConfinement2D.h"

CloudsUI::CloudsUI( core::Graph::Ptr graph, QWidget *parent) : QWidget(parent), m_graph(graph)
{
	ui.setupUi(this);


	Solver::Ptr solver = m_graph->getNode<Solver>("solver");

	// vorticity confinement ===
	VortexConfinement2D::Ptr vc = solver->getOperator<VortexConfinement2D>( "vortex confinement" );

	ui.vcGroup->setChecked( vc->isEnabled() );
	connect( ui.vcGroup, SIGNAL(toggled(bool)), this, SLOT(onVCToggled(bool)) );
	ui.vcStrengthSlider->setValue( (int)(vc->getStrength()*(ui.vcStrengthSlider->maximum() - ui.vcStrengthSlider->minimum())) + ui.vcStrengthSlider->minimum() );
	connect( ui.vcStrengthSlider, SIGNAL(valueChanged(int)), this, SLOT(onVCStrengthChanged(int)) );

}

void CloudsUI::onVCToggled( bool checked )
{
	m_graph->getNode<Solver>("solver")->getOperator<VortexConfinement2D>( "vortex confinement" )->setEnabled( checked );
	makeDirty();
}

void CloudsUI::onVCStrengthChanged( int value )
{
	float t = (float)value/(float)(ui.vcStrengthSlider->maximum() - ui.vcStrengthSlider->minimum());
	m_graph->getNode<Solver>("solver")->getOperator<VortexConfinement2D>( "vortex confinement" )->setStrength( t );
	makeDirty();
}

