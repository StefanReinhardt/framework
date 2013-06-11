#include <core/Core.h>
#include <core/DataFactory.h>
#include <core/Plugin.h>




struct HoudiniPlugin : public core::Plugin
{
	HoudiniPlugin( core::Core::Ptr core )
	{
	}
};






core::Plugin::Ptr getPlugin_houdini( core::Core::Ptr core )
{
	return std::make_shared<HoudiniPlugin>(core);
}
