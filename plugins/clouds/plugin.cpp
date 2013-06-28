#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>

#include "CreateClouds.h"
#include "ExportClouds.h"
#include "ImportClouds.h"
#include "Advect2d.h"
#include "Project2d.h"
#include "WaterContinuity.h"
#include "AddSource.h"



struct CloudsPlugin : public core::Plugin
{
	CloudsPlugin( core::Core::Ptr core )
	{
        core->addDataFactory( core::DataFactoryT<CreateClouds>::create(CreateClouds::staticMetaObject.className(), "creates cloud default values") );
		core->addDataFactory( core::DataFactoryT<ImportClouds>::create(ImportClouds::staticMetaObject.className(), "imports initial cloud state to simobject") );
		core->addDataFactory( core::DataFactoryT<ExportClouds>::create(ExportClouds::staticMetaObject.className(), "exports cloud state from simobject") );
		core->addDataFactory( core::DataFactoryT<Advect2d>::create(Advect2d::staticMetaObject.className(), "advects name identified field in 2d given a name identified velocity ") );
		core->addDataFactory( core::DataFactoryT<Project2d>::create(Project2d::staticMetaObject.className(), "projects name identified 2d velocity field ") );
		core->addDataFactory( core::DataFactoryT<WaterContinuity>::create(WaterContinuity::staticMetaObject.className(), "balances water vapor and condensed cloud water") );
		core->addDataFactory( core::DataFactoryT<AddSource>::create(AddSource::staticMetaObject.className(), "adds a source to a field") );
	}
};






core::Plugin::Ptr getPlugin_clouds( core::Core::Ptr core )
{
	return std::make_shared<CloudsPlugin>(core);
}
