#pragma once
#include "Lexicon/Engine/Field.h"

#include "Lexicon/Qualifier.h"


//*src

#include "Lexicon/Engine/InfoBase.h"
#include "Lexicon/Engine/QualifiedType.h"
#include "Lexicon/Specifier.h"


namespace LEX
{
	struct Solution;

	struct QualifiedField : public Field
	{


		//The qualified type could be anything qualifiable, but type seems to be the most likely to pull (well solution, but solutions are type qualified).
		QualifiedField(Field* field, QualifiedType type = nullptr) : _target{ field }
		{
			//By this point, it's expected that the calling has already emplaced it's rules on what this is.

			if (type)
			{
				auto qualifiers = field->GetQualifiers();
				_constState = qualifiers.constness;
				_refState = qualifiers.reference;

			}
		}

		QualifiedField(Field& field, QualifiedType type = nullptr) : QualifiedField{ &field, type }
		{			
		
		}
		


		Field* _target = nullptr;

		Constness _constState = Constness::Modable;

		//Thi
		Reference _refState = Reference::Temp;



		uint32_t GetFieldIndex() const override
		{
			return _target->GetFieldIndex();
		}

		FieldType GetFieldType() const override
		{
			return _target->GetFieldType();
		}

		AbstractType* GetType() const override
		{
			return _target->GetType();
		}

		Qualifier GetRawQualifiers() const
		{
			return _target->GetQualifiers();
		}

		Qualifier GetQualifiers() const override
		{
			auto result = GetRawQualifiers();


			if (result.GetConstNormalized() < _constState)
			{
				result.constness = _constState;
			}


			if (_refState != Reference::Temp)
			{
				result.reference = _refState;
			}


			return result;
		}
		Specifier GetSpecifiers() const override
		{
			return _target->GetSpecifiers();
		}

		std::string GetFieldName() const override
		{
			return _target->GetFieldName();
		}

		operator Field* () const
		{
			return _target;
		}

		Field* GetField() const
		{
			return _target;
		}

		Solution AsSolution();

		operator bool() const
		{
			return _target;
			//return _target && _target->operator bool();
		}
	};


}