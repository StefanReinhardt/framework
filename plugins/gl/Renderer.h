#pragma once
#include <math/Math.h>
#include <core/Data.h>

#include <memory>
#include <stack>

#include "Context.h"


namespace gl
{
	class Renderer : public core::Data
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<Renderer> Ptr;


		virtual void                      render( Context* context ){}
	};

}
