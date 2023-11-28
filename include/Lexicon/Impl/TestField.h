#pragma once

//#include "Object.h"
//#include "ObjectID.h"
//#include "ObjectIDMapper.h"

#include "Lexicon/Impl/Operand.h"

#include "Lexicon/Impl/Prototype.h"
#include "Lexicon/Impl/Construct.h"
#include "Lexicon/Impl/Hierarchy.h"
#include "Lexicon/Impl/Script.h"
#include "Lexicon/Impl/Project.h"
#include "Lexicon/Impl/ProjectManager.h"

#include "Lexicon/Impl/Exception.h"
//#include "Parser.h"
#include "Lexicon/Impl/ParserTest.h"


#include "Compiler.h"

#include "Lexicon/Impl/Variable.h"

#include "VarInfo.h"

//#include "SignatureManager.h"//suppressed for now

//This is a test space for this. I'd like to move it to a more fitting location.

//New
//*
#include "Instruction.h"
#include "InstructionType.h"
#include "OperatorType.h"
#include "Runtime.h"
#include "RuntimeVariable.h"
#include "IVariable.h"
#include "GlobalVariable.h"
#include "Literal.h"
#include "LiteralManager.h"
#include "Scope.h"
#include "Solution.h"
#include "RoutineBase.h"
#include "Operation.h"
#include "Operand.h"
#include "OperandType.h"
#include "Target.h"
#include "RuntimeVariable.h"
#include "MemberPointer.h"
#include "RoutineCompiler.h"
//*/

//Move me you idiot.
struct Initializer
{
	using def = void(*)();

	template <std::convertible_to<def> T>
	Initializer(T func)
	{
		if (func)
			func();
	}
};
inline static int test = 0;

#define INITIALIZE() inline static Initializer CONCAT(__init_,__COUNTER__) = []() -> void

namespace LEX
{
		struct InstructWorkShop
		{
			//None of these actually work rn. As for most, number will need adjusting first.

			//I would like to make a core function that sets this up if I can.

			static Variable BasisFunc(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				return {};
			}

			//Mathmatics and Comparison can have a base function.
			template<class Operatable, bool Assign = false>
			static RuntimeVariable BinaryMath(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				//This covers basically most of the below stuff.
				Number back = a_lhs->AsNumber();
				Number front = a_rhs->AsNumber();

				Operatable op{};

				Number result = op(back, front);
				//The below needs to curb "class std::" from the below
				RGL_LOG(trace, "{} {} {} = {}", back, typeid(Operatable).name(), front, result);

				a_rhs = Variable{ result, back.Cmp(front) > 0 ? nullptr : nullptr };

				if constexpr (Assign){
					//Whichever one is used is based on the measuring of numbers
					a_lhs = a_rhs;

				}
					
				return a_rhs;
			}

			template<class Operatable>
			static RuntimeVariable UnaryMath(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				//This covers basically most of the below stuff.
				Number back = a_lhs->AsNumber();

				Operatable op{};
				
				Number result = op(back);
				//The below needs to curb "class std::" from the below
				RGL_LOG(trace, "{} on {} = {}", typeid(Operatable).name(), back, result);

				a_rhs = Variable{ result,  nullptr };

				return a_rhs;
			}


			template<class Operatable>
			static RuntimeVariable BinaryCompare(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				//Don't really need a unary compare much, it's basically just '!'. So I could use an std::conditional for it.

				//This covers basically most of the below stuff.
				Number back = a_lhs->AsNumber();
				Number front = a_rhs->AsNumber();

				Operatable op{};

				bool result = op(back, front);
				//The below needs to curb "class std::" from the below
				RGL_LOG(trace, "{} {} {} = {}", back, typeid(Operatable).name(), front, result);

				a_rhs = Variable{ result, back.Cmp(front) > 0 ? nullptr : nullptr };


				return a_rhs;
			}

