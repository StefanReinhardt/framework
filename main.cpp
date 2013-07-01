#include <iostream>
#include "Application.h"

#include <core/Core.h>

// these includes are here only for node creation
#include <plugins/sim/Solver.h>
#include <plugins/clouds/Advect2d.h>
#include <plugins/clouds/Project2d.h>
#include <plugins/clouds/WaterContinuity.h>
#include <plugins/clouds/AddSource.h>
#include <plugins/clouds/Buoyancy.h>
#include<plugins/clouds/VortexConfinement.h>


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
		Solver::Ptr solver = std::dynamic_pointer_cast<Solver>(graph->createNode("Solver"));
		core::GraphNode::Ptr cloudExport = graph->createNode("ExportClouds", "export");

		//***********************************************************************************************
		// setup Nodes
		//***********************************************************************************************



		//********** ADVECT FIELDS

		// Advect Density
		Advect2d::Ptr advectDensity = std::dynamic_pointer_cast<Advect2d>(solver->createOperator( "Advect2d", "advect density" ));
		advectDensity->setType("density", "velocity", true);

		// Advect qv
		Advect2d::Ptr advectQv = std::dynamic_pointer_cast<Advect2d>(solver->createOperator( "Advect2d", "advect qv" ));
		advectQv->setType("qv", "velocity", true);

		// Advect qc
		Advect2d::Ptr advectQc = std::dynamic_pointer_cast<Advect2d>(solver->createOperator( "Advect2d", "advect qc" ));
		advectQc->setType("qc", "velocity", false);

		// Advect pt
		Advect2d::Ptr advectPt = std::dynamic_pointer_cast<Advect2d>(solver->createOperator( "Advect2d", "advect qc" ));
		advectPt->setType("pt", "velocity", false);

		// Advect Velocity
		Advect2d::Ptr advectVelocity = std::dynamic_pointer_cast<Advect2d>(solver->createOperator( "Advect2d", "advect velocity" ));
		advectVelocity->setType("velocity", "velocity", false);


		//********** ADD FORCES

		// Buoyancy
		Buoyancy::Ptr buoyantForce = std::dynamic_pointer_cast<Buoyancy>(solver->createOperator( "Buoyancy", "apply buoyant Force" ));

		// Vortex confinement
		VortexConfinement::Ptr vortConf = std::dynamic_pointer_cast<VortexConfinement>(solver->createOperator("VortexConfinement", "add curls back in"));
		vortConf->setField("velocity");


		//********** SOLVE FOR QC & QV & PT
		// Watercontinuity
		WaterContinuity::Ptr WaterCont = std::dynamic_pointer_cast<WaterContinuity>(solver->createOperator( "WaterContinuity", "water continuity" ));

		//********** PROJECT
		// Project
		Project2d::Ptr project = std::dynamic_pointer_cast<Project2d>(solver->createOperator( "Project2d", "projection step" ) );
		project->setField("velocity");



		//***********************************************************************************************
		// setup Nodes end
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
		graph->render( cloudExport, 1, 200 );
		timer.stop();
		qCritical() << "time taken: " << timer.elapsedSeconds() << "s";
	}
	qDebug() << "EXIT";
	return 0;
}
