#include "Operand.h"

#include "Target.h"
#include "Lexicon/Impl/Register.h"
#include "Lexicon/Impl/OperandType.h"


#include "Runtime.h"
#include "RuntimeVariable.h"

#include "Parser.h"
namespace LEX
{
	Variable Operand::CopyVariable(Runtime* runtime)
	{
		//This merely takes the runtime variable and removes the reference part

		return GetVariable(runtime);
	}


	RuntimeVariable Operand::GetVariable(Runtime* runtime)
	{
		//This is what an operand uses in order to instantiate itself theoretically, however some operations can't really do that depending. I think this
		// should be someting that's operation uses

		//Like, operations on something like a type or a function. And real talk, it'd be a waste of space to make it done via variable there. A complete waste.
		// So this needs to be able to filter out when something has broken down. Or perhaps it states that the follo



		switch (type)
		{
		case OperandType::Register:
			return runtime->GetRegister(Get<Register>()).AsRef();

		case OperandType::Argument:
			return runtime->GetArgument(Get<Index>()).AsRef();

		case OperandType::Index:
			//With this, I'd like negative 1 to be something used to represent that I want to pick the "index - 1", or the last value.
			RGL_LOG(critical, "Index__1 {}", Get<Index>());
			return runtime->GetVariable(Get<Index>()).AsRef();

		case OperandType::Variable:
			//-1 should mean the default target.
		{
			auto& variable = *Get<Variable*>();
			return std::ref(variable);
		}
			


		case OperandType::Type:
		case OperandType::Function:
			//Unsure if I'mm going to handle these, but this might be somewhat worth considering.


		//Useless without context.
		case OperandType::Member:
		case OperandType::Review:
			break;
		case OperandType::Literal:
			return *Get<Literal>();

		default:
			//something.
			break;
		}

		RGL_LOG(critical, "Operand didn't exist. Fixer later. {} ", type);
		throw nullptr;//Error.

		//*/
	}


	RuntimeVariable Operand::ObtainVariable(Runtime* runtime)
	{
		//For the ones that it's relevant to do so, it confirms that the variable truly exists. If not, it instantiates it.
		// <!>not valid on all operands. Cannot resolve unexpected operand types.
		RuntimeVariable* run_var = nullptr;

		switch (type)
		{
		case OperandType::Register:
			run_var = std::addressof(runtime->GetRegister(Get<Register>()));
			break;

		case OperandType::Index:
			run_var = std::addressof(runtime->GetVariable(Get<Index>()));
			break;

		case OperandType::Argument:
			run_var = std::addressof(runtime->GetArgument(Get<Index>()));
			break;

		}

		if (run_var && run_var->IsVoid() == true) {
			*run_var = Variable{};
		}

		return GetVariable(runtime);
	}


}