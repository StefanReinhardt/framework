#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>

#include "ExportClouds.h"
#include "ImportClouds.h"



struct CloudsPlugin : public core::Plugin
{
	CloudsPlugin( core::Core::Ptr core )
	{
		core->addDataFactory( core::DataFactoryT<ImportClouds>::create(ImportClouds::staticMetaObject.className(), "imports initial cloud state to simobject") );
		core->addDataFactory( core::DataFactoryT<ExportClouds>::create(ExportClouds::staticMetaObject.className(), "exports cloud state from simobject") );
	}
};






core::Plugin::Ptr getPlugin_clouds( core::Core::Ptr core )
{
	return std::make_shared<CloudsPlugin>(core);
}
