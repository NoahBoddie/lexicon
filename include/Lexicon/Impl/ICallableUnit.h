#pragma once


namespace LEX
{
	struct RoutineBase;
	class RuntimeVariable;

	struct ICallableUnit
	{
		//This is where all the things about something that can simply just be called goes.

		//Should this have the ability to get function data or something?

		//This needs to demand definition
		virtual RoutineBase* GetRoutine() { return nullptr; }
		
		//resolves arguments and confirms they are as they should be. Failure results in an application error.
		virtual void ResolveArguments(std::vector<RuntimeVariable>&) = 0;
	};

}