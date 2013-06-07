#include "Camera.h"

#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>



struct PrimitivesPlugin : public core::Plugin
{
	PrimitivesPlugin( core::Core::Ptr core )
	{
		core->addDataFactory( core::DataFactoryT<Camera>::create(Camera::staticMetaObject.className(), "camera primitive") );
	}
};






core::Plugin::Ptr getPlugin_primitives( core::Core::Ptr core )
{
	return std::make_shared<PrimitivesPlugin>(core);
}