			/*
			//I can use the same function for this btw, about what function type it is
			static RuntimeVariable Subtract(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Number back = a_lhs->AsNumber();
				Number front = a_rhs->AsNumber();

				Number result = back - front;

				RGL_LOG(trace, "{} - {} = {}", back, front, result);

				if (type != InstructType::Subtract) {
					a_lhs = Variable{ result, back.IsSubordinate(front) ? nullptr : nullptr };
				}

				return Variable{ result, nullptr };
			}

			static Variable Addition(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Number back = a_lhs.AsNumber();
				Number front = a_rhs.AsNumber();

				auto result = back + front;

				RGL_LOG(trace, "{} + {} = {}", back, front, result);

				if (type != InstructType::Addition)
					a_lhs = result;

				return result;
			}

			static Variable Multiply(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Number back = a_lhs.AsNumber();
				Number front = a_rhs.AsNumber();

				auto result = back * front;

				RGL_LOG(trace, "{} * {} = {}", back, front, result);

				if (type != InstructType::Multiply)
					a_lhs = result;

				return result;
			}

			static Variable Modulo(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Integer back = a_lhs.GetImplicit<ValueType::Integer>();
				Integer front = a_rhs.GetImplicit<ValueType::Integer>();

				auto result = back % front;

				RGL_LOG(trace, "{} * {} = {}", back, front, result);

				if (type != InstructType::Modulo)
					a_lhs = result;

				return result;
			}

			static Variable Division(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Number back = a_lhs.GetImplicit<ValueType::Number>();
				Number front = a_rhs.GetImplicit<ValueType::Number>();

				auto result = back / front;

				RGL_LOG(trace, "{} / {} = {}", back, front, result);

				if (type != InstructType::Division)
					a_lhs = result;

				return result;
			}

			static Variable Exponent(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Number back = a_lhs.GetImplicit<ValueType::Number>();
				Number front = a_rhs.GetImplicit<ValueType::Number>();

				auto result = pow(back, front);

				RGL_LOG(trace, "{} / {} = {}", back, front, result);

				if (type != InstructType::Exponent)
					a_lhs = result;

				return result;
			}

			static Variable BitwiseXOR(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Integer back = a_lhs.GetImplicit<ValueType::Integer>();
				Integer front = a_rhs.GetImplicit<ValueType::Integer>();

				auto result = back ^ front;

				RGL_LOG(trace, "{} XOR {} = {}", back, front, result);

				if (type != InstructType::BitwiseXOR)
					a_lhs = result;

				return result;
			}


			static Variable BitwiseAND(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Integer back = a_lhs.GetImplicit<ValueType::Integer>();
				Integer front = a_rhs.GetImplicit<ValueType::Integer>();

				auto result = back & front;

				RGL_LOG(trace, "{} & {} = {}", back, front, result);

				if (type != InstructType::BitwiseAND)
					a_lhs = result;

				return result;
			}


			static Variable BitwiseOR(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Integer back = a_lhs.GetImplicit<ValueType::Integer>();
				Integer front = a_rhs.GetImplicit<ValueType::Integer>();

				auto result = back & front;

				RGL_LOG(trace, "{} | {} = {}", back, front, result);

				if (type != InstructType::BitwiseOR)
					a_lhs = result;

				return result;
			}

			static Variable LogicalAND(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Boolean back = a_lhs.GetImplicit<ValueType::Boolean>();
				Boolean front = a_rhs.GetImplicit<ValueType::Boolean>();

				auto result = back && front;

				RGL_LOG(trace, "{} && {} = {}", back, front, result);

				return result;
			}


			static Variable LogicalOR(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Boolean back = a_lhs.GetImplicit<ValueType::Boolean>();
				Boolean front = a_rhs.GetImplicit<ValueType::Boolean>();

				auto result = back && front;

				RGL_LOG(trace, "{} || {} = {}", back, front, result);

				return result;
			}


			static Variable RightShift(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Integer back = a_lhs.GetImplicit<ValueType::Integer>();
				Integer front = a_rhs.GetImplicit<ValueType::Integer>();

				auto result = back & front;

				RGL_LOG(trace, "{} >> {} = {}", back, front, result);

				if (type != InstructType::RightShift)
					a_lhs = result;

				return result;
			}


			static Variable LeftShift(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Integer back = a_lhs.GetImplicit<ValueType::Integer>();
				Integer front = a_rhs.GetImplicit<ValueType::Integer>();

				auto result = back & front;

				RGL_LOG(trace, "{} << {} = {}", back, front, result);

				if (type != InstructType::LeftShift)
					a_lhs = result;

				return result;
			}



			//Groups equals to the XThan
			static Variable GreaterThan(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Number back = a_lhs.GetImplicit<ValueType::Number>();
				Number front = a_rhs.GetImplicit<ValueType::Number>();

				auto result = back > front || (type == InstructType::GreaterOrEqual && back == front);

				if (type == InstructType::GreaterOrEqual)
					RGL_LOG(trace, "{} >= {} = {}", back, front, result);
				else
					RGL_LOG(trace, "{} > {} = {}", back, front, result);

				return result;
			}


			static Variable LesserThan(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Number back = a_lhs.GetImplicit<ValueType::Number>();
				Number front = a_rhs.GetImplicit<ValueType::Number>();

				auto result = back < front || (type == InstructType::LesserOrEqual && back == front);


				if (type == InstructType::LesserOrEqual)
					RGL_LOG(trace, "{} <= {} = {}", back, front, result);
				else
					RGL_LOG(trace, "{} < {} = {}", back, front, result);

				return result;
			}


			static Variable Equals(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
			{
				Number back = a_lhs.GetImplicit<ValueType::Number>();
				Number front = a_rhs.GetImplicit<ValueType::Number>();

				//Needs to use true equal to

				auto result = type == InstructType::EqualTo ? back == front : back != front;

				if (type == InstructType::EqualTo)
					RGL_LOG(trace, "{} == {} = {}", back, front, result);
				else
					RGL_LOG(trace, "{} != {} = {}", back, front, result);

				return result;
			}
			
			static Variable BitwiseNOT(RuntimeVariable& a_lhs, RuntimeVariable, InstructType, const Runtime*)
			{
				Integer value = a_lhs.GetImplicit<ValueType::Integer>();

				auto result = ~value;

				RGL_LOG(trace, "~{} = {}", value, result);

				return result;
			}

			static Variable LogicalNOT(RuntimeVariable& a_lhs, RuntimeVariable, InstructType, const Runtime*)
			{
				Number value = a_lhs.GetImplicit<ValueType::Number>();

				auto result = !value;

				RGL_LOG(trace, "!{} = {}", value, result);

				return result;
			}



			static Variable UnaryMinus(RuntimeVariable& a_lhs, RuntimeVariable, InstructType, const Runtime*)
			{
				Number value = a_lhs.GetImplicit<ValueType::Number>();

				auto result = -value;

				RGL_LOG(trace, "-{} = {}", value, result);

				return result;
			}
			//*/

			//Directives-These will have to be moved

