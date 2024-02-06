#pragma once

#include "RuntimeVariable.h"


namespace LEX
{
	//A struct to be used in callable unit whenever one wants to signify a default value, rather
	// than an argument.
	struct Default
	{
		RuntimeVariable value;

		template <std::convertible_to<RuntimeVariable> T>
		explicit Default(T&& t) : value {t}
		{

		}
	};
}