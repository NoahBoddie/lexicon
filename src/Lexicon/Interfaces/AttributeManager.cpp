#include "Lexicon/Interfaces/AttributeManager.h"

#include "Lexicon/Engine/TypeBase.h"

namespace LEX
{
	struct 
	{
		std::unordered_map<std::string, AttrDataBuilder> builders;
		std::unordered_map<std::string, TypeBase*> waiters;
	}& data = make_singleton();



	bool AttributeManager::RegisterNativeData(const std::string_view& name, AttributeBuilder builder)
	{
		auto& result = data.builders[std::string{ name }];

		assert_if(result) {
			return false;
		}

		result = builder;

		return true;
	}


	//Registers attributes that have yet to be registered properly.
	std::optional<bool> AttributeManager::RequestNativeData(const std::string_view& name, TypeBase* type)
	{
		if (auto builder = GetBuilder(name)) {
			type->attrBuilder = builder;
			return true;
		}
		else {
			auto& result = data.waiters[std::string{ name }];

			assert_if(result) {
				return false;
			}

			result = type;

			return std::nullopt;
		}
	}


	AttributeBuilder AttributeManager::GetBuilder(const std::string_view& name)
	{
		auto it = data.builders.find(std::string{ name });
		
		if (data.builders.end() != it) {
			return it->second;
		}

		return nullptr;
	}
}