#include "Lexicon/Impl/Script.h"
#include "Lexicon/Impl/Project.h"

namespace LEX
{

	Script* Script::GetCommons()
	{
		//This 
		return _parent->FetchCommons();
	}

	void Script::SetParent(Element* env)
	{
		//TODO: Script::SetParent is actually supposed to ask if new parent isn't a project. Attend to that when enum is added.
		//TODO: EnvironmentError in this situation is exclusively an error on my part, need a new exception for that.
		if (env->IsComponentType<Project>() == false)
			throw EnvironmentError("Parent of script must be a project.");

		_parent = env->GetProject();
	}
}