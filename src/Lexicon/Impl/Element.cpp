#include "Lexicon/Impl/Script.h"
#include "Lexicon/Impl/Element.h"
#include "Lexicon/Impl/ProjectManager.h"

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
			RGL_LOG(critical, "parent set for Element was not an environment.");
			throw nullptr;
		}

		_parent = env;
	}


}