#pragma once

#include "Environment.h"

//*src
#include "Function.h"
#include "GlobalVariable.h"

#include "Project.h"

namespace LEX
{
	void Environment::SetParent(Element* par)
	{
		if (_parent)
			return;
		//I'd like something easier for this
		//Perhaps fetch project and 

		//Environment* env = dynamic_cast<Environment*>(par);
		//Project* proj = dynamic_cast<Project*>(par);

		if (par->FetchProject() != par && par->FetchEnvironment() != par) {
			RGL_LOG(critical, "parent set for Element was not an environment.");
			throw nullptr;
		}
		
		_parent = par;
	}
}
