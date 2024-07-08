#include "Lexicon/Field.h"

#include "Lexicon/Engine/Solution.h"
#include "Lexicon/QualifiedType.h"
namespace LEX
{
	QualifiedType Field::GetQualifiedType() const
	{
		QualifiedType result{};

		
		result.policy = GetType();

		result.flags = GetQualifiers();

		return result;
	}


	QualifiedType Field::FetchQualifiedType() const
	{
		if (!this)
			return {};

		return GetQualifiedType();
	}

}