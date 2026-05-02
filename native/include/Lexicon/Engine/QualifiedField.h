#pragma once
#include "Lexicon/Engine/Field.h"
#include "Lexicon/Interfaces/VarInfo.h"

#include "Lexicon/Qualifier.h"


//*src

#include "Lexicon/Engine/InfoBase.h"
#include "Lexicon/Engine/QualifiedType.h"
#include "Lexicon/Specifier.h"


namespace LEX
{
	struct Solution;
	struct ExpressionCompiler;

	struct QualifiedField : public VarInfo
	{


		//The qualified type could be anything qualifiable, but type seems to be the most likely to pull (well solution, but solutions are type qualified).
		QualifiedField(VarInfo* info, QualifiedType type = nullptr) : _target{ info }
		{
			//By this point, it's expected that the calling has already emplaced it's rules on what this is.

			if (type)
			{
				auto qualifiers = info->GetQualifiers();
				_constState = qualifiers.constness;
				_refState = qualifiers.reference;

			}
		}

		QualifiedField(VarInfo& info, QualifiedType type = nullptr) : QualifiedField{ &info, type }
		{			
		
		}
		


		VarInfo* _target = nullptr;

		Constness _constState = Constness::Modable;

		//Thi
		Refness _refState = Refness::Temp;



		std::string_view GetName() const override
		{
			return _target->GetName();
		}

		ITypeInfo* GetType() const override
		{
			return _target->GetType();
		}


		Refness GetRefness() override
		{
			if (_refState != Refness::Temp)
			{
				return _refState;
			}

			return _target->GetRefness();
		}
		Constness GetConstness() override
		{
			auto constness = _target->GetConstness();

			if (constness < _constState) {
				constness = _constState;
			}

			return constness;
		}
		QualifierFlag GetQualifierFlags() override
		{
			return _target->GetQualifierFlags();
		}



		operator VarInfo* () const
		{
			return _target;
		}

		VarInfo* GetInfo() const
		{
			return _target;
		}

		Solution AsSolution(ExpressionCompiler* compiler);

		operator bool() const
		{
			return _target;
			//return _target && _target->operator bool();
		}
	};

	struct QualifiedVar;
}