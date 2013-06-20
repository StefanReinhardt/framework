#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>

#include "SimObject.h"
#include "Solver.h"



struct SimPlugin : public core::Plugin
{
	SimPlugin( core::Core::Ptr core )
	{
		core->addDataFactory( core::DataFactoryT<SimObject>::create(SimObject::staticMetaObject.className(), "simlation object") );
		core->addDataFactory( core::DataFactoryT<Solver>::create(Solver::staticMetaObject.className(), "solver") );
	}
};






core::Plugin::Ptr getPlugin_sim( core::Core::Ptr core )
{
	return std::make_shared<SimPlugin>(core);
}
