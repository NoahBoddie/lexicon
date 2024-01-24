#pragma once

#include "FunctionBase.h"
#include "AbstractFunction.h"

namespace LEX
{
	class RuntimeVariable;

	class ConcreteFunction : public FunctionBase, public AbstractFunction
	{
	public:

		//void LoadFromRecord(Record&)

		bool IsAbstract() const override { return false; }


		virtual RuntimeVariable _Invoke(std::vector<RuntimeVariable> args, RuntimeVariable* def) = 0;


		RuntimeVariable _Invoke(std::vector<RuntimeVariable> args) { return _Invoke(args, nullptr); }
		virtual RuntimeVariable Invoke(std::vector<RuntimeVariable> args, RuntimeVariable* def) = 0;


		RuntimeVariable Invoke(std::vector<RuntimeVariable>& args, RuntimeVariable*) override
		{
			if (auto routine = GetRoutine(); routine)
			{
				//TODO: make this use the actual function pls
				LEX::Runtime runtime{ *routine, args };

				ret = runtime.Run();
			}



		}

	};

	using Function = ConcreteFunction;
}
