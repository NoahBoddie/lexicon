#include "Lexicon/Interfaces/Script.h"
#include "Lexicon/Interfaces/Element.h"
#include "Lexicon/Interfaces/ProjectManager.h"

namespace LEX
{
	Project* Element::GetShared()
	{
		//Actually supposed to search for the project manager
		return ProjectManager::GetSharedProject();
	}
	
	Script* Element::GetCommons() { return GetScript()->FetchCommons(); }




	Element* SecondaryElement::GetParent()
	{
		return _parent;
	}

	void SecondaryElement::SetParent(Element* par)
	{
		if (_parent)
			return;

		Environment* env = dynamic_cast<Environment*>(par);

		if (!env) {
			report::compile::critical("parent set for Element was not an environment.");
		}

		_parent = env;
	}


}