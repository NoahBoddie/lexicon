#pragma once

#include "Operand.h"
#include "InstructionType.h"
namespace LEX
{

	class Runtime;
	//union Target;
	//class Operand;


	//EVERYTHING here will need some revision, but most of all the operation compiling functions. Once that is fixed up, I can go into
	// details about the compiler.


	//For operator, the left variable is a reference, while the right is a const Variable ref. 
	// Pointing to the left being an lvalue, being able to be edited under the right conditions (though it will be forbidden for constants).
	// While the right is something that may not be there (so maybe it should be a pointer or optional)

	//Includes InstructType so I can reuse some functions, like "+" for "+="

	//Both of these types aren't so used yet, so I'm concealing them for now.
	//Operator is the term for exposed functions like + or *. They're quite literal and use variables instead of positions.
	//Operator can access runtime, but it's going to be const, because operators can't edit anything.
	using Operator = RuntimeVariable(*)(RuntimeVariable&, RuntimeVariable, InstructType, const Runtime*);

	//Directive is the term for components of instructions, like TEST or push_arg. They cannot be manually manipulated.
	using Directive = void(*)(RuntimeVariable&, Operand, Operand, InstructType, Runtime*);



	//Should probably call this instructions
	//The public one are likely called operators, these just take variables. The other one is instructions or destiction like that.
	// I have plenty of names for the inbetweens of them.
	struct Instruction : public ConstClassAlias<std::variant<Operator, Directive>>
	{
		Instruction() = default;
		ALIAS_HEADER;

		//Should this use std::function? Probably no real reason.

		//This is basically what would sort this, no idea what I'll do. Do it later.
		//TEST_OpResult OpResult no longer viable, given this doesn't remark what register it goes into.
		//std::vector<ForceTest>* test_list//I'm really unsure if I'm truly gonna use this. Having a "test" function for 
		// NewOperation might work.
		void Operate(RuntimeVariable& result, Runtime* process, Operand a_lhs, Operand a_rhs) const
		{
			//No idea why this shit works but I can't just call it.

			if (index() == 0)
			{
				//do stuff that makes this operator.
				//Note, the routine process should likely handle the base transfer to turn the index into a variable.
				//result = std::get<Operator>(*this)->operator()();
			}
			else
			{
				//result = std::get<Directive>(*this)->operator()();
			}
		}
	};


	//TODO:Move the OperatorWorkShop functions to their own placement when I've confirmed their functionality.
	//TODO: (MakeManager) The instruct execution list. Should be named to that effect. Give it it's own space.
	//I intend on moving this
	inline std::array<Instruction, InstructType::Total> instructList
	{
		/*
		Instruction{},//Pass
		OperatorWorkShop::BitwiseXOR,//BitwiseXOR
		OperatorWorkShop::BitwiseOR,//BitwiseOR
		OperatorWorkShop::BitwiseAND,//BitwiseAND
		OperatorWorkShop::RightShift,//RightShift
		OperatorWorkShop::LeftShift,//LeftShift
		OperatorWorkShop::BitwiseNOT,//BitwiseNOT
		OperatorWorkShop::LogicalNOT,//LogicalNOT
		{},//UnaryPlus
		OperatorWorkShop::UnaryMinus,//UnaryMinus
		OperatorWorkShop::GreaterThan,//GreaterThan
		OperatorWorkShop::LesserThan,//LesserThan
		OperatorWorkShop::GreaterThan,//GreaterOrEqual
		OperatorWorkShop::LesserThan,//LesserOrEqual
		OperatorWorkShop::Equals,//EqualTo
		OperatorWorkShop::Equals,//NotEqualTo
		OperatorWorkShop::Equals,//EqualAbsTo
		OperatorWorkShop::LogicalOR,//LogicalOR
		OperatorWorkShop::LogicalAND,//LogicalAND
		OperatorWorkShop::Subtract,//Subtract
		OperatorWorkShop::Addition,//Addition
		OperatorWorkShop::Division,//Division
		OperatorWorkShop::Multiply,//Multiply
		OperatorWorkShop::Modulo,//Modulo
		OperatorWorkShop::Exponent,//Exponent
		{},//Access
		OperatorWorkShop::Push,//Push
		{},//PushVariable
		OperatorWorkShop::IncArgStack,//IncrementArgStack
		{},//IncrementTarStack
		OperatorWorkShop::Test,//Test
		{},//ObjectTest
		OperatorWorkShop::Call,//Call
		{},//ReturnDefault
		OperatorWorkShop::DropStack,//DropStack
		OperatorWorkShop::JumpStack//JumpStack
		//*/
	};
}