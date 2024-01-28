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


		RuntimeVariable Invoke(std::vector<RuntimeVariable>& args, RuntimeVariable*) override
		{
			/*

			if (auto routine = GetRoutine(); routine)
			{
				//TODO: make this use the actual function pls
				LEX::Runtime runtime{ *routine, args };

				ret = runtime.Run();
			}

			//*/
			return {};
		}

	};

	using Function = ConcreteFunction;
}
