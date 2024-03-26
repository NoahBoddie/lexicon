#pragma once

#include "Scope.h"

#include "Environment.h"

#include "OverloadInput.h"

#include "DeclareHeader.h"
namespace LEX
{

	Field* Scope::SearchField(std::string name, OverloadKey& key, FieldPredicate pred)
	{
		//Move to the compiler maybe?
		auto end = vars.end();

		if (auto it = vars.find(name); it != end) {
			return &it->second;
		}
		else if (parent) {
			return parent->SearchField(name, key, pred);
		}
		else if (auto field = process->GetEnvironment()->SearchField(name, key, pred); field) {
			return field;


		}

		return nullptr;
	}

	Field* Scope::SearchField(std::string name, FieldPredicate pred)
	{
		OverloadInput input;

		return SearchField(name, input, pred);
	}

	ITypePolicy* Scope::SearchType(std::string name)
	{
		//I'm too fucking lazy to make it work normally, so this is what we're gonna deal with til I do.
		
		Record dummy{ "dummy", 0, Record{name} };

		auto result = GetPolicyFromSpecifiers(dummy, process->GetEnvironment());

		return result;
	}


	std::string Scope::name()
	{
		return process->_targetFunc->_name;
	}
}