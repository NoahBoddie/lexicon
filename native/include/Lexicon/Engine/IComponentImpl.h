#pragma once

#include "Lexicon/Interfaces/IComponentBase.h"

namespace LEX
{

	struct IComponentImpl_Deleted : public IComponentBase
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::IComponentImpl)
	};
}