#pragma once

#include "BasicQualifier.h"
#include "RuntimeQualifier.h"



namespace LEX
{
	struct ITypePolicy;
	struct LocalInfo;


	//Please relocate
	template <auto T>
	using TypeOf = decltype(T);


	template <auto Filter> requires (std::is_enum_v<TypeOf<Filter>>)
		constexpr bool FilterEquals(TypeOf<Filter> value, TypeOf<Filter> check)
	{

		using T = TypeOf<Filter>;

		using U = std::underlying_type_t<T>;

		U filtered = std::to_underlying(value) & std::to_underlying(Filter);

		return filtered == std::to_underlying(check);
	}


	struct ParamInput
	{
		ITypePolicy* policy{};

		RuntimeQualifier runtimeFlags{};
		BasicQualifier basicFlags{};


		ParamInput() = default;

		ParamInput(LocalInfo info);
	};

	struct DefaultParamInput : public ParamInput
	{
		std::string name;
	};

}