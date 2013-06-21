#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>

#include "Camera.h"
#include "ScalarField.h"
#include "VectorField.h"



struct PrimitivesPlugin : public core::Plugin
{
	PrimitivesPlugin( core::Core::Ptr core )
	{
		core->addDataFactory( core::DataFactoryT<Camera>::create(Camera::staticMetaObject.className(), "camera primitive") );
		core->addDataFactory( core::DataFactoryT<ScalarField>::create(ScalarField::staticMetaObject.className(), "scalar field") );
		core->addDataFactory( core::DataFactoryT<VectorField>::create(VectorField::staticMetaObject.className(), "vector field") );
	}
};






core::Plugin::Ptr getPlugin_primitives( core::Core::Ptr core )
{
	return std::make_shared<PrimitivesPlugin>(core);
}
