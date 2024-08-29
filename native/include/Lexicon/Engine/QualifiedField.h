#pragma once
#include "Lexicon/Field.h"

#include "Lexicon/Qualifier.h"


//*src

#include "Lexicon/Engine/InfoBase.h"
#include "Lexicon/QualifiedType.h"
#include "Lexicon/DeclareSpecifier.h"


namespace LEX
{
	struct Solution;

	struct QualifiedField : public Field
	{
		enum struct Constness : uint8_t
		{
			kNone,
			kConst,
			kReadonly,
		};

		enum struct RefrType : uint8_t
		{
			kNone,
			kLeft,
			kRight,
		};

		//The qualified type could be anything qualifiable, but type seems to be the most likely to pull (well solution, but solutions are type qualified).
		QualifiedField(Field* field, QualifiedType type = nullptr) : _target{ field }
		{
			//By this point, it's expected that the calling has already emplaced it's rules on what this is.

			if (type)
			{
				auto qualifiers = field->GetQualifiers();

				switch (type.flags & Qualifier::Constness_)
				{
				case Qualifier::Const:
					//If the field is mutable or readonly, it cannot be made into const.
					if (FilterEquals<Qualifier::Constness_>(qualifiers, Qualifier::Modable) == false)
						_constLevel = Constness::kConst; break;

				case Qualifier::Readonly:
					_constLevel = Constness::kReadonly; break;
				}

				//If the field has a reference type, that is uses instead.
				if (!!(qualifiers & Qualifier::Reference_))
				{
					switch (type.flags & Qualifier::Reference_)
					{
					case Qualifier::RefL:
						_refLevel = RefrType::kLeft; break;

					case Qualifier::RefR:
						_refLevel = RefrType::kRight; break;
					}
				}
			}
		}

		QualifiedField(Field& field, QualifiedType type = nullptr) : QualifiedField{ &field, type }
		{			
		
		}
		


		Field* _target = nullptr;

		Constness _constLevel = Constness::kNone;

		//Thi
		RefrType _refLevel = RefrType::kNone;



		uint32_t GetFieldIndex() const override
		{
			return _target->GetFieldIndex();
		}

		FieldType GetFieldType() const override
		{
			return _target->GetFieldType();
		}

		ITypePolicy* GetType() const override
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


			if (_constLevel != Constness::kNone)
			{
				result &= ~Qualifier::Constness_;

				switch (_constLevel)
				{
				case Constness::kConst:
					result |= Qualifier::Const; break;

				case Constness::kReadonly:
					result |= Qualifier::Readonly; break;
				}

			}


			if (_refLevel != RefrType::kNone)
			{
				result &= ~Qualifier::Reference_;

				switch (_refLevel)
				{
				case RefrType::kLeft:
					result |= Qualifier::RefL; break;

				case RefrType::kRight:
					result |= Qualifier::RefR; break;
				}

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