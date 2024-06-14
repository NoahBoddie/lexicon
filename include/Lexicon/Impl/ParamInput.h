#pragma once

//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"

#include "Qualifier.h"
#include "QualifiedType.h"

namespace LEX
{
	struct ITypePolicy;
	struct LocalInfo;


	


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