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

extern core::Graph::Ptr clouds_graph2D();
//extern core::Graph::Ptr grandyn_demo1();

// seems like QCoreApplication::arguments() has a bug causing memory explosion...
QStringList arguments( int argc, char ** argv )
{
	QStringList list;

	const int ac = argc;
	char ** const av = argv;
	list.reserve(ac);

	for (int a = 0; a < ac; ++a)
		list << QString::fromLocal8Bit(av[a]);

	return list;
}

int main(int argc, char ** argv)
{
	QStringList args = arguments(argc, argv);
	bool standalone = true;

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
		if(args.size()<2)
		{
			core::Graph::Ptr graph = clouds_graph2D();

			// save graph
			core::save( "$HERE/test.json", graph );
		}

		// debug
		// deserialize and execute local graph if no arguments are given
		QString graphfilename = "$HERE/test.json";
		QString nodename = "export";
		int numFrames = 1;


		//arguments:
		// 0 - executeable filename
		// 1 - graph filepath
		// 2 - nodename to evaluate
		// 3 - number of frames
		if( args.size() >= 2 )
			graphfilename = args[1];
		if( args.size() >= 3 )
			nodename = args[2];
		if( args.size() >= 4 )
			numFrames = args[3].toInt();

		if(1)
		{
			// load graph
			core::Graph::Ptr graph = core::load( graphfilename );

			if( !graph )
				return -1;

			graph->print();

			// find node to render
			core::GraphNode::Ptr node = graph->getNode( nodename );

			if( node )
			{
				core::Timer timer;
				timer.start();
				// evaluate this node for some frames
				graph->render( node, 1, numFrames );
				timer.stop();
				qDebug() << "time taken: " << timer.elapsedSeconds() << "s";
			}else
				qCritical() << "unable to find node " << nodename;
		}

	}

	qDebug() << "EXIT";
	return 0;
}
