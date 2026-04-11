#pragma once

#include "Lexicon/IFormula.h"

#include "Lexicon/Engine/FunctionData.h"
#include "Lexicon/Engine/Component.h"

//*src
#include "Lexicon/Engine/Runtime.h"

namespace LEX
{

	class BasicFormula : public Component, public IFormula, public BasicCallableData
	{
	public:
		DEFINE_COMPONENT_OFFSET(ComponentType::BasicFormula)
	private:
		//This version is obscured for the user. It should help inline functions and such into the code, or that can be given to others to run.
		//Formula rules. 
		// They don't have default parameters, 
		// they don't have procedures. 
		// They don't have names. Rather they have handles and hashes.
		// They don't have targets (SCRATCH, they now have targets)

		//Think this should probably store the string that it came from perhaps. Could be useful.
		
		const Component* GetComponent() const { return this; }

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

		mutable std::atomic_int32_t refCount = 1;


		void ModRefCountImpl(bool inc) const noexcept override
		{
			bool destroy = !(refCount += inc ? 1 : -1);

			if (destroy) {
				delete this;
			}

		}

		RuntimeVariable Execute(std::span<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def) override
		{
			return BasicExecute(nullptr, nullptr, args, caller, def);
		}


		RuntimeVariable Invoke(std::span<RuntimeVariable> args, RuntimeVariable* def) override
		{
			return BasicInvoke(nullptr, nullptr, args, def);
		}
	};
}