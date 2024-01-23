#pragma once

#include "Element.h"
#include "AbstractFunction.h"

#include "VarInfo.h"
#include "Variable.h"
#include "RoutineBase.h"

//*src
#include "RuntimeVariable.h"

namespace LEX
{
	struct FunctionData
	{
		//weren't functions environments? A: They don't need to be anymore.
		//Data



		//These are FunctionData: Functions, GenericFunctions, and Formulas.
		// Yes, formulas are function data. I thought to make CallableUnit handle the setting of things,
		// but I realized that I want ICallableUnit to be an information and invoking interface, not one made
		// for setting features or flags.



		Variable _returnType;

		//using this raw, but at a later point it's the variable that holds the default value of the function, 
		// as well as it's type.
		ITypePolicy* temp_returnType;

		
		//The actual object for this has severely changed. It's "ParamInfo" I think? But this isn't terribly important
		// here yet so you know.
		//std::vector<DeclaredParam> reqParams;
		//std::vector<DefinedParam> defParams;
		using Temp_Parameter = void*;
		
		std::vector<ParamInfo> parameters;

		size_t defaultIndex = max_value<size_t>;//basically whenever the defaults start.
		
		RoutineBase _routine;//actually needs to be a pointer


		RoutineBase* GetRoutine()
		{
			//This plans to be a pointer later, as this will end up just being 
			return &_routine;
		}


		std::vector<ParamInfo> GetParameters()
		{
			return parameters;
		}


		//Needs to be moved into ITypePolicy/AbstractTypePolicy.
		ITypePolicy* GetReturnType()
		{
			return temp_returnType;
		}
		
		//AbstractTypePolicy* GetConcreteReturnType();//move to abstractFunction


		//Possible use in deductions with generics, then I realized this isn't C++ and auto cannot exactly exist
		// like I think it would.
		//void SetReturnType(ITypePolicy*);
	};


	class FunctionBase : public virtual IFunction, public SecondaryElement, public FunctionData
	{
		//This is a pivot for for functions, more important than anywhere else, this set up excludes formulas
		// from being able to be stored in a function, or having the same linking



	public:
		std::string GetName()
		{
			RGL_LOG(critical, "this shit not supposed to be used fam");
			throw nullptr;
		}
	};



	class Function : public FunctionBase, public AbstractFunction
	{
	public:

		//void LoadFromRecord(Record&)

		bool IsAbstract() const override { return false; }


		virtual RuntimeVariable _Invoke(std::vector<RuntimeVariable> args, RuntimeVariable* def) = 0;


		RuntimeVariable _Invoke(std::vector<RuntimeVariable> args) { return _Invoke(args, nullptr); }



		void Invoke(std::vector<RuntimeVariable>& args, RuntimeVariable*) override
		{
			if (auto routine = GetRoutine(); routine)
			{
				//TODO: make this use the actual function pls
				LEX::Runtime runtime{ *routine, args };

				ret = runtime.Run();
			}



		}

	};
	
}