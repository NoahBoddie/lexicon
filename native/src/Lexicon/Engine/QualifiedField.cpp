#include "Lexicon/Engine/QualifiedField.h"
#include "Lexicon/Engine/Solution.h"
#include "Lexicon/Engine/ConcreteGlobal.h"
#include "Lexicon/Engine/RoutineCompiler.h"
#include "Lexicon/Engine/Scope.h"
namespace LEX
{
	Solution QualifiedField::AsSolution(ExpressionCompiler* compiler)
	{
		bool is_loc = true;

		switch (GetFieldType())
		{
		case FieldType::Parameter:
			is_loc = false;
			[[fallthrough]];
		case FieldType::Local: {
			QualifiedType type{ GetType(), GetQualifiers() };
			OperandType op;
			Index index;
			
			
			if (type.IsVariadic())
			{
				auto buffer = compiler->GetScope()->ObtainLocalVariable(parse_strings::variadic_buffer);
				op = OperandType::Variable | OperandType::VariableIndex;
				index = buffer->GetFieldIndex();
				
			}
			else
			{
				op = is_loc ? OperandType::Variable : OperandType::Parameter;
				index = GetFieldIndex();
			}

			return Solution{ type, op, index };
		}

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