#pragma once

#include "Lexicon/IFormula.h"

#include "Lexicon/Engine/FunctionData.h"


//*src
#include "Lexicon/Engine/Runtime.h"

namespace LEX
{

	class BasicFormula : public IFormula, public BasicCallableData
	{
		//This version is obscured for the user. It should help inline functions and such into the code, or that can be given to others to run.
		//Formula rules. 
		// They don't have default parameters, 
		// they don't have procedures. 
		// They don't have names. Rather they have handles and hashes.
		// They don't have targets (SCRATCH, they now have targets)

		//Think this should probably store the string that it came from perhaps. Could be useful.
		
	public:

		std::string_view GetName() const
		{
			return text;
		}
		
		
		virtual void SetName(const std::string_view& name) override
		{
			text = name;
			_routine.name = text.data();
		}

		void SetFile(const char* file)
		{
			_routine.file = file;
		}

		std::string text;
		
		SyntaxRecord records;

		RuntimeVariable Execute(std::span<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def) override
		{
			return BasicExecute(nullptr, nullptr, args, caller, def);
		}
	};

}