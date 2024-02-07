#pragma once

#include "Scope.h"

#include "Environment.h"


namespace LEX
{

	Field* Scope::GetField(std::string name, ITypePolicy* p)
	{
		//Move to the compiler maybe?
		auto end = vars.end();

		if (auto it = vars.find(name); it != end) {
			return &it->second;
		}
		else if (parent) {
			return parent->GetField(name, p);
		}
		else if (auto field = process->GetEnvironment()->TEMPSearchField(name); field) {
			return field;


		}

		return nullptr;
	}
}