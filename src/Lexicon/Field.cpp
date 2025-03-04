#include "Lexicon/Field.h"

#include "Lexicon/QualifiedType.h"
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