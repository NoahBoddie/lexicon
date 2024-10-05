#pragma once

#include "Lexicon/Engine/Scope.h"

#include "Lexicon/Engine/Environment.h"
#include "Lexicon/Engine/OverloadInput.h"
#include "Lexicon/Engine/Declaration.h"
#include "Lexicon/Engine/PolicyBase.h"


namespace LEX
{
	PolicyBase* Scope::SearchTypePath(Record& _path)
	{
		Record dummy{ "dummy", 0, _path };

		//I'm too fucking lazy to make it work normally, so this is what we're gonna deal with til I do.
		//PolicyBase* result = GetPolicyFromSpecifiers(_path, process->GetEnvironment());
		PolicyBase* result = GetPolicyFromSpecifiers(dummy, process->GetEnvironment());

		return result;

	}
	

	//This actually probably will want to return the function info, because of something like generic functions.
	FunctionInfo* Scope::SearchFunctionPath(Record& path, OverloadKey& input, Overload& out)
	{
		return process->GetEnvironment()->SearchFunctionPath(path, input, out);

	}

	QualifiedField Scope::SearchFieldPath(Record& _path, OverloadKey* key)
	{


		//Move to the compiler maybe?
		auto& name = _path.GetTag();

		if (_path.SYNTAX().type != SyntaxType::Unary && _path.SYNTAX().type != SyntaxType::Binary && name != "::")
		{

			auto end = vars.end();

			if (auto it = vars.find(name); it != end) {
				return it->second;
			}
			else if (_parent) {
				return _parent->SearchFieldPath(_path, key);
			}

			if (ParameterInfo* field = process->FindParameter(name); field) {
				return field;
			}
		}


		if (auto field = process->GetEnvironment()->SearchFieldPath(_path); field) {
			return field;
		}




		return nullptr;
	}



}