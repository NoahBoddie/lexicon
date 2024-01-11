#include "Lexicon/Impl/Element.h"
#include "Lexicon/Impl/ProjectManager.h"

namespace LEX
{
	Project* Element::GetShared()
	{
		//Actually supposed to search for the project manager
		return ProjectManager::GetSharedProject();
	}
}