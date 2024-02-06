#pragma once

#include "Default.h"


namespace LEX
{
	struct Default;
	
	class Variable;
	class RuntimeVariable;
	
	

	struct ICallableUnit
	{
		//This is where all the things about something that can simply just be called goes.

		//Should this have the ability to get function data or something?

		//resolves arguments and confirms they are as they should be. Failure results in an application error.
		virtual void ResolveArguments(std::vector<RuntimeVariable>&) {}//Does nothing for now.

		//This is the very most basic function. There should be additional conveniences
		//virtual void Invoke(RuntimeVariable& ret, std::vector<RuntimeVariable> args) = 0;


		virtual RuntimeVariable Invoke(std::vector<RuntimeVariable>& args, RuntimeVariable* def) = 0;

		RuntimeVariable Call(std::vector<Variable>& args);

		RuntimeVariable Call(std::vector<RuntimeVariable>& args);
		
		template<std::convertible_to<RuntimeVariable>... Ts>
		RuntimeVariable Call(Ts&&... args)
		{
			std::vector<RuntimeVariable> t_args{ args... };
			auto value = Invoke(t_args, nullptr);
			return value;
		}


		template<std::convertible_to<RuntimeVariable>... Ts>
		RuntimeVariable Call(Default def, Ts&&... args)
		{
			std::vector<RuntimeVariable> t_args{ args... };
			auto value = Invoke(t_args, &def.value);
			return value;
		}

		//Need src
		//RuntimeVariable Invoke(std::vector<RuntimeVariable> args) { return Invoke(args, nullptr); }

	};

}