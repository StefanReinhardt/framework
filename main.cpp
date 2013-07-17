#include <iostream>

#include <frontend/Application.h>

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



class StandaloneApplication : public QApplication
{
public:
	StandaloneApplication( int argc, char **argv ) : QApplication(argc, argv)
	{
		//  initialize core =======================
		core::init();
		core::setVariable( "$HERE", applicationDirPath() );
	}

	virtual ~StandaloneApplication()
	{
		//  shutdown core =======================
		core::shutdown();
	}
};

extern core::Graph::Ptr clouds_graph1();
//extern core::Graph::Ptr grandyn_demo1();

int main(int argc, char ** argv)
{
	bool standalone = false;

	qInstallMessageHandler(logger);

	if( !standalone )
	{
		// Frontend ================
		frontend::Application app(argc, argv);
		app.setOrganizationName("app");
		app.setApplicationName("app");
		return app.exec();
	}else
	{
		// Standalone/commandline ================
		StandaloneApplication app(argc, argv);
		app.setOrganizationName("app");
		app.setApplicationName("app");

		// generate and serialize some hardcoded graph if there are no arguments
		if(QApplication::arguments().size()<2)
		{
			core::Graph::Ptr graph = clouds_graph1();

			// save graph
			core::save( "$HERE/test.json", graph );
		}

		// debug
		// deserialize and execute local graph if no arguments are given
		QString graphfilename = "$HERE/test.json";
		QString nodename = "export";

		//arguments:
		// 0 - executeable filename
		// 1 - graph filepath
		// 2 - nodename to evaluate
		if( QApplication::arguments().size() >= 2 )
			graphfilename = QApplication::arguments()[1];
		if( QApplication::arguments().size() >= 3 )
			nodename = QApplication::arguments()[2];

		if(1)
		{
			// load graph
			core::Graph::Ptr graph = core::load( graphfilename );

			if( !graph )
				return -1;

			graph->print();

			// find node to render
			core::GraphNode::Ptr node = graph->getNode( nodename );

			// evaluate this node for some frames
			core::Timer timer;
			timer.start();

			graph->render( node, 1, 2 );

			timer.stop();
			qCritical() << "time taken: " << timer.elapsedSeconds() << "s";
		}
	}

	qDebug() << "EXIT";
	return 0;
}
