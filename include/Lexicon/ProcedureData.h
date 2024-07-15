#pragma once

namespace LEX
{
	struct IRuntime;
	struct RuntimeVariable;
	struct AbstractFunction;

	struct ProcedureData
	{
		IRuntime* runtime = nullptr;
		RuntimeVariable* defOption = nullptr;
		const AbstractFunction* function = nullptr;
	};
}