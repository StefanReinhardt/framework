#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>

#include "ExportParticles.h"
#include "ImportParticles.h"

#include "2D/Integrate2D.h"
#include "2D/Gravity2D.h"


struct GrandynPlugin : public core::Plugin
{
	GrandynPlugin( core::Core::Ptr core )
	{
		/*
		core->addDataFactory( core::DataFactoryT<ImportParticles>::create(ImportParticles::staticMetaObject.className(), "imports initial particle state to simobject") );
		core->addDataFactory( core::DataFactoryT<ExportParticles>::create(ExportParticles::staticMetaObject.className(), "exports particle state from simobject") );

		// 2D
		core->addDataFactory( core::DataFactoryT<Gravity2D>::create(Gravity2D::staticMetaObject.className(), "adds gravity force") );
		core->addDataFactory( core::DataFactoryT<Integrate2D>::create(Integrate2D::staticMetaObject.className(), "proceeds particles") );
		*/
	}
};




core::Graph::Ptr grandyn_demo1()
{
	return core::Graph::Ptr();
}

core::Plugin::Ptr getPlugin_grandyn( core::Core::Ptr core )
{
	return std::make_shared<GrandynPlugin>(core);
}
