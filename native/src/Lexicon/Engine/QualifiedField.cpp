#include "Lexicon/Engine/QualifiedField.h"
#include "Lexicon/Engine/Solution.h"
#include "Lexicon/Engine/ConcreteGlobal.h"
#include "Lexicon/Engine/RoutineCompiler.h"
#include "Lexicon/Engine/Scope.h"
namespace LEX
{

	Solution QualifiedField::AsSolution(ExpressionCompiler* compiler)
	{
		VarInfo* info = _target;
		bool is_loc = true;

		switch (info->GetInfoType())
		{



		case InfoType::ParameterInfo:
			is_loc = false;
			[[fallthrough]];
		case InfoType::LocalInfo: {
			LocalInfo* loc = info->As<LocalInfo>();

			QualifiedType type{ loc->type, loc->qualifiers };
			OperandType op;
			Index index;


			if (type.IsVariadic())
			{
				auto buffer = compiler->GetScope()->ObtainLocalVariable(parse_strings::variadic_buffer);
				op = OperandType::Variable | OperandType::VariableIndex;
				index = buffer->index;

			}
			else
			{
				op = is_loc ? OperandType::Variable : OperandType::Parameter;
				index = loc->index;
			}

			return Solution{ type, op, index };
		}

		case InfoType::GlobalBase: {
			GlobalBase* glob = info->As<GlobalBase>();

			Solution result{ QualifiedType{ glob->GetType(), glob->AsInfo()->GetQualifiers() }, OperandType::Global, glob->AsGlobal() };

			return result;
		}

								 //case FieldType::Member:
								 //case FieldType::Function:

		default:
			report::compile::error("cannot handle this type at this time");
			return {};
		}
	}
}