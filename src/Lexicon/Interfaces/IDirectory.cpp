#include "Lexicon/Interfaces/IDirectory.h"

#include "Lexicon/Engine/SyntaxRecord.h"
#include "Lexicon/Engine/Directory.h"
namespace LEX
{
	IDirectory* IDirectory::FindDirectory(SyntaxRecord& record)
	{
		return GetAs<Directory>()->FindDirectory(record);
	}

	//std::vector<IDirectory*> IDirectory::GetAssociates(RelateType relate)
	//{
	//	return GetAs<Directory>()->GetAssociates(relate);
	//}
}