#include "Lexicon/Engine/Operand.h"

#include "Lexicon/Engine/Target.h"
#include "Lexicon/Engine/Register.h"
#include "Lexicon/Engine/OperandType.h"


#include "Lexicon/Engine/Runtime.h"
#include "Lexicon/RuntimeVariable.h"

#include "Lexicon/Engine/Parser.h"

#include "Lexicon/Engine/GlobalBase.h"

namespace LEX
{


	Index Operand::GetIndex(Runtime* runtime)
	{
		switch (type())
		{
		case OperandType::Value: {
			constexpr auto settings = Number::Settings::CreateFromType<Index>();

			auto& var = runtime->GetVariable(Get<Index>(), true);
			//TODO: ensure the settings match, no conversions. We must have exactness
			//Rather than ensure, maybe make a setting for it.

			return var->AsNumber().Convert(settings);
		}


		default:
			return Get<Index>();

		}

		report::runtime::critical("Operand cannot return GetIndex. Invalid operand type detected. {} ", magic_enum::enum_name(type()));
		return {};
	}

	Differ Operand::GetDiffer(Runtime* runtime)
	{
		switch (type())
		{
		case OperandType::Value: {
			constexpr auto settings = Number::Settings::CreateFromType<Differ>();

			auto& var = runtime->GetVariable(Get<Index>(), true);

			return var->AsNumber().Convert(settings);
		}


		default:
			return Get<Differ>();

		}

		report::runtime::critical("Operand cannot return GetDiffer. Invalid operand type detected. {} ", magic_enum::enum_name(type()));
		return {};
	}

	Number Operand::GetConstant()
	{
		//A value is basically a variable, but can ex
		switch (type())
		{
		case OperandType::Differ:
			return Get<Differ>();

		case OperandType::Index:
			return Get<Index>();

		}

		report::runtime::critical("Operand cannot return GetConstant. Invalid Value type detected. {} ", magic_enum::enum_name(type()));
		return {};
	}

	RuntimeVariable& Operand::AsValue(Runtime* runtime)
	{
		//A value is basically a variable, but can ex
		switch (type())
		{
		case OperandType::Value:
			return runtime->GetVariable(GetIndex(runtime), true);
		}

		report::runtime::critical("Operand cannot return AsValue. Invalid Value type detected. {} ", magic_enum::enum_name(type()));
	}

	RuntimeVariable Operand::GetValue(Runtime* runtime)
	{
		switch (type())
		{
		case OperandType::Value:
			return AsValue(runtime);
		
		case OperandType::Differ:
		case OperandType::Index:
			return GetConstant();
		}

		report::runtime::critical("Operand cannot return GetValue. Invalid Value type detected. {} ", magic_enum::enum_name(type()));

	}

	




	RuntimeVariable& Operand::CheckDynamicSubject(Runtime* runtime, RuntimeVariable& subject)
	{
		auto type = GetDynamicIndex();

		if (type)
		{
			constexpr auto settings = Number::Settings::CreateFromType<Index>();

			Index index = subject->AsNumber().Convert(settings);

			switch (type)
			{
			case OperandType::ArgumentIndex:
				return runtime->GetArgumentFromBack(index);

			case OperandType::ParameterIndex:
			case OperandType::VariableIndex:
				//With this, I'd like negative 1 to be something used to represent that I want to pick the "index - 1", or the last value.
				return runtime->GetVariable(index, type == OperandType::VariableIndex);

			default:

				report::compile::critical("Something something, bad type {}", magic_enum::enum_name(type));
			}
		}


		return subject;
	}



	Variable Operand::CopyVariable(Runtime* runtime)
	{
		//This merely takes the runtime variable and removes the reference part

		return GetVariable(runtime);
	}


	RuntimeVariable& Operand::AsSubject(Runtime* runtime)
	{
		switch (type())
		{
		case OperandType::Register:
			return CheckDynamicSubject(runtime, runtime->GetRegister(Get<Register>()));

		case OperandType::Argument:
			return CheckDynamicSubject(runtime, runtime->GetArgumentFromBack(Get<Index>()));

		case OperandType::Parameter:
		case OperandType::Variable:
		case OperandType::Value:
			//With this, I'd like negative 1 to be something used to represent that I want to pick the "index - 1", or the last value.
			return CheckDynamicSubject(runtime, runtime->GetVariable(Get<Index>(), !IsParameter()));


		}

		report::runtime::critical("Operand Cannot return AsSubject. No reference type detected. {} ", magic_enum::enum_name(type()));
		throw nullptr;//Error.
	}


