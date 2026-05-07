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


		case InfoType::ThisInfo:
		case InfoType::ParameterInfo:
			is_loc = false;
			[[fallthrough]];
		case InfoType::LocalInfo: {
			IVarIndexInfo* loc = static_cast<IVarIndexInfo*>(info);

			QualifiedType type{ loc->GetType(), loc->GetQualifiers() };
			OperandType op;
			Index index;


			if (type.IsVariadic())
			{
				auto buffer = compiler->GetScope()->ObtainLocalVariable(parse_strings::variadic_buffer);
				op = OperandType::Variable | OperandType::VariableIndex;
				index = buffer->GetIndex();

			}
			else
			{
				op = is_loc ? OperandType::Variable : OperandType::Parameter;
				index = loc->GetIndex();
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