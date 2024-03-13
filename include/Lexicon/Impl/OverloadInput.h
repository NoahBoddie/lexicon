#pragma once


#include "BasicQualifier.h"
#include "RuntimeQualifier.h"

#include "OverloadKey.h"

#include "Solution.h"

namespace LEX
{
	struct ITypePolicy;
	struct TargetObject;

	//The stand alone class. I have this because 
	class OverloadInput : public OverloadKey
	{
	public:
		//This isn't quite right, the they should share something that tells you if they're const or not.
		// Some way to get these sort of basic/runtime flags

		//Simple way would probably be to have a function do it like Field.
		// Within input, it stores a target object, so no issues getting the flags there.
		// within a function policy, it's pretty easy to just make them on the spot.

		//TODO: Give OverloadInput a fucking constructor, please and thank you

		BasicQualifier GetBasicFlags() const override;

		RuntimeQualifier GetRuntimeFlags() const override;


		virtual std::vector<ITypePolicy*> GetGenericInput() override
		{
			return {};//return genericInput;
		}
		virtual std::vector<DefaultGenericInput> GetDefaultGenericInput() override
		{
			return {};
		}

		ITypePolicy* GetTarget() override;

		virtual std::vector<ParamInput> GetParamInput() override
		{
			return  std::vector<ParamInput>{ paramInput.begin(), paramInput.end() };
		}

		virtual std::vector<DefaultParamInput> GetDefaultParamInput() override
		{
			return {};
		}

		//private://it SHOULD be private but I'm lazy and didn't make a constructor

		TargetObject* object = nullptr;
		//std::vector<ITypePolicy*>			genericInput;
		std::vector<Solution>				paramInput;
		//std::map<std::string, Solution>		defaultInput;//rename
	};

}