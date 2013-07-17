#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>

#include "ExportParticles.h"
#include "ImportParticles.h"

#include "2D/Integrate2D.h"
#include "2D/Gravity2D.h"


// these includes are here only for building the graph in code =========================
#include <plugins/sim/Solver.h>

struct GrandynPlugin : public core::Plugin
{
	GrandynPlugin( core::Core::Ptr core )
	{
		core->addDataFactory( core::DataFactoryT<ExportParticles>::create(ExportParticles::staticMetaObject.className(), "exports particle state from simobject") );
		core->addDataFactory( core::DataFactoryT<ImportParticles>::create(ImportParticles::staticMetaObject.className(), "imports initial particle state to simobject") );

		// 2D
		core->addDataFactory( core::DataFactoryT<Gravity2D>::create(Gravity2D::staticMetaObject.className(), "adds gravity force") );
		//core->addDataFactory( core::DataFactoryT<Integrate2D>::create(Integrate2D::staticMetaObject.className(), "proceeds particles") );
	}
};




core::Graph::Ptr grandyn_demo2D()
{
	core::Graph::Ptr graph = std::make_shared<core::Graph>();

	// create nodes =================
	core::GraphNode::Ptr import = graph->createNode("ImportParticles");
	//import->getSocket("file")->setString("$HERE/input.bgeo");


	Solver::Ptr solver = std::dynamic_pointer_cast<Solver>(graph->createNode("Solver"));

	core::GraphNode::Ptr xport = graph->createNode("ExportParticles", "export");
	xport->getSocket("file")->setString("$HERE/particles.$F4.bgeo");


	// connect nodes =================
	graph->addConnection( import, "output", solver, "input" );
	graph->addConnection( "$F", solver, "frame" );
	graph->addConnection( solver, "output", xport, "input" );


	// setup solver =================
	solver->createOperator("Gravity2D", "gravity force");


	return graph;
}

core::Plugin::Ptr getPlugin_grandyn( core::Core::Ptr core )
{
	return std::make_shared<GrandynPlugin>(core);
}
