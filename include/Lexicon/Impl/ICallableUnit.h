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

		//TODO: I would like to split  ICallableUnit's Call and Invoke more finely.
		// RawCall- basically no fluff. This is what we use internally, others cant use it because it lacks varification.
		// Call- This is the normal call that people would use. This has to kinda be virtual cause I want it to be the thing that
		//  throws application errors when need be.
		// Largely, this basically means one is fit for internal and external stuff (the non-fancy one stil handles external calls so it should be careful on that)
		//  and one that's made to handle external calls only, which handles things such as conversions
		//names, call, invoke, execute. Execute is the raw version. Invoke is  the external version. Call is the fancy version just like before.
	INTERNAL:
		//This function does not check what is submitted to it, nor does it do conversions.
		//RuntimeVariable _Execute(RuntimeVariable a_this, std::vector<RuntimeVariable>&& args, RuntimeVariable* def);


		//This function will do conversions to apply if need be.
		//Variable _Invoke(Variable a_this, std::vector<Variable>&& args, Variable* def);


		//template <typename T>
		//T Call(std::vector<Variable>& args, Variable* def) { return {}; }
	public:

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