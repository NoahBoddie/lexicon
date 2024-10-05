#include "Lexicon/Object.h"

#include "Lexicon/String.h"



namespace LEX
{

	std::string Object::PrintString() const
	{
		return policy->PrintString(data(), {});
	}

}