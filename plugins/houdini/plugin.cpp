#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>

#include "ImportHoudini.h"




struct HoudiniPlugin : public core::Plugin
{
	HoudiniPlugin( core::Core::Ptr core )
	{
		core->addDataFactory( core::DataFactoryT<ImportHoudini>::create(ImportHoudini::staticMetaObject.className(), "imports .(b)geo or .json (houdini scene exports)") );
	}
};






core::Plugin::Ptr getPlugin_houdini( core::Core::Ptr core )
{
	return std::make_shared<HoudiniPlugin>(core);
}
