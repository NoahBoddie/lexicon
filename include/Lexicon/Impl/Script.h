#pragma once

//Temp
#include "Lexicon/Impl/TypeAliases.h"


#include "Lexicon/Impl/Environment.h"

namespace LEX
{
	class Construct;


	//TODO: Creation of static script evironments. You know commons. But more below
	// I want a different place where all objects are accounted for, but no objects will connect with one enough. The scrapheap?
	//  garbage collector? Whatever. The idea would be something along the lines where all the unaccounted for stuff ends up, so just in case, we have a little bit
	//  of fucking accountability.
	// In addition to this, I've presented the idea for "Core", this core environment is as you'd think unreachable by normal means, and basically is used
	// to pull custom
	struct Script : public Environment
	{
		inline static Script* commons = nullptr;

		//TODO: Scripts need something that says that it already imported included a thing before.
		// Which when doing so should also make it do it's own includes.
		// I may have includes just be a different section called includes. may allow for more descriptive
		// ambiguity errors.

		//Imports and includes go here, as well as properties and such. This is probably what you'd be thinking of when you use most things.


		//Should these own a variableID instead? It feels like extra steps...
		std::vector<GlobalVariable*> properties;
		std::vector<Prototype*> constructs;//responsible classes

		//I think imports and includes should go to the same place.
		std::vector<Script*> imports;




		void ImportScript(Script* script)
		{
			//Unable to fail
		}

		void IncludeScript(Script* script)
		{
			//Can possibly fail. If it does so, that's actually a compiling error and should throw as such.

			if (false) {
				//if already imported script.
				return;
			}
			for (auto prop : script->properties)
			{
				AddProperty(prop);
			}

			for (auto func : script->functions)
			{
				AddFunction(func);
			}


			for (auto proto : script->constructs)
			{
				AddConstruct(proto);
			}

			ImportScript(script);
		}

		FieldQuery GetFunction(std::string name, Function*& function, bool use_imports)
		{
			//TODO:Split Script::GetFunction into multiple functions, that search exclusively for different function types. More below.
			// _GetInternalFunction
			// _GetExtneralFunction?
			// _GetStaticFunction
			// _GetExtensionFunction
			if (this)
			{
				for (auto function : functions)
				{
					//do stuff
				}
			}

			if (use_imports)
			{
				Function* result = nullptr;

				for (auto imp : imports)
				{
					//do stuff

					if (result)
						return FieldQuery::Ambiguous;
				}
			}

			return FieldQuery::Failure;
		}


		FieldQuery GetProperty(std::string name, GlobalVariable*& global, bool use_imports)
		{
			//_GetInternalProperty
			//_GetExternalProperty
			if (this)
			{
				for (auto prop : properties)
				{
					//do stuff
				}
			}

			if (use_imports)
			{
				GlobalVariable* result = nullptr;

				for (auto imp : imports)
				{
					//do stuff

					if (result)
						return FieldQuery::Ambiguous;
				}
			}

			return FieldQuery::Failure;
		}

		FieldQuery GetStruct(std::string name, Prototype*& proto, bool use_imports)
		{
			//_GetInternalStruct
			//_GetExternalStruct
			if (this)
			{
				for (auto prop : properties)
				{
					//do stuff
				}
			}

			if (use_imports)
			{
				Construct* result = nullptr;

				for (auto imp : imports)
				{
					//do stuff

					if (result)
						return FieldQuery::Ambiguous;
				}
			}

			return FieldQuery::Failure;
		}




		void AddProperty(GlobalVariable* prop)
		{

		}

		void AddFunction(Coroutine* func)
		{

		}

		void AddConstruct(Prototype* proto)
		{

		}
	};
}