#pragma once

//*src
#include "RuntimeVariable.h"

namespace LEX
{
	struct RoutineBase;
	class Variable;
	class RuntimeVariable;

	struct ICallableUnit
	{
		//This is where all the things about something that can simply just be called goes.

		//Should this have the ability to get function data or something?

		//resolves arguments and confirms they are as they should be. Failure results in an application error.
		virtual void ResolveArguments(std::vector<RuntimeVariable>&) {}//Does nothing for now.

		//This is the very most basic function. There should be additional conveniences
		virtual void Invoke(RuntimeVariable& ret, std::vector<RuntimeVariable> args) = 0;


		virtual RuntimeVariable Invoke(std::vector<RuntimeVariable> args, RuntimeVariable* def) = 0;


		RuntimeVariable Invoke(std::vector<RuntimeVariable> args) { return Invoke(args, nullptr); }

	};

}