			static void Call(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
			{
				//This is old and I forget how it's done.

				/*
				Coroutine* routine = a_lhs.As<Coroutine*>();

				size_t arg_size = a_rhs.As<size_t>();

				auto end = process->argStack.end();
				auto it = end - arg_size;

				//std::vector<Variable> args{ arg_size };
				std::vector<Variable> args{ it, end };
				//How this will work perhaps at a later point


				//This should be included later 
				Variable target = process->tarStack[process->tarIndex];

				--process->tarIndex;
				process->argIndex -= arg_size;

				result = routine->Run(target, args);
				//*/
			}

			//void(*)(RuntimeVariable&, Operand, Operand, InstructType, Runtime*);

			static void IncArgStack(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
			{
				//Pretty simple honestly. Increase by the amount.
				runtime->AdjustStackPointer(StackPointer::Argument, a_lhs.Get<int64_t>());
			}

			static void IncVarStack(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
			{
				//Pretty simple honestly. Increase by the amount.
				runtime->AdjustStackPointer(StackPointer::Variable, a_lhs.Get<int64_t>());
			}

			static void Push(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
			{
				//Not much else needs to be known.
				// The destination is upto the operation out.
				ret = a_lhs.GetVariable(runtime);

				//Correction, you cannot do this because out is for register output only.
				//So ngl, I have no fucking idea what the hell this was supposed to do.
			}

			//Test is probably deprecated.
			static void Test(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
			{
				//Not actually sure how this would work, the idea is basically that I implicit bool cast to it, and output the result
				// to the process flags.



				//bool value = process->GetVariable(a_lhs).GetImplicit<ValueType::Boolean>();
				//process->flags.Set(ProcessFlags::TestBit, value);
			}

			static void Ret(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
			{
				//This is going to have to have an official function to handle this
				runtime->_flags.Set(RuntimeFlag::RetBit, true);
	
			}

			static void DropStack(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
			{
				//Drop should converge with a climb as well.

				//Rename to drop runtime pointer
				int64_t mode = a_rhs.Get<int64_t>();


				//If mode is 0 flag is required to be 0,
				// if 1 flag is required to be 1,
				// if any other value, flag is not required.
				// though, what drop wouldn't be conditional? A while loop probably that's broken by an out.
				if (mode <= 1)
				{
					bool flag = runtime->GetFlags().Get(RuntimeFlag::TestBit);

					if (mode && flag || !mode && !flag)
						return;
				}

				runtime->AdjustStackPointer(StackPointer::Runtime, a_lhs.Get<uint64_t>());
			}

			static void JumpStack(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
			{
				//Canning this because I think drop is likely going to be a better solution, but I'll wait until I can compile some shit.


				int64_t mode = a_rhs.Get<int64_t>();


				//If mode is 0 flag is required to be 0,
				// if 1 flag is required to be 1,
				// if any other value, flag is not required.
				// though, what drop wouldn't be conditional? A while loop probably that's broken by an out.
				if (mode <= 1)
				{
					bool flag = runtime->GetFlags().Get(RuntimeFlag::TestBit);

					if (mode && flag || !mode && !flag)
						return;
				}

				runtime->SetStackPointer(StackPointer::Runtime, a_lhs.Get<uint64_t>());
			}
			//Conditional instructions
			// SOME of these could have 2 versions one conditional and the other not conditional, which would determine if it's going to do the conditional verison of it or not
			// The conditional version should probably always likely be a plus 1.




			//I think access is supposed to be like object on the left gets pushed into the register. Not

			//target.GetCombatTarget().GetOwner().GetActorValue('health')
			//This should look like the below (ignoring the drops and stuff because it's a pain):
			//test target
			//drop x 0
			//inc_arg_stack 1//<1>
			//push target => arguments[argStack - 1]//
			//inc_arg_stack 0
			//call GetCombatTarget 1 => pref_reg//<2>
			//inc_arg_stack 1
			//push pref_reg => arguments[argStack - 1]
			//inc_arg_stack 0
			//call GetOwner 1 => pref_reg
			//inc_arg_stack 1
			//push pref_reg => arguments[argStack - 1]
			//inc_arg_stack 1
			//push lit:'health' => arguments[argStack - (1 + paramNo)]
			//call GetActorValue 2 => pref_reg
			//push pref_reg result
			//end
			//<1> This is the start of where the access operator does it's business. To do this, it needs to check for another dual operator that isnt 
			// access operation. If it encounters access operation, it should skip.
			// 
			//<2> This outputing to pref_reg could possibly be ammended manually be the function read by peeking the next value. If it's an access operator it
			// will output it to that. Similarly, when access operator is peeking back it can use this time to know if it needs to load the push of the pref register
			// into the it.
			//<!>Actually, negatory. I can just check if the first thing on the left is an open operator.


		};



		struct RoutineCompiler;



	




	struct Scope;
	struct FunctionData;

	
	
	namespace //Processes the records, these 2 are expression parsers.
	{

		//
		using OperatorConstructor = Solution(*)(ExpressionCompiler*, OperatorType, Solution, Solution, Register);
		//The main function that uses the above is effectively processing ranges. Remember that.

		//The idea of this this is what interprets OPerators and pushes back instructions for the instruct list to use.
		//TODO: (MakeManager) The Operator Constructor list
		constexpr std::array<OperatorConstructor, OperatorType::Total> operatorCtorList
		{
			{}
		};


		OperatorType GetOperatorType(Record& target)
		{
			bool binary = target.SYNTAX().type == SyntaxType::Binary;

			RGL_LOG(trace, "START CTOR");

			switch (Hash(target.GetTag()))
			{
			case "|"_h:
			case "OR"_h:
				logger::debug("construct symbol '|' / 'OR'");
				return OperatorType::BitwiseOR;

			case "or"_h:
			case "||"_h:
				logger::debug("construct symbol '|' / 'or'");
				return OperatorType::LogicalOR;

			case "&"_h:
			case "AND"_h:
				logger::debug("construct symbol '&' / 'AND'");
				return OperatorType::BitwiseAND;

			case"&&"_h:
			case "and"_h:
				logger::debug("construct symbol '&&' / 'and'");
				return OperatorType::LogicalAND;

			case ">>"_h:
				logger::debug("construct symbol '>>'");
				return OperatorType::RightShift;

			case "<<"_h:
				logger::debug("construct symbol '<<'");
				return OperatorType::LeftShift;

			case "!="_h:
				logger::debug("construct symbol '!='");
				return OperatorType::NotEqualTo;

			case "!=="_h:
				logger::debug("construct symbol '!=='");
				return OperatorType::AbsNotEqualTo;


			case "<"_h:
				logger::debug("construct symbol '<'");
				return OperatorType::LesserThan;
				break;

			case ">"_h:
				logger::debug("construct symbol '>'");
				return OperatorType::GreaterThan;

			case "<="_h:
				logger::debug("construct symbol '<='");
				return OperatorType::LesserOrEqual;

			case ">="_h:
				logger::debug("construct symbol '>='");
				return OperatorType::GreaterOrEqual;

			case "=="_h:
				logger::debug("construct symbol '=='");
				return OperatorType::EqualTo;

			case "==="_h:
				logger::debug("construct symbol '==='");
				return OperatorType::AbsEqualTo;

			case "^"_h:
			case "XOR"_h:
				logger::debug("construct symbol '^' / 'OR'");
				return OperatorType::BitwiseXOR;

			case "*"_h:
				logger::debug("construct symbol \'*\'");
				return OperatorType::Multiply;

			case "+"_h:
				RGL_LOG(debug, "construct symbol '+' ({})", binary ? "binary" : "unary");
				return binary ? OperatorType::Addition : OperatorType::UnaryPlus;

			case "-"_h:
				logger::debug("construct symbol \'-\'");
				return binary ? OperatorType::Subtract : OperatorType::UnaryMinus;

			case "/"_h:
				logger::debug("construct symbol \'/\'");
				return OperatorType::Division;

			case "**"_h:
				logger::debug("construct symbol \'^\'");
				return OperatorType::Exponent;

			case "%"_h:
				logger::debug("construct symbol \'%\'");
				return OperatorType::Modulo;

			default:
				//ARTHMETIC_LOGGER(info, "Failure to handle operator, value \'{}\'", data->view[0]);
				return OperatorType::Invalid;
			}
		}



		InstructType GetInstructType(Record& target)
		{
			bool binary = target.SYNTAX().type == SyntaxType::Binary;

			switch (Hash(target.GetTag()))
			{
			case "|"_h:
			case "OR"_h:
				logger::debug("construct symbol '|' / 'OR'");
				return InstructType::BitwiseOR;

			case "or"_h:
			case "||"_h:
				logger::debug("construct symbol '|' / 'or'");
				return InstructType::LogicalOR;

			case "&"_h:
			case "AND"_h:
				logger::debug("construct symbol '&' / 'AND'");
				return InstructType::BitwiseAND;

			case"&&"_h:
			case "and"_h:
				logger::debug("construct symbol '&&' / 'and'");
				return InstructType::LogicalAND;

			case ">>"_h:
				logger::debug("construct symbol '>>'");
				return InstructType::RightShift;

			case "<<"_h:
				logger::debug("construct symbol '<<'");
				return InstructType::LeftShift;

			case "!="_h:
				logger::debug("construct symbol '!='");
				return InstructType::NotEqualTo;

			case "!=="_h:
				logger::debug("construct symbol '!=='");
				return InstructType::NotEqualAbsTo;


			case "<"_h:
				logger::debug("construct symbol '<'");
				return InstructType::LesserThan;
				break;

			case ">"_h:
				logger::debug("construct symbol '>'");
				return InstructType::GreaterThan;

			case "<="_h:
				logger::debug("construct symbol '<='");
				return InstructType::LesserOrEqual;

			case ">="_h:
				logger::debug("construct symbol '>='");
				return InstructType::GreaterOrEqual;

			case "=="_h:
				logger::debug("construct symbol '=='");
				return InstructType::EqualTo;

			case "==="_h:
				logger::debug("construct symbol '==='");
				return InstructType::EqualAbsTo;

			case "^"_h:
			case "XOR"_h:
				logger::debug("construct symbol '^' / 'OR'");
				return InstructType::BitwiseXOR;

			case "*"_h:
				logger::debug("construct symbol \'*\'");
				return InstructType::Multiply;

			case "+"_h:
				RGL_LOG(trace, "construct symbol '+' ({})", binary ? "binary" : "unary");
				return binary ? InstructType::Addition : InstructType::UnaryPlus;

			case "-"_h:
				logger::debug("construct symbol \'-\'");
				return binary ? InstructType::Subtract : InstructType::UnaryMinus;

			case "/"_h:
				logger::debug("construct symbol \'/\'");
				return InstructType::Division;

			case "**"_h:
				logger::debug("construct symbol \'^\'");
				return InstructType::Exponent;

			case "%"_h:
				logger::debug("construct symbol \'%\'");
				return InstructType::Modulo;

			default:
				//ARTHMETIC_LOGGER(info, "Failure to handle operator, value \'{}\'", data->view[0]);
				return InstructType::Invalid;
			}
		}


		InstructType OperatorToInstruction(OperatorType op)
		{

			return InstructType::Invalid;
		}

		Solution BasicBinaryGenerator(ExpressionCompiler* compiler, InstructType op, Solution lhs, Solution rhs, Register out)
		{

			//No need to deviate for now.
			//InstructType type = OperatorToInstruction(op);

			compiler->GetOperationList().push_back(Operation{ op, out, lhs, rhs });
			logger::trace("{}", compiler->GetOperationList().back()._instruct);
			return Solution(nullptr, OperandType::Register, out);
		}

		//RecordProcessors
		Solution OperatorProcess(ExpressionCompiler* compiler, Record& target)
		{
			//TODO:Note, this operator stuff WILL fail when it comes to member access, since it needs
			// what's on the left to process what's on the right. In this specific instance, I'm going to make 
			// something else handle thing

			//Honestly, in general, maybe this should be taking control a bit less. Consider a set up
			// like how the functional part of operators work, 2 template to do all the jobs.

			//Should be an expression compiler but I'm pretending that it's a routine compiler.
			Register prefered = compiler->GetPrefered();

			//OperatorType op = GetOperatorType(target);
			InstructType op = GetInstructType(target);

			//if (op == OperatorType::Invalid) {
			if (op == InstructType::Invalid) {
				RGL_LOG(critical, "Invalid something something");
				throw nullptr;
			}

			Solution lhs{};
			Solution rhs{};

			if (target.SYNTAX().type == SyntaxType::Binary)
			{
				//Which is done first should be handled by which is more complex
				std::string str1 = "left";
				std::string str2 = "right";

				Register reg1 = Register::Left;
				Register reg2 = Register::Right;
				
				Record& left = target.FindChild(str1)->GetChild(0);
				RGL_LOG(debug, "<%>after find child");
				lhs = compiler->CompileExpression(left, reg1);
				RGL_LOG(debug, "<%>end");
				if (prefered == Register::Result)
				{
					//This should actually be pushing what's in the lhs into the result register, then
					// making the lh solution the result register.
					Operation free_reg{ InstructType::Push,Register::Result, lhs };



					//free_reg._instruct = InstructType::Push;
					//free_reg._lhs = reg1;
					//free_reg._out = Register::Result;

					//This statement will become more properly invalid once when I obsure the current vector. Address later.
					compiler->GetOperationList().push_back(free_reg);


					lhs = Solution{ nullptr, OperandType::Register, Register::Result };
				}

				rhs = compiler->CompileExpression(target.FindChild(str2)->GetChild(0), reg2);
			}
			else// if syntax == unary blah blah blah
			{
				//Unary is a lot simpler to deal with
				lhs = compiler->CompileExpression(target.GetChild(0), Register::Left);
			}

			//Do operation here. Needs solutions, outputs solution.
			//return operatorCtorList[op](compiler, op, lhs, rhs, prefered);
			return BasicBinaryGenerator(compiler, op, lhs, rhs, prefered);

		}


		Solution LiteralProcess(ExpressionCompiler* compiler, Record& target)
		{
			//Combine with the use of variables.
			Literal result = LiteralManager::ObtainLiteral(target);
			RGL_LOG(trace, "test0");
			result = LiteralManager::ObtainLiteral(target);
			*result;
			RGL_LOG(trace, "test1");
			Solution sol { result->GetPolicy(), OperandType::Literal, result };
			RGL_LOG(trace, "test2");
			return sol;
		}

		void BlockProcess(RoutineCompiler* compiler, Record& target)
		{
			compiler->CompileBlock(target);
		}



		void ReturnProcess(RoutineCompiler* compiler, Record& target)
		{
			Solution result = compiler->CompileExpression(target.GetChild(0), Register::Result); 

			//Right here the solutions given type should be evaluated to see if a correct type is being returned.
			
			//TODO: if the solution isn't in a register that is the result register, push the solution into 
			// the return. Also check type.
			//compiler->GetOperationList().push_back(ret_op);

			Operation ret_op{ InstructionType::Return };

			//This part should probably say what left and right situation should be on display.



				
			compiler->GetOperationList().push_back(ret_op);
		}


		void CallProcess(RoutineCompiler* compiler, Record& target)
		{
			//This cannot be used right now, as the actual ability to get a function hasn't been implemented just yet.
			// But, I can put in the most of everything.

			IFunction* function = nullptr;



			Solution result = compiler->CompileExpression(target.GetChild(0), Register::Result);

			//Right here the solutions given type should be evaluated to see if a correct type is being returned.

			Operation ret_op{ InstructionType::Return };

			//This part should probably say what left and right situation should be on display.




			compiler->GetOperationList().push_back(ret_op);
		}

		INITIALIZE()
		{
			generatorList[SyntaxType::Return] = ReturnProcess;
			generatorList[SyntaxType::Block] = BlockProcess;

			generatorList[SyntaxType::Unary] = OperatorProcess;
			generatorList[SyntaxType::Binary] = OperatorProcess;
			generatorList[SyntaxType::Assign] = OperatorProcess;

			generatorList[SyntaxType::Number] = LiteralProcess;
			generatorList[SyntaxType::Integer] = LiteralProcess;
			generatorList[SyntaxType::Boolean] = LiteralProcess;
			generatorList[SyntaxType::String] = LiteralProcess;
			generatorList[SyntaxType::Object] = LiteralProcess;


			instructList[InstructType::Push] = InstructWorkShop::Push;
			instructList[InstructType::Return] = InstructWorkShop::Ret;
			instructList[InstructType::JumpStack] = InstructWorkShop::JumpStack;
			instructList[InstructType::IncArgStack] = InstructWorkShop::IncArgStack;
			instructList[InstructType::IncVarStack] = InstructWorkShop::IncVarStack;
			instructList[InstructType::DropStack] = InstructWorkShop::DropStack;

			
			instructList[InstructType::Addition] = InstructWorkShop::BinaryMath<std::plus<>, false>;
			instructList[InstructType::Subtract] = InstructWorkShop::BinaryMath<std::minus<>, false>;
			instructList[InstructType::Multiply] = InstructWorkShop::BinaryMath<std::multiplies<>, false>;
			instructList[InstructType::Division] = InstructWorkShop::BinaryMath<std::divides<>, false>;
			instructList[InstructType::Modulo] = InstructWorkShop::BinaryMath<std::modulus<>, false>;
			//We not ready for this one.
			//instructList[InstructType::LogicalNOT] = InstructWorkShop::UnaryMath<std::logical_not<void>>;
			instructList[InstructType::UnaryMinus] = InstructWorkShop::UnaryMath<std::negate<void>>;
			


		};

	}
	


	///////////////////////////////////////////////////////////////////

	enum struct ReserveType
	{
		None,
		Function,
		Property,
		Other,		//with other you can make your own format string as to why, otherwise there's a prebuilt reason for each.

	};
	//You use the above along with a reservation type to reserve the use of a name at a particular place. This will be attached to the environment.
	// basically it will prevent you from 


	struct ReserveNode{};

	//I reckon to dare this is where 
	enum struct Statement
	{
		TopLevel,	//The top level, where most things can go
		Function,	//the declaration of a function obvs
		IfBounds,	//Within the bounds of an if statements paras, if that's ever relevant to me.
		//Switch,	//Never going to do these, but stands as the point of some statement that would require extra noting.
		Line,		//The moment a non-one of the above happens, it's a line.
		Assign,		//THIS is what subroutines most often are. Simple single line evaluations that go to one place.

		//YOu should also be able to start writing from one of these statements, like within a line or function, both of things lexicon may be needed for.
	};

	//Contexts
	// Script-top level
	//  struct
	//   member
	//   constructor
	//   method
	//  function
	//   line


	//Notes for parsing some things
	// There's a parser function for each and every thing to parse of the [ExpressionType?], however there's specific compiling for each operator.
	// how the above should work would likely be very similar to that of how Expression works (pulling all classes via type).


	//To begin compiling the information, I should be addressing the Expression, maybe some of the RoutineCompiler
	//So I'm thinking like, the compil




	//So for pointer specification this is the set up I'm going to take, but the major difference is and will be that
	// the test base is the thing that will be specialized, and not the upper part. This will allow me to throw
	// errors about what's expected where better using concepts (I hope). 
	namespace MoreTesting
	{
		struct test_base {};

		template <class T>
		class testing : public test_base
		{

		};

		template <>
		class testing<int>
		{

		};
	}
	//How this sort of thing will then likely work is akin to a type trait, with a singular type alias
	// handling all of the grunt work.

	//A note also is that I can use this kind of specialization on functions too, so long as they share a return type,
	// so I can just use that rather than an out and out class getting the object code of something


	

}


namespace LEX::makeType
{
	//namespace for making type policies manually.


	void TestSpace()
	{
		//TypePolicy new_policy {};
	}
}

//TODO: Most if not all todos past this point are invalid



//This shouldn't be deleted, has some valuable things within.
/*
namespace TestField__Types
{

	//Just a note about parsing now, I think I want to move away from toml, for a bit, but have an area specifically for toml, like once it's done, it's done.
	//I additionally want some stuff that can signify special roles, like

	//#use DEBUG
	//or
	//#use CASE SENSETIVE
	//Another one,
	//#use return <Type>, this will set the default type for functions in the project. So making something just 
	// function(){} in someting like AVG will automatically be a float. In other projects that are more about doing, it will likely be void.
	//#use this <Type> could be another idea, setting the default call from function, which is normally determined via other methods.

	//Lastly, a thing that I could do, is have certain sections of code only loaded if another file is detected


	struct Void {};

	//Just a fucking example
	using ObjectVar = std::variant<Void, void*, int*, char*>;

	//TODO:struct is called Construct
	struct Struct;


	//This is generally the type of setup I'm looking to have.
#define TYPES_MACRO void*
#define HEAD_TYPES_MACRO 1
	//This might not work though, because here's something possibly stupid. What if someone has 2 different APIs like ToolUI set up, 
	// and actor value generator both.
	using TruerObjectVar = std::variant<
		Void,
		TYPES_MACRO,
		Struct			//No matter where this index is, the outputted region should always be 7F if regular, and FF ir array.
	>;


	//I think it would possibly be for the best that the variant is exposed as something that can be mallible as a template, but the hard cast type when working
	// in it from this side. 
	//The idea would be to allow me to have the API version of API to apply different types (which you'd juse like AVG::Object or TUI::Object).

	//I can probably do something like this, and have which one makes it as the "defacto" type macro. So it will check if it's own source is enabled
	// and if not, it doesn't set a "core". Most importantly, if no LEX source is used, it's not relevant.

	//So something like this

	template <class... Ts>
	using APIObject = int;


	//This goes in lexicon, given it's  macros and the idea of types macro is made there first (designed to be overwritten or perhaps pushed)
#define TYPES() Void, TYPES_MACRO, Struct
#define HEAD_TYPES() Void, HEAD_TYPES_MACRO, Struct
	//This goes in XXX
#define TYPES_MACRO std::string, int, void*, float
	//This too goes in XXX, which looks for it's own source value.
#ifndef IS_XXX_SOURCE
	//HEAD_TYPES is what the actual LEX::Object class uses to populate it's values.
#define HEAD_TYPES_MACRO TYPES_MACRO
#endif

	namespace XXX
	{

		//Using this version of object is vastly different than using the actual version of object. What this version will do is send you
		// an index and a void pointer. The point being that depending on what API version you're requesting from type you're requesting from will cause it to be different.
		// additionally, to tell the difference between which API is supposed to be gotten, I think it would possibly be best to seperate uses of the API source by
		// templates or something. Because only one API use wins, and getting and storing the location of them all might be a pain.
		//So make it so
		using Object = APIObject<TYPES()>;
	}


	//The more and more I see this, the more I think it might be prudent to make lexicon redistributables. These redistributables would store the main "OBJECT"
	// and a lot of the virtual machine related behaviour.
	//Then the individual plugins can have their own implementations of Object

	//Alright, so that idea is fucking insane, instead, include the API inside of a namespace. DO NOT USE RE namespace, that shit will fuck some stuff up,
	// but other than that, this will allow us to include the same API twice. As long as you gate it with a namespace. The large part is, don't use the API directly
	// I'll need to convey that.

	//Also, important note. No pragma once, so I can include it in the namespace. To take up some less space, I can make some common API headers
	// that contain my ABIContainer and such.

	//If someone to use it directly, should make something for that.
	
#undef TYPES_MACRO



	






	//A specialization like this is required when
	// LookingUp an object to which a lookup can be invalid
	// when getting the LEX::ObjectID of a given object
	//
	template<uint8_t N> struct speck
	{
		virtual LEX::ObjectID GetType(ObjectVar& object)
		{
			return {};
		}
	};




	template<> struct speck<0> {
		//Speck zero is invalid

		virtual LEX::ObjectID GetType(ObjectVar& object)
		{
			//Report that there's no valid specialization of the type here.
			return {};
		}
	};

	//needs a class above it.
	//Also, may make a macro for this, just so all the classes conform properly.
	//Main reason for the macro, is it would be nice to just use the type to get the number. I guess all this is fine. For now.
	template<> struct speck<1>
	{
		//using DefinedClass = T;

		//Should return only the 7 bits that matter.
		LEX::ObjectID operator()(ObjectVar& object)
		{
			return GetType(object);
		}

		LEX::ObjectID GetType(ObjectVar& object)
		{
			//Report that there's no valid specialization of the type here.
			return {};
		}
	};
S
	template <uint8_t I>
	struct speck_user : public speck<I>
	{
		//Should return only the 7 bits that matter.
		static LEX::ObjectID TESTType(ObjectVar& object)
		{
			speck<I> speck;

			return speck(object);

			return GetType(object);
		}
	};
	



	namespace RE
	{
		using TESForm = int;
	}



	template <int I, int Max>
	LEX::ObjectID _GetType(ObjectVar& obj, uint8_t index)
	{
		if (index != I)
		{

			if constexpr (I != Max)
				return _GetType<I + 1, Max>(obj, index);

			return LEX::ObjectID::Invalid();
		}

		using ClassType = speck<I>;

		speck<uint8_t(I)> speck;

		return speck.GetType(object);
	}

	LEX::ObjectID _GetType(ObjectVar obj)
	{
		uint8_t index = obj.index();

		if (!index)
			return LEX::ObjectID::Invalid();

		if (index == structIndex)
		{
			//Get obj as a struct
			//Get prototype
			//Get Type ID
			return LEX::ObjectID::Invalid();
		}

		
		//Depending on what the index is, I can give it certain starting points. Kinda shortens the searching I guess.

		if (index < 128)
			return _GetType<0, 127>(obj, index);
		else
			return _GetType<128, 255>(obj, index);
	}

	//So the idea with object type ids

	//Having conversion functions might be interesting.
	// Like say for forms and strings. Explicitly used when 

	
	
}
//*/

//This is the old version, I'm gonna just be playing it off the guide for now.
/*
namespace LEX
{
	//The toml handler will have a function map, that handles different types.
	//Of it, it handles 2 sorts things on 2 different inputs. The name, or the member called
	// type.

	//It can probably store string hashes, as nothing about this needs to be stored specifically.
	// 

	



	enum struct ParseQueryResult
	{
		No,
		Yes,
		Cancel
	};


	struct DataCollection
	{
		RecordDataIterator& it;
		RecordDataIterator begin;
		RecordDataIterator last;
		RecordDataIterator end;


		DataCollection(RecordDataIterator& a_it, RecordDataIterator a_end) :
			it(a_it),
			begin(a_it),
			end(a_end)
		{}

		DataCollection(RecordDataIterator& a_it, RecordDataIterator a_begin, RecordDataIterator a_last,  RecordDataIterator a_end) :
			it(a_it),
			begin(a_begin),
			last(a_last),
			end(a_end)
		{}
	};



	struct Parser;

	//TODO:Extend functionality of this a bit, I'd like this to keep record of the syntax's that parse this.
	// For example what if an '=' is detected where arguments go? Syntax should be able to beget syntax, but it should know where
	// such things came from.
	using ParseVisitor = ParseQueryResult(*)(Parser*, Record*, DataCollection);

	ENUM(ParseType)
	{
		Keyword,
		Syntax,
		Error,
		Total
	};

	struct Syntax
	{
		struct Sort
		{
			constexpr bool operator()(const Syntax& lhs, const Syntax& rhs) const
			{
				if (lhs.type != rhs.type)
					return lhs.type < rhs.type;

				return lhs.GetPriority() < rhs.GetPriority();
			}

		};

		ParseVisitor _func;

		std::string_view category;

		bool invert = false;//If invert it will only work if the choice category is not within.
		
		//Invert should be the sign bit. Make getting priority a function.
		int16_t _priority = 0;

		int16_t GetPriority() const
		{
			constexpr int16_t sign_bit = ~(1 << 15);
			return _priority & sign_bit;
		}

		bool IsInverted() const
		{
			return _priority & (1 << 15);
		}

		ParseType type = ParseType::Syntax;

		//ParseQueryResult operator()(Parser* parser, Record* record, DataCollection collection)
		//{
		//	_func(parser, record, collection);
		//}
	};

	

	//Entries are sorted by type, and if equal, by priority
	//TODO: Rename Syntax. The operator handler is the actual syntax. This is just the tokenizer?
	std::set<Syntax, Syntax::Sort> syntaxList;
	

	//Known 



	struct Parser
	{
		//virtual ~Parser() = default;//Required why?

		//Note, for memory, adding a character at the front might be a good idea to differentiate.
		std::list<std::string> memory;
		
		//If the context of a syntax is not found within the context of the parser, it will not be evaluated to run.
		// A possible setting would be the inversion of this.
		std::multiset<std::string_view> context;
		//Expandable context might be a good idea. Like to say, we are in a function right now. And have it seperate so it doesn't clash or something.
		//std::multiset<std::string_view> expandingContext;
		Record* _target;


		//TODO: Make data collection struct a function instead of a parameter. Not all types will need the set up. After, just have gets for each one. Status: Easy
		// Additionally, it can be something within collection that does it, lastly, collection can be a template class.

		RecordDataIterator it;	//The current iterator
		RecordDataIterator last;//The iterator parsed at the start of the last process
		RecordDataIterator begin;//Where it's started
		RecordDataIterator end;	//Where it will no longer be valid.

		const Syntax* lastSyntax = nullptr;

		Record& GetTarget()
		{
			return *_target;
		}


		//The might replace crew
		bool SyntaxMatch(const Syntax* syntax)
		{
			if (!syntax)
				return false;

			auto end = context.end();

			return std::find(context.begin(), end, syntax->category) != end;
		}
		bool SyntaxMatch(const Syntax& syntax)
		{
			return SyntaxMatch(&syntax);
		}

		void SyntaxHandle()
		{
			//Above this, needs to be a parsing function of it's own.
			for (auto& syntax : syntaxList)
			{
				if (SyntaxMatch(syntax) == false) {
					continue;
				}
				//TODO: Address, should the collection for syntax(? Or Tokenizer?) be able to push the actual parsing iterator?
				DataCollection collection{ it, begin, last, end };

				auto result = syntax._func(this, _target, collection);

				//Check validity?

				switch (result)
				{
				case ParseQueryResult::Yes://It did meet requirements. No more parsing.
					lastSyntax = &syntax;
					return;
				case ParseQueryResult::No://Means didn't match requirements.
					continue;
				case ParseQueryResult::Cancel://It met requirements but an error occured. Going to error parsing.
					//TODO: Make parsing error from cancel bail to the error section instead.
					throw ParsingError("Replace");
				}
			}
		}

		void HandleData(Record& tar, std::list<std::string> l_mem, std::multiset<std::string_view> txt)
		{
			//I'm just gonna manually visit for now. I sorta wish that I could do this in some other way.
			//TODO: suggestion, maybe I should use the actual visitor.

			//I want something to deal with this parser
			//{ DataType::Invalid, OperatorType::Invalid };
			//_lhs.second = OperatorType::Invalid;
			while (it != end) {
				auto old_it = it;

				SyntaxHandle();

				last = old_it;

				//If it didn't move, that's kinda an issue. Should this then throw?
				if (old_it == it) it++;
			}

		}

		template<class... Ts>requires(RGL::AllTrue<std::convertible_to<Ts, std::string_view>...>::value && sizeof...(Ts) > 0)
		static void Handle(Record& tar, Ts... txts)
		{

			Parser parser{};

			parser.HandleData(tar, {}, std::multiset<std::string_view>{txts...});
			
		}

		template<class... Ts>requires(RGL::AllTrue<std::convertible_to<Ts, std::string_view>...>::value && sizeof...(Ts) > 0)
		static void Handle(Record& tar, std::list<std::string>& l_mem, Ts... txts)
		{
			Parser parser{};

			parser.HandleData(tar, l_mem, std::multiset<std::string_view>{txts...});

			l_mem = parser.memory;
		}


	};


	


	//Now, I need to figure out if I want to visit all the syntax objects, how would I exactly go about doing such a thing?


	constexpr std::string_view routine_regex = "(?<Minus>- )|(?<Object><:.+(?=:>)..)|(?<Digits>[\\d\\.]+)|(?<Quotes>'[^']+.)|(?<Identifiers>(\\w|::)+)|(?<Symbols>[^\\w\\d\\s'\\.]([^\\w\\d\\s'\\.:]){0,})";
	constexpr std::string_view seq_routine_regex = "(?<Header>^[\\w;]{0,}\\s{0,}(?=;).)|(?<Minus>- )|(?<Object><:.+(?=:>)..)|(?<Digits>[\\d\\.]+)|(?<Quotes>'[^']+.)|(?<Identifiers>(\\w|::)+)|(?<Symbols>[^\\w\\d\\s'\\.]([^\\w\\d\\s'\\.:]){0,})";
	//May need revisions due to the ; possibly being caught first.
	constexpr std::string_view property_regex = "(?<Tooltip>;.+)|(?<DefValue>(?<=:)\\s{0,}\\K.[^;\n]+)|(?<Type>\\w+\\s{0,}:)|(?<RawValue>(\\d|'|<:)[^\\/\\/\\n]+)|(?<Identifier>\\w+)";

	namespace
	{
		using namespace RGL;

		void pure_test()
		{

			std::string func = "GetActorValue('SingleWielding', Damage | Permanent) - GetActorValue('SingleWielding', Damage | Permanent)";
			//std::string func = "label_1; GetActorValue(::ending = 'SingleWielding', ActorValue::Damage | ActorValue::Permanent); return result";

			RGL::Record function("Commons");
			function.CreateData("GetWieldingWeight");
			//I think I'll use hex for the code, just in case it grows that much in size. Keeps the space usage down.
			Record& code_record = function.CreateChild("@code0000");

			code_record.CreateData(func, routine_regex);
			//code_record.CreateData(func, seq_routine_regex);//This is what would be used if it was an array instead of a string.

			Record& params_required = function.CreateChild("params_required");

			////Would that this had parameters, this is what it would do.
			////This data represents the number of regular parameters
			//params_required.CreateData("1");
			//params_required.CreateChild("param1OrWhateverTheNameIs").CreateData("", property_regex);
			//Record& params_default = function.CreateChild("params_default");
			////This data represents the number of default parameters
			//params_default.CreateData("1");
			//params_default.CreateChild("param2OrWhateverTheNameIs").CreateData("", property_regex);

		}
	}


	//Should the goal be to produce a record, or produce a thing?
	// For now, nothing.
	
	void HandleRecord_Routine(std::string name, const FileNode& node)
	{
		//TODO: Todos for the toml handling function below. Status: Undetermined
		// I think I would prefer it if the file was made before we get to the file node personally.
		
		//First things this should do, make sure the object name can go somewhere, make sure it's not reserved, then make sure that that
		// the object is actually a table, and we are a go.
		auto& table = *node.as_table();
		

		Record& routine = FileManager::EmplaceFileRecord("TempSpot", name);
		//Right here in data, the first object will be the scope name of the record.


		std::string formula_code = table["formula"].value_or("0");

		std::list <std::string> memory{};

		auto param_view = table["params"];

		
		if (auto p_test = param_view.as_array(); p_test)
		{
			Record& req_params = routine.CreateChild("parameters_required");
				
			//If def params exist, that's where things must go, and if they don't that's a parsing error.
			Record* def_params = nullptr;

			auto& param_array = *p_test;
				

			for (auto& entry : param_array)
			{
				//Right here is the place for that handle multiple entries deally, BUT, this is not where it will be handling that properly.
				if (entry.is_string() == false) {
					throw ParsingError("parameters only accept string types.");
				}

				std::string str = entry.value_or("");

				//May create a static function to do this, might get messy.
				Record parameter{ str, property_regex.data() };

				

				//Visit right here.
				Parser::Handle(parameter, memory, "Function");

				if (parameter.FindData("\\w+:") != nullptr)
				{
					if (!def_params)
						def_params = &routine.CreateChild("parameters_default");

					def_params->PushBackChild(parameter);
				}
				else
				{
					if (!def_params)
						req_params.PushBackChild(parameter);
					else
						//ROMBUR: Needs to submit to error names.
						throw ParsingError("non-default declared after default params in... something");
				}
			}

		}

		if (table["default"])
		{
			//This does different things based on what type this is, so I'm gonna put this to the side. For now.
			//formula->defaultValue = table["default"].value_or(0);
		}
			
		//Code is stored in a record called code_body. This can range from having only one entry. The group record will hold the needed
		// data to tell whether it's a single line or sequence, with such data being held in it's tag as a boolean.

		//TODO: Parser needs to force the differences between native and non-native code.
		if (auto code = table["formula"]; code)
		{
			Record& body = routine.CreateChild("code_body");
			
			std::function list_handle = [&](toml::array list)
			{
				body.CreateData("+");
				
				std::vector<std::string> code_list{ list.size() };

				std::transform(list.begin(), list.end(), code_list.begin(),
					[](toml::node& entry) 
					{ 
						if (entry.is_string() == false)
							throw ParsingError("Code body within array must only use toml strings.");
						//Could do this another way?
						return entry.as_string()->get();
					}
				);

				body.CreateChildren("code_", code_list, seq_routine_regex.data());
					
				//body.GetChildFirst()->GetTag() = "code_entry";
				body.GetChildBack()->GetTag() = "code_return";//Only return needs to be named like this.

				//visit children right here.
				//Keeping in case.
				//for (auto& entry : list)
				//{
				//	//Right here is the place for that handle multiple entries deally, BUT, this is not where it will be handling that properly.
				//	if (entry.is_string() == false) {
				//		throw ParsingError("Code body within array must only use toml strings.");
				//	}
				//	//Need a function to do this specically.
				//		
				//	body.PushBackChild(Record{ entry.value_or(""), seq_routine_regex.data() });
				//}
			};

			std::function string_handle = [&](std::string str)
			{
				body.CreateData("-");
				//Name doesn't matter here.
				body.CreateChildWithData("", str, seq_routine_regex.data());
			};

			HandleNodeView("No name currently", code,
				list_handle,
				string_handle
			);
			//Why this is like this, I have no idea. I
			auto test = [&](Record* visitee, RecordIterator& iterator, RecordIterator end)
			{
				//It's notable that right here, this is where something would use memory
				Parser::Handle(*iterator, memory, "Function");
			};

			std::function func = test;
			GenericVisitor c_body_visitor{ func, nullptr };

			body.VisitChildren(&c_body_visitor);
		}
			
		//auto* flag_array = table["flags"].as_array();
		//if (flag_array)
		//{
		//	auto result = std::find_if(flag_array->cbegin(), flag_array->cend(), [](auto& it) {return it.value_or("") == "native"; });
		//	if (flag_array->end() == result)
		//		formula = CreateFormula(parameters);
		//}


		//End of the record.

		//At this point, the toml has been processed, now what needs to happen is the creation of an object that can handle a record.
		//This type needs to be able to be given a record, and construct itself from the data given. So for example.

		//auto routine = new Coroutine();
		//routine->HandleRecords(routine);

		//And then that would be the end of it.


		
	}
}
//*/

//Disabled for now.
//#undef TOKEN
//#undef SYNTAX