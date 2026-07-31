#pragma once

#include "Lexicon/Engine/VarIndexInfo.h"

namespace LEX
{

	struct FieldInfo : public VarIndexInfo
	{
		DEFINE_INFO_TYPE(InfoType::FieldInfo)
	public:

		//std::unique_ptr<Routine> def_value{};
	};
}