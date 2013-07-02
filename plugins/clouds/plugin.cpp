#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>

#include "CreateClouds.h"
#include "ExportClouds.h"
#include "ImportClouds.h"
#include "2D/Advect2D.h"
#include "2D/Project2D.h"
#include "2D/WaterContinuity2D.h"
#include "2D/AddSource2D.h"
#include "2D/Buoyancy2D.h"
#include "2D/VortexConfinement2D.h"
#include "2D/AddHeatSource2D.h"


struct CloudsPlugin : public core::Plugin
{
	CloudsPlugin( core::Core::Ptr core )
	{
        core->addDataFactory( core::DataFactoryT<CreateClouds>::create(CreateClouds::staticMetaObject.className(), "creates cloud default values") );
		core->addDataFactory( core::DataFactoryT<ImportClouds>::create(ImportClouds::staticMetaObject.className(), "imports initial cloud state to simobject") );
		core->addDataFactory( core::DataFactoryT<ExportClouds>::create(ExportClouds::staticMetaObject.className(), "exports cloud state from simobject") );
		core->addDataFactory( core::DataFactoryT<Advect2D>::create(Advect2D::staticMetaObject.className(), "advects name identified field in 2d given a name identified velocity ") );
		core->addDataFactory( core::DataFactoryT<Project2D>::create(Project2D::staticMetaObject.className(), "projects name identified 2d velocity field ") );
		core->addDataFactory( core::DataFactoryT<WaterContinuity>::create(WaterContinuity::staticMetaObject.className(), "balances water vapor and condensed cloud water") );
		core->addDataFactory( core::DataFactoryT<AddSource>::create(AddSource::staticMetaObject.className(), "adds a source to a field") );
		core->addDataFactory( core::DataFactoryT<Buoyancy>::create(Buoyancy::staticMetaObject.className(), "adds vertical velocity depending on temperature and water") );
		core->addDataFactory( core::DataFactoryT<VortexConfinement>::create(VortexConfinement::staticMetaObject.className(), "adds lost curl energy back in") );
		core->addDataFactory( core::DataFactoryT<AddHeatSource>::create(AddHeatSource::staticMetaObject.className(), "adds temperature to pt") );
	}
};






core::Plugin::Ptr getPlugin_clouds( core::Core::Ptr core )
{
	return std::make_shared<CloudsPlugin>(core);
}
