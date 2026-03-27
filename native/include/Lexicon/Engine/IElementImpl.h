#pragma once

#include "Lexicon/Interfaces/IElementBase.h"

namespace LEX
{
	class Script;
	class Project;
	struct Element;
	struct Environment;
	struct OverloadArgument;


	struct IElementImpl_Deleted : public IElementBase
	{
		static constexpr auto COMPONENT_TYPE = ComponentType::IElementImpl;
	};
}