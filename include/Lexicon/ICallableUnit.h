#pragma once

#include "Default.h"


namespace LEX
{
	struct Default;
	
	class Variable;
	class Runtime;
	class IRuntime;
	class RuntimeVariable;
	
	

	struct ICallableUnit
	{
		//This is where all the things about something that can simply just be called goes.

		//Should this have the ability to get function data or something?



		//This is the very most basic function. There should be additional conveniences
		//virtual void Execute(RuntimeVariable& ret, std::vector<RuntimeVariable> args) = 0;

		//TODO: I would like to split  ICallableUnit's Call and Execute more finely.
		// RawCall- basically no fluff. This is what we use internally, others cant use it because it lacks varification.
		// Call- This is the normal call that people would use. This has to kinda be virtual cause I want it to be the thing that
		//  throws application errors when need be.
		// Largely, this basically means one is fit for internal and external stuff (the non-fancy one stil handles external calls so it should be careful on that)
		//  and one that's made to handle external calls only, which handles things such as conversions
		//names, call, invoke, execute. Execute is the raw version. Invoke is  the external version. Call is the fancy version just like before.
		
		//New name, run. It was to be a lazy function, but targets are not needed more often then they are, so I'm xing that.
		// IF, I ever make a sort of promise like system, I'll likely call its function Run.

	INTERNAL:
		//This function does not check what is submitted to it, nor does it do conversions.
		//RuntimeVariable _Execute(RuntimeVariable a_this, std::vector<RuntimeVariable>&& args, RuntimeVariable* def);


		//This function will do conversions to apply if need be.
		//Variable _Invoke(Variable a_this, std::vector<Variable>&& args, Variable* def);


		//template <typename T>
		//T Call(std::vector<Variable>& args, Variable* def) { return {}; }
	public:
		//using Procedure = RuntimeVariable(*)();
		//virtual RuntimeVariable Invoke_(Variable* tar, std::vector<RuntimeVariable>& args, RuntimeVariable* def, IRuntime* runtime) = 0;
	INTERNAL:
		//This is internal only so I'll just make runtime the thing that's used here. 
		// IRuntime should be able to transfer into runtime directly anyhow.
		virtual RuntimeVariable Execute(std::vector<RuntimeVariable>& args, Runtime* runtime, RuntimeVariable* def) = 0;

	
		
	public:
		//Call and Invoke may just return a variable instead.

		//Invoke is to be the function that handles external function calls, doing conversions before hand. 
		// internally, it just calls execute. later, it will call execute after converting and handling some checks.
		virtual RuntimeVariable Invoke(std::vector<RuntimeVariable>& args, Variable* def);


	

		

		template<std::convertible_to<RuntimeVariable>... Ts>
		RuntimeVariable Call(Ts&&... args)
		{
			std::vector<RuntimeVariable> t_args{ args... };
			auto value = Execute(t_args, nullptr, nullptr);
			return value;
		}


		template<std::convertible_to<RuntimeVariable>... Ts>
		RuntimeVariable Call(Default def, Ts&&... args)
		{
			std::vector<RuntimeVariable> t_args{ args... };
			auto value = Execute(t_args, &def.value);
			return value;
		}

		//Need src
		//RuntimeVariable Execute(std::vector<RuntimeVariable> args) { return Execute(args, nullptr); }

	};


}