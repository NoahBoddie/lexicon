#pragma once

//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"

#include "Qualifier.h"

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
		//To be replaced by qualified type

		ITypePolicy* policy{};
		Qualifier flags{};
		


		ParamInput() = default;

		ParamInput(LocalInfo info);
	};

	struct DefaultParamInput : public ParamInput
	{
		//To be renamed DefaultInput.
		std::string name;
	};

}