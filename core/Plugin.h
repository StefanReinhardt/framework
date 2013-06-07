#pragma once

#include <memory>
#include <string>
#include <vector>


namespace core
{
	struct Plugin
	{
		typedef std::shared_ptr<Plugin> Ptr;

		virtual ~Plugin();
	};
}
