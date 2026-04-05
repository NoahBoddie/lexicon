#include "Lexicon/Engine/ProjectDirectory.h"


#include "Lexicon/Engine/Project.h"

namespace LEX
{


	Project* ProjectDirectory::FindProject(const std::string_view& name)
	{
		if (name == "Shared") {
			return GetShared();
		}

		auto end = _projects.end();


		//Proper version of Script not implement
		auto it = std::find_if(_projects.begin(), end, [&](Project* search) { return search->GetName() == name; });

		if (it != end) {
			return *it;
		}

		return nullptr;
	}
}