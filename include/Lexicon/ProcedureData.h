#pragma once

namespace LEX
{
	struct IRuntime;
	struct RuntimeVariable;
	struct Function;

	struct ProcedureData
	{
		IRuntime* runtime = nullptr;
		RuntimeVariable* defOption = nullptr;
		const Function* function = nullptr;
	};
}