	RuntimeVariable Operand::GetSubject(Runtime* runtime)
	{
		switch (type())
		{
		case OperandType::Register:
		case OperandType::Argument:
		case OperandType::Value:
		case OperandType::Variable:
		case OperandType::Parameter:
			return AsVariable(runtime).AsRef();
			return AsSubject(runtime).AsRef();
		case OperandType::Global: {
			//-1 should mean the default target.
			auto buff = Get<IGlobal*>();
			auto global = buff->GetGlobal(runtime);
			return global->GetReference();
		}



		case OperandType::Type: {
			auto type = Get<ITypeInfo*>();
			return type->GetTypeInfo(runtime);
		}

		case OperandType::Function: {
			//auto func = Get<IFunction*>();
			//return func->GetFunction(runtime);
		}


								  //Useless without context.
		case OperandType::Member:
		case OperandType::Review:
			break;
		case OperandType::Literal:
			return Get<LiteralPtr>()->GetVariable();

		default:
			//something.
			break;
		}

		report::runtime::critical("Operand didn't exist. Fixer later. {} ", magic_enum::enum_name(type()));
	}

	RuntimeVariable& Operand::AsVariable(Runtime* runtime)
	{
		//Make Use AsSubject
		switch (type())
		{
		case OperandType::Register:
			return CheckDynamicSubject(runtime, runtime->GetRegister(Get<Register>()));

		case OperandType::Argument:
			return CheckDynamicSubject(runtime, runtime->GetArgumentFromBack(Get<Index>()));

		case OperandType::Parameter:
		case OperandType::Variable:
			//With this, I'd like negative 1 to be something used to represent that I want to pick the "index - 1", or the last value.
			return CheckDynamicSubject(runtime, runtime->GetVariable(Get<Index>(), IsVariable()));
	
			
		}

		report::runtime::critical("Operand Cannot return AsVariable. No reference type detected. {} ", magic_enum::enum_name(type()));
		throw nullptr;//Error.
	}


	RuntimeVariable Operand::GetVariable(Runtime* runtime)
	{
		//This is what an operand uses in order to instantiate itself theoretically, however some operations can't really do that depending. I think this
		// should be someting that's operation uses

		//Like, operations on something like a type or a function. And real talk, it'd be a waste of space to make it done via variable there. A complete waste.
		// So this needs to be able to filter out when something has broken down. Or perhaps it states that the follo


		switch (type())
		{
		case OperandType::Register:
		case OperandType::Argument:
		case OperandType::Variable:
		case OperandType::Parameter:
		case OperandType::Global:
		case OperandType::Type:
		case OperandType::Function:		
		case OperandType::Literal:
			return GetSubject(runtime);
		case OperandType::Member:
		case OperandType::Review:
			break;
		default:
			//something.
			break;
		}

		report::runtime::critical("Operand didn't exist. Fixer later. {} ", magic_enum::enum_name(type()));
		throw nullptr;//Error.

		//*/
	}


	RuntimeVariable& Operand::ObtainAsVariable(Runtime* runtime)
	{
		//For the ones that it's relevant to do so, it confirms that the variable truly exists. If not, it instantiates it.
		// <!>not valid on all operands. Cannot resolve unexpected operand types.
		RuntimeVariable* run_var = nullptr;


		RuntimeVariable& result = AsVariable(runtime);

		if (result.IsEmpty() == true) {
			result = Variable{};
		}

		return result;
	}

	RuntimeVariable Operand::ObtainVariable(Runtime* runtime)
	{
		//For the ones that it's relevant to do so, it confirms that the variable truly exists. If not, it instantiates it.
		// <!>not valid on all operands. Cannot resolve unexpected operand types.
		RuntimeVariable* run_var = nullptr;

		switch (type())
		{
		case OperandType::Register:
		case OperandType::Parameter:
		case OperandType::Variable:
		case OperandType::Argument:
			run_var = std::addressof(AsVariable(runtime));
			break;

		}


		if (run_var && run_var->IsEmpty() == true) {
			RuntimeVariable& set = *run_var;
			
			set = Variable{};
		}

		return GetVariable(runtime);
	}


	TypeInfo* Operand::GetTypeInfo(Runtime* runtime)
	{
		switch (type())
		{
		case OperandType::Type:
			return NULL_OP(NULL_Q(Get<ITypeInfo*>())->GetTypeInfo(runtime));

		default:
			report::fault::critical("Operand didn't exist. Fixer later. {} ", magic_enum::enum_name(type()));
		}

		return nullptr;
	}
	


}