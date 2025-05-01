#include "Lexicon/Engine/QualifiedField.h"
#include "Lexicon/Engine/Solution.h"
#include "Lexicon/Engine/ConcreteGlobal.h"
namespace LEX
{
	Solution QualifiedField::AsSolution()
	{
		switch (GetFieldType())
		{
		case FieldType::Parameter:
		case FieldType::Local:
			return Solution{ QualifiedType{ GetType(), GetQualifiers() }, OperandType::Index, (size_t)GetFieldIndex()};


		case FieldType::Global: {
			IGlobal* glob = static_cast<GlobalBase*>(_target)->AsGlobal();
			

			Solution result{ QualifiedType{ GetType(), GetQualifiers() }, OperandType::Global, glob };

			return result;
		}
		
		case FieldType::Member:
		case FieldType::Function:
		
		default:
			report::compile::error("cannot handle this type at this time");
			return {};
		}


	}
}