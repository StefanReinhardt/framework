#include <iostream>
#include "Application.h"

#include <core/Core.h>

// these includes are here only for node creation
#include <plugins/sim/Solver.h>

#include <plugins/clouds/2D/Advect2D.h>
#include <plugins/clouds/2D/Project2D.h>
#include <plugins/clouds/2D/WaterContinuity2D.h>
#include <plugins/clouds/2D/AddSource2D.h>
#include <plugins/clouds/2D/Buoyancy2D.h>
#include <plugins/clouds/2D/VortexConfinement2D.h>
#include <plugins/clouds/2D/AddHeatSource2D.h>

#include <plugins/clouds/3D/Advect.h>
#include <plugins/clouds/3D/Project.h>
#include <plugins/clouds/3D/WaterContinuity.h>
#include <plugins/clouds/3D/Buoyancy.h>
#include <plugins/clouds/3D/AddHeatSource.h>
#include <plugins/clouds/3D/AddHeatSource.h>
#include <plugins/clouds/3D/VortexConfinement.h>


void logger(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	switch (type) {
	case QtDebugMsg:
		//fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		//fprintf(stderr, "Debug: %s\n", localMsg.constData());
		break;
	case QtWarningMsg:
		//fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		fprintf(stderr, "Warning: %s\n", localMsg.constData());
		break;
	case QtCriticalMsg:
		//fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		fprintf(stderr, "Critical: %s\n", localMsg.constData());
		break;
	case QtFatalMsg:
		//fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		fprintf(stderr, "Fatal: %s\n", localMsg.constData());
		abort();
	}
}


int main(int argc, char ** argv)
{
	qInstallMessageHandler(logger);
	Application app(argc, argv);
	app.setOrganizationName("app");
	app.setApplicationName("app");


	bool commandline = true;
	if( !commandline )
		return app.exec();
	// standalone
	// generate and serialize graph

    {
/*
		core::Graph::Ptr graph = std::make_shared<core::Graph>();
        //core::GraphNode::Ptr cloudImport = graph->createNode("ImportClouds");
        core::GraphNode::Ptr cloudCreate = graph->createNode("CreateClouds");
		core::GraphNode::Ptr cloudExport = graph->createNode("ExportClouds", "export");

        graph->addConnection( cloudCreate, "output", cloudExport, "input" );

        //graph->addConnection( cloudImport, "output", cloudExport, "input" );

        core::save( "$HERE/test.json", graph );
*/
	}

	{

		core::Graph::Ptr graph = std::make_shared<core::Graph>();

		// create nodes
		//core::GraphNode::Ptr cloudImport = graph->createNode("ImportClouds");
		core::GraphNode::Ptr cloudCreate = graph->createNode("CreateClouds");
		Solver::Ptr solver = std::dynamic_pointer_cast<Solver>(graph->createNode("Solver", "solver"));
		core::GraphNode::Ptr cloudExport = graph->createNode("ExportClouds", "export");

		//***********************************************************************************************
		// setup Nodes 2D
		//***********************************************************************************************

/*

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





		//********** SOLVE FOR QC & QV & PT
		// Watercontinuity
		WaterContinuity2D::Ptr WaterCont = std::dynamic_pointer_cast<WaterContinuity2D>(solver->createOperator( "WaterContinuity2D", "water continuity" ));

		//********** ADD FORCES

		// Buoyancy
		Buoyancy2D::Ptr buoyantForce = std::dynamic_pointer_cast<Buoyancy2D>(solver->createOperator( "Buoyancy2D", "apply buoyant Force" ));

		// Add Heat Src
		AddHeatSource2D::Ptr heatInput = std::dynamic_pointer_cast<AddHeatSource2D>(solver->createOperator( "AddHeatSource2D", "add heat field" ));


		// Vortex confinement
		VortexConfinement2D::Ptr vortConf = std::dynamic_pointer_cast<VortexConfinement2D>(solver->createOperator("VortexConfinement2D", "add curls back in"));
		vortConf->setField("velocity");

		//********** PROJECT
		// Project
		Project2D::Ptr project = std::dynamic_pointer_cast<Project2D>(solver->createOperator( "Project2D", "projection step" ) );
		project->setField("velocity");



		//***********************************************************************************************
		// setup Nodes 2D end
		//***********************************************************************************************
*/


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





		//********** SOLVE FOR QC & QV & PT
		// Watercontinuity
		WaterContinuity::Ptr WaterCont = std::dynamic_pointer_cast<WaterContinuity>(solver->createOperator( "WaterContinuity", "water continuity" ));

		//********** ADD FORCES

		// Buoyancy
		Buoyancy::Ptr buoyantForce = std::dynamic_pointer_cast<Buoyancy>(solver->createOperator( "Buoyancy", "apply buoyant Force" ));

		// Add Heat Src
		AddHeatSource::Ptr heatInput = std::dynamic_pointer_cast<AddHeatSource>(solver->createOperator( "AddHeatSource", "add heat field" ));


		// Vortex confinement
		VortexConfinement::Ptr vortConf = std::dynamic_pointer_cast<VortexConfinement>(solver->createOperator("VortexConfinement", "add curls back in"));
		vortConf->setField("velocity");

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
		graph->addConnection( cloudCreate, "output", solver, "input" );
		//graph->addConnection( cloudImport, "output", solver, "input" );
		graph->addConnection( "$F", solver, "frame" );
		graph->addConnection( solver, "output", cloudExport, "input" );

		// save graph
		core::save( "$HERE/test.json", graph );

	}
	// debug
	// deserialize and execute graph
	{
		// load graph
		core::Graph::Ptr graph = core::load( "$HERE/test.json" );
		graph->print();

		// find node which saves cloud data to disk
		core::GraphNode::Ptr cloudExport = graph->getNode( "export" );

		// evaluate this node for 10 frames
		core::Timer timer;
		timer.start();
		graph->render( cloudExport, 1, 5000 );
		timer.stop();
		qCritical() << "time taken: " << timer.elapsedSeconds() << "s";
	}
	qDebug() << "EXIT";
	return 0;
}
