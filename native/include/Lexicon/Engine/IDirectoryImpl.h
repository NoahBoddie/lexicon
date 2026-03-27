#pragma once

#include "Lexicon/Interfaces/IDirectoryBase.h"

namespace LEX
{

	struct IDirectoryImpl_Deleted : public IDirectoryBase
	{
		static constexpr auto COMPONENT_TYPE = ComponentType::IDirectoryImpl;
	};
}