#include "Lexicon/Reflection.h"

#include "Lexicon/TypeInfo.h"
#include "Lexicon/VariableType.h"
#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{
	TypeInfo* Reflection::GetVariableType(const Reflection*& reflect)
	{
		Reflect type;
		size_t offset;

		if (reflect)
		{
			type = reflect->GetReflect();
			offset = reflect->GetReflectOffset();
		}
		else
		{
			type = Reflect::None;
			offset = 0;
		}

		auto result = IdentityManager::instance->GetTypeByOffset(std::format("REFLECT_{}", magic_enum::enum_name(type)), offset);

		if (!result) {
			report::error("'REFLECT_{}' at offset {} is empty.", magic_enum::enum_name(type), offset);
		}

		return result->GetTypePolicy(nullptr);

	}
}