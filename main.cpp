#include <iostream>
#include "Application.h"

#include <core/Core.h>


void logger(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	switch (type) {
	case QtDebugMsg:
		//fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		fprintf(stderr, "Debug: %s\n", localMsg.constData());
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
		core::Graph::Ptr graph = std::make_shared<core::Graph>();
		core::GraphNode::Ptr cloudImport = graph->createNode("ImportClouds");
		core::GraphNode::Ptr cloudExport = graph->createNode("ExportClouds", "export");
		graph->addConnection( cloudImport, "output", cloudExport, "input" );

		core::save( "c:\\projects\\clouds\\bin\\test.json", graph );
	}

	// debug
	// deserialize and execute graph
	{
		core::Graph::Ptr graph = core::load( "c:\\projects\\clouds\\bin\\test.json" );
		core::GraphNode::Ptr cloudExport = graph->getNode( "export" );
		graph->print();
		graph->render( cloudExport, 0, 0 );
	}

	return 0;
}
