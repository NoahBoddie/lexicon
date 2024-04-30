#pragma once

//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"

#include "Qualifier.h"
#include "QualifiedType.h"

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

	template <auto Filter> requires (std::is_enum_v<TypeOf<Filter>>)
		constexpr TypeOf<Filter> FilterOR(TypeOf<Filter> value, TypeOf<Filter> add)
	{

		using T = TypeOf<Filter>;

		using U = std::underlying_type_t<T>;
		//We remove the filter from the value, and remove everything but the filter from add.
		U fil_ret = std::to_underlying(value) & ~std::to_underlying(Filter);
		U fil_add = std::to_underlying(add) & std::to_underlying(Filter);

		return static_cast<T>(fil_ret | fil_add);
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

	//This is to be what default input is, but I'm mulling it over.
	struct __ParamInput : public QualifiedType
	{
		std::string name;
	};
}