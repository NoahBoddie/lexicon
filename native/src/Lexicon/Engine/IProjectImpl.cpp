#include "Lexicon/Engine/IProjectImpl.h"

#include "Lexicon/Engine/Script.h"


namespace LEX
{
	IScript* IProjectImpl::GetCommons()
	{
		return GetCommons({});
	}
}