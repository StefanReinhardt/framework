#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>

#include "ImportHouGeo.h"



struct HoudiniPlugin : public core::Plugin
{
	HoudiniPlugin( core::Core::Ptr core )
	{
		core->addDataFactory( core::DataFactoryT<ImportHouGeo>::create(ImportHouGeo::staticMetaObject.className(), "loads .geo and .bgeo files from houdini") );
	}
};






core::Plugin::Ptr getPlugin_houdini( core::Core::Ptr core )
{
	return std::make_shared<HoudiniPlugin>(core);
}
