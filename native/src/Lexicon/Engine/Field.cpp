#include "Lexicon/Engine/Field.h"

#include "Lexicon/Engine/QualifiedType.h"
namespace LEX
{
	QualifiedType Field::GetQualifiedType() const
	{
		QualifiedType result{};

		
		result.policy = GetType();

		result = GetQualifiers();

		return result;
	}


	QualifiedType Field::FetchQualifiedType() const
	{
		if (!this)
			return {};

		return GetQualifiedType();
	}

}