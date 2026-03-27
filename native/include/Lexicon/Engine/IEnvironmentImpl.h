#pragma once

#include "Lexicon/Interfaces/IEnvironmentBase.h"

namespace LEX
{
	class Script;
	class Project;
	struct Element;
	struct Environment;
	struct OverloadArgument;


	struct IEnvironmentImpl_Deleted : public IEnvironmentBase
	{
		static constexpr auto COMPONENT_TYPE = ComponentType::IEnvironmentImpl;
	};
}