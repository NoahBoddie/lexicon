#pragma once

namespace LEX
{
	struct ITypePolicy;

	using GenericInput = ITypePolicy*;

	struct DefaultGenericInput
	{
		ITypePolicy* policy = nullptr;
		std::string name;
	};


}