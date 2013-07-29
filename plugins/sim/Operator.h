#pragma once

#include <core/Data.h>
#include "SimObject.h"






class Operator : public core::Data
{
	Q_OBJECT
public:
	typedef std::shared_ptr<Operator> Ptr;

	Operator();

	void                            apply( SimObject::Ptr so, float dt ); // dt in seconds
	void                            setEnabled( bool enabled );
	bool                            isEnabled()const;

protected:
	virtual void                    applyImpl( SimObject::Ptr so, float dt ); // dt in seconds
private:
	bool                            m_enabled;
};
