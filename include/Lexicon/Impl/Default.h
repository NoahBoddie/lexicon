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

	//A struct that allows call to have targets for invoke.
	struct Focus
	{
		RuntimeVariable value;

		template <std::convertible_to<RuntimeVariable> T>
		explicit Focus(T&& t) : value{ t }
		{

		}
	};
}