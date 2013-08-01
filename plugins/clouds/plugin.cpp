#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>

#include "CreateClouds.h"
#include "ExportClouds.h"
#include "ImportClouds.h"

#include "2D/Advect2D.h"
#include "2D/Project2D.h"
#include "2D/WaterContinuity2D.h"
#include "2D/AddSource2D.h"
#include "2D/Buoyancy2D.h"
#include "2D/Diffuse2D.h"
#include "2D/VortexConfinement2D.h"
#include "2D/AddHeatSource2D.h"
#include "2D/CloudControl2D.h"

#include "3D/Advect.h"
#include "3D/Project.h"
#include "3D/WaterContinuity.h"
#include "3D/Buoyancy.h"
#include "3D/AddHeatSource.h"
#include "3D/VortexConfinement.h"

// these includes are here only for building the graph in code =========================
#include <plugins/sim/Solver.h>

struct CloudsPlugin : public core::Plugin
{
	CloudsPlugin( core::Core::Ptr core )
	{
        core->addDataFactory( core::DataFactoryT<CreateClouds>::create(CreateClouds::staticMetaObject.className(), "creates cloud default values") );
		core->addDataFactory( core::DataFactoryT<ImportClouds>::create(ImportClouds::staticMetaObject.className(), "imports initial cloud state to simobject") );
		core->addDataFactory( core::DataFactoryT<ExportClouds>::create(ExportClouds::staticMetaObject.className(), "exports cloud state from simobject") );

		// 2D
		core->addDataFactory( core::DataFactoryT<Advect2D>::create(Advect2D::staticMetaObject.className(), "advects name identified field in 2d given a name identified velocity ") );
		core->addDataFactory( core::DataFactoryT<Project2D>::create(Project2D::staticMetaObject.className(), "projects name identified 2d velocity field ") );
		core->addDataFactory( core::DataFactoryT<WaterContinuity2D>::create(WaterContinuity2D::staticMetaObject.className(), "balances water vapor and condensed cloud water") );
		core->addDataFactory( core::DataFactoryT<AddSource2D>::create(AddSource2D::staticMetaObject.className(), "adds a source to a field") );
		core->addDataFactory( core::DataFactoryT<Buoyancy2D>::create(Buoyancy2D::staticMetaObject.className(), "adds vertical velocity depending on temperature and water") );
		core->addDataFactory( core::DataFactoryT<Diffuse2D>::create(Diffuse2D::staticMetaObject.className(), "diffuses a given field") );
		core->addDataFactory( core::DataFactoryT<VortexConfinement2D>::create(VortexConfinement2D::staticMetaObject.className(), "adds lost curl energy back in") );
		core->addDataFactory( core::DataFactoryT<AddHeatSource2D>::create(AddHeatSource2D::staticMetaObject.className(), "adds temperature to pt") );
		core->addDataFactory( core::DataFactoryT<CloudControl2D>::create(CloudControl2D::staticMetaObject.className(), "cloud target morph") );

		// 3D
		core->addDataFactory( core::DataFactoryT<Advect>::create(Advect::staticMetaObject.className(),"advects name identified field in 3d given a name identified velocity ") );
		core->addDataFactory( core::DataFactoryT<Project>::create(Project::staticMetaObject.className(), "projects name identified 3d velocity field ") );
		core->addDataFactory( core::DataFactoryT<WaterContinuity>::create(WaterContinuity::staticMetaObject.className(), "balances water vapor and condensed cloud water") );
		core->addDataFactory( core::DataFactoryT<Buoyancy>::create(Buoyancy::staticMetaObject.className(), "adds vertical velocity depending on temperature and water") );
		core->addDataFactory( core::DataFactoryT<AddHeatSource>::create(AddHeatSource::staticMetaObject.className(), "adds temperature to pt") );
		core->addDataFactory( core::DataFactoryT<VortexConfinement>::create(VortexConfinement::staticMetaObject.className(), "adds lost curl energy back in") );

	}
};



core::Graph::Ptr clouds_graph2D()
{
	core::Graph::Ptr graph = std::make_shared<core::Graph>();

	// create nodes
	core::GraphNode::Ptr cloudCreate = graph->createNode("CreateClouds");
	Solver::Ptr solver = std::dynamic_pointer_cast<Solver>(graph->createNode("Solver", "solver"));
	core::GraphNode::Ptr cloudExport = graph->createNode("ExportClouds", "export");

	solver->setTimeStrech(96.0f); // stretches 1 step to 1 second


	//********** ADVECT FIELDS

	// Advect Density
	Advect2D::Ptr advectDensity = std::dynamic_pointer_cast<Advect2D>(solver->createOperator( "Advect2D", "advect density" ));
	advectDensity->setType("density", "velocity", true);

	// Advect qv
	Advect2D::Ptr advectQv = std::dynamic_pointer_cast<Advect2D>(solver->createOperator( "Advect2D", "advect qv" ));
	advectQv->setType("qv", "velocity", true);

	// Advect qc
	Advect2D::Ptr advectQc = std::dynamic_pointer_cast<Advect2D>(solver->createOperator( "Advect2D", "advect qc" ));
	advectQc->setType("qc", "velocity", false);

	// Advect pt
	Advect2D::Ptr advectPt = std::dynamic_pointer_cast<Advect2D>(solver->createOperator( "Advect2D", "advect qc" ));
	advectPt->setType("pt", "velocity", false);

	// Advect Velocity
	Advect2D::Ptr advectVelocity = std::dynamic_pointer_cast<Advect2D>(solver->createOperator( "Advect2D", "advect velocity" ));
	advectVelocity->setType("velocity", "velocity", false);

	// Vortex confinement
	VortexConfinement2D::Ptr vortConf = std::dynamic_pointer_cast<VortexConfinement2D>(solver->createOperator("VortexConfinement2D", "vortex confinement"));
	vortConf->setField("velocity");
	vortConf->setStrength(0.015f);
	vortConf->setOnCloudOnly(false);
/*
*/
	// Cloud Control
//	CloudControl2D::Ptr CloudCtrl = std::dynamic_pointer_cast<CloudControl2D>(solver->createOperator( "CloudControl2D", "target morph"));

	//********** ADD FORCES
	// buoyancy and vort Conf should have same vel input field.
	// Buoyancy
	Buoyancy2D::Ptr buoyantForce = std::dynamic_pointer_cast<Buoyancy2D>(solver->createOperator( "Buoyancy2D", "apply buoyant Force" ));
	buoyantForce->setStrenght(1.00000f);



	//********** SOLVE FOR QC & QV & PT
	// Watercontinuity
	WaterContinuity2D::Ptr WaterCont = std::dynamic_pointer_cast<WaterContinuity2D>(solver->createOperator( "WaterContinuity2D", "water continuity" ));

/*
		Diffuse2D::Ptr diffPt = std::dynamic_pointer_cast<Diffuse2D>(solver->createOperator( "Diffuse2D", "diffuse Temperature" ));
		diffPt->setField("pt");
		diffPt->setDiffusion(0.000001f);			//0.00001f little to much

		Diffuse2D::Ptr diffQv = std::dynamic_pointer_cast<Diffuse2D>(solver->createOperator( "Diffuse2D", "diffuse Temperature" ));
		diffQv->setField("qv");
		diffQv->setDiffusion(0.000001f);

	*/

	// +++++++++++++++++
	// sources for Cumulus Setup

	// Add Velocity Source
	AddHeatSource2D::Ptr velInput = std::dynamic_pointer_cast<AddHeatSource2D>(solver->createOperator( "AddHeatSource2D", "add Velocity input" ));
	velInput->setAnimationSpeed(0.1f);
	velInput->setContrast(1.20f);
	velInput->setEmitterSize(0.2333333f);
	velInput->setFrequence(100.0f);
	velInput->setStrenght(2.01f);		// meter per sec
	velInput->setOffset(2.30f);
	velInput->setVelEmission(true);

	AddHeatSource2D::Ptr heatInput =  std::dynamic_pointer_cast<AddHeatSource2D>(solver->createOperator( "AddHeatSource2D", "add Heat input" ));
	heatInput->setAnimationSpeed(0.051f);
	heatInput->setContrast(2.00f);
	heatInput->setEmitterSize(0.2333333f);
	heatInput->setFrequence(100.0f);
	heatInput->setStrenght(25.0f);
	heatInput->setOffset(0.50f);
	heatInput->setPtEmission(true);


	AddHeatSource2D::Ptr vaporInput =  std::dynamic_pointer_cast<AddHeatSource2D>(solver->createOperator( "AddHeatSource2D", "add Heat input" ));
	vaporInput->setAnimationSpeed(0.051f);
	vaporInput->setContrast(1.00f);
	vaporInput->setEmitterSize(0.2333333f);
	vaporInput->setFrequence(100.0f);
	vaporInput->setStrenght(0.0015f);
	vaporInput->setOffset(2.0f);
	vaporInput->setQvEmission(true);

/*
	// Add Heat Src for Side Wind
	AddHeatSource2D::Ptr heatInput = std::dynamic_pointer_cast<AddHeatSource2D>(solver->createOperator( "AddHeatSource2D", "add heat field" ));
	heatInput->setAnimationSpeed(0.02f);
	heatInput->setContrast(0.80f);
	heatInput->setEmitterSize(0.80f);
	heatInput->setFrequence(1.0f);
	heatInput->setTemperature(-0.0f);
	heatInput->setOffset(0.4f);
	heatInput->setQvEmission(true);
	heatInput->setInputFace(AddHeatSource2D::LEFT);

	// Add Heat Src for Side Wind
	AddHeatSource2D::Ptr heatInputR = std::dynamic_pointer_cast<AddHeatSource2D>(solver->createOperator( "AddHeatSource2D", "add heat field" ));
	heatInputR->setAnimationSpeed(0.02f);
	heatInputR->setContrast(0.80f);
	heatInputR->setEmitterSize(1.0f);
	heatInputR->setFrequence(1.0f);
	heatInputR->setTemperature(5.0f);
	heatInputR->setOffset(0.4f);
	heatInputR->setInputFace(AddHeatSource2D::RIGHT);
*/
	//********** PROJECT
	// Project
	Project2D::Ptr project = std::dynamic_pointer_cast<Project2D>(solver->createOperator( "Project2D", "projection step" ) );
	project->setField("velocity");


	//
	// set inputs
	cloudExport->getSocket("file")->setString("$HERE/cloud_output.$F4.bgeo");

	// make connections
	graph->addConnection( "$F", cloudCreate, "frame" );
	graph->addConnection( cloudCreate, "output", solver, "input" );
	graph->addConnection( "$F", solver, "frame" );
	graph->addConnection( solver, "output", cloudExport, "input" );

	return  graph;
}

core::Graph::Ptr clouds_graph3D()
{
	core::Graph::Ptr graph = std::make_shared<core::Graph>();

	// create nodes
	//core::GraphNode::Ptr cloudImport = graph->createNode("ImportClouds");
	core::GraphNode::Ptr cloudCreate = graph->createNode("CreateClouds");
	Solver::Ptr solver = std::dynamic_pointer_cast<Solver>(graph->createNode("Solver", "solver"));
	core::GraphNode::Ptr cloudExport = graph->createNode("ExportClouds", "export");

	//***********************************************************************************************
	// setup Nodes 3D
	//***********************************************************************************************

	//********** ADVECT FIELDS

	// Advect Density
	Advect::Ptr advectDensity = std::dynamic_pointer_cast<Advect>(solver->createOperator( "Advect", "advect density" ));
	advectDensity->setType("density", "velocity", true);

	// Advect qv
	Advect::Ptr advectQv = std::dynamic_pointer_cast<Advect>(solver->createOperator( "Advect", "advect qv" ));
	advectQv->setType("qv", "velocity", true);

	// Advect qc
	Advect::Ptr advectQc = std::dynamic_pointer_cast<Advect>(solver->createOperator( "Advect", "advect qc" ));
	advectQc->setType("qc", "velocity", false);

	// Advect pt
	Advect::Ptr advectPt = std::dynamic_pointer_cast<Advect>(solver->createOperator( "Advect", "advect qc" ));
	advectPt->setType("pt", "velocity", false);

	// Advect Velocity
	Advect::Ptr advectVelocity = std::dynamic_pointer_cast<Advect>(solver->createOperator( "Advect", "advect velocity" ));
	advectVelocity->setType("velocity", "velocity", false);





	//********** ADD FORCES

	// Vortex confinement
	VortexConfinement::Ptr vortConf = std::dynamic_pointer_cast<VortexConfinement>(solver->createOperator("VortexConfinement", "add curls back in"));
	vortConf->setField("velocity");

	// Buoyancy
	Buoyancy::Ptr buoyantForce = std::dynamic_pointer_cast<Buoyancy>(solver->createOperator( "Buoyancy", "apply buoyant Force" ));
	buoyantForce->setStrenght(0.8f);

	// Add Heat Src
	AddHeatSource::Ptr heatInput = std::dynamic_pointer_cast<AddHeatSource>(solver->createOperator( "AddHeatSource", "add heat field" ));
	heatInput->setAnimationSpeed(0.02f);
	heatInput->setContrast(0.80f);
	heatInput->setEmitterSize(0.2f);
	heatInput->setFrequence(1.0f);
	heatInput->setTemperature(15.0f);
	heatInput->setOffset(0.4f);

	//********** SOLVE FOR QC & QV & PT
	// Watercontinuity
	WaterContinuity::Ptr WaterCont = std::dynamic_pointer_cast<WaterContinuity>(solver->createOperator( "WaterContinuity", "water continuity" ));



	//********** PROJECT
	// Project
	Project::Ptr project = std::dynamic_pointer_cast<Project>(solver->createOperator( "Project", "projection step" ) );
	project->setField("velocity");


	//***********************************************************************************************
	// setup Nodes 3D end
	//***********************************************************************************************

	//
	// set inputs
	//cloudImport->getSocket("file")->setString("$HERE/cloud_initial.bgeo");
	cloudExport->getSocket("file")->setString("$HERE/cloud_output.$F4.bgeo");

	// make connections
	graph->addConnection( "$F", cloudCreate, "frame" );
	graph->addConnection( cloudCreate, "output", solver, "input" );
	//graph->addConnection( cloudImport, "output", solver, "input" );
	graph->addConnection( "$F", solver, "frame" );
	graph->addConnection( solver, "output", cloudExport, "input" );

	// save graph
	return  graph;
}


core::Plugin::Ptr getPlugin_clouds( core::Core::Ptr core )
{
	return std::make_shared<CloudsPlugin>(core);
}
