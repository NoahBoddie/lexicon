#pragma once

#include "Operand.h"

#include "Target.h"
#include "Lexicon/Impl/Register.h"
#include "Lexicon/Impl/OperandType.h"


#include "Runtime.h"
#include "RuntimeVariable.h"

namespace LEX
{

	RuntimeVariable Operand::GetVariable(Runtime* runtime)
	{
		//This is what an operand uses in order to instantiate itself theoretically, however some operations can't really do that depending. I think this
		// should be someting that's operation uses

		//Like, operations on something like a type or a function. And real talk, it'd be a waste of space to make it done via variable there. A complete waste.
		// So this needs to be able to filter out when something has broken down. Or perhaps it states that the follo



		switch (type)
		{
		case OperandType::Register:
			return runtime->GetRegister(Get<Register>());

		case OperandType::Index:
			//With this, I'd like negative 1 to be something used to represent that I want to pick the "index - 1", or the last value.
			return runtime->GetArgument(Get<Index>());

		case OperandType::Variable:
			//-1 should mean the default target.
			return *Get<Variable*>();


		case OperandType::Type:
		case OperandType::Function:
			//Unsure if I'mm going to handle these, but this might be somewhat worth considering.


		//Useless without context.
		case OperandType::Member:
		case OperandType::Review:


		default:
			//something.
			break;
		}

		RGL_LOG(critical, "Register didn't exist. Fixer later.");
		throw nullptr;//Error.

		//*/
	}

}