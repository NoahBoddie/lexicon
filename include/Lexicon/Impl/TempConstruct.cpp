#pragma once

#include "TempConstruct.h"

#include "RoutineCompiler.h"
#include "Expression.h"
#include "LiteralManager.h"
#include "OperatorType.h"




#include "RuntimeVariable.h"
#include "Solution.h"

#include "Instruction.h"
#include "InstructionType.h"
#include "Runtime.h"
#include "TypePolicy.h"
//Move me you idiot.

#include "Scope.h"

#include "IdentityManager.h"


#ifndef gbirhyigo
//*
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

			if constexpr (Assign) {
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
			runtime->AdjustStackPointer(StackPointer::Argument, a_lhs.Get<Differ>());
		}

		static void IncVarStack(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{

			logger::critical(STRINGIZE(CONCAT(__hit, __COUNTER__)));
			std::system("pause");

			//Pretty simple honestly. Increase by the amount.
			runtime->AdjustStackPointer(StackPointer::Variable, a_lhs.Get<Differ>());
			logger::critical("exit out");
		}


		static void DefVarPolicy(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{ 
			logger::critical(STRINGIZE(CONCAT(__hit, __COUNTER__)));
			std::system("pause");

			RuntimeVariable& var = runtime->GetVariable(a_lhs.Get<Index>());
			AbstractTypePolicy* policy = a_rhs.Get<ITypePolicy*>()->GetTypePolicy(runtime);
			
			//if no policy, fatal fault
			if (!policy)
			{
				RGL_LOG(critical, "no policy");
				throw nullptr;
			}
		
			var = policy;
		}

		static void Push(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{
			//This is deprecated at this point,
			//Not much else needs to be known.
			// The destination is upto the operation out.
			
			//Push is going to become move. There's no merit in this being as it is.
			ret = a_lhs.GetVariable(runtime);
		}


		static void Move(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{
			//Moves the right hand into the left hand.

			if (a_lhs.type == OperandType::Literal) {
				//This is hard enforced because this will impact so much more than this function.
				RGL_LOG(critical, "Move attempted to set literal value.");
				throw nullptr;
			}

			//This is deprecated at this point,
			//Not much else needs to be known.
			// The destination is upto the operation out.
			a_lhs.ObtainVariable(runtime).Ref() = a_rhs.GetVariable(runtime).Ref();

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
			RGL_LOG(trace, "Ret done");
		}

		static void DropStack(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{
			//Drop should converge with a climb as well.

			//Rename to drop runtime pointer
			int64_t mode = a_rhs.Get<Differ>();


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

			runtime->AdjustStackPointer(StackPointer::Runtime, a_lhs.Get<Index>());
		}

		static void JumpStack(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{
			//Canning this because I think drop is likely going to be a better solution, but I'll wait until I can compile some shit.


			int64_t mode = a_rhs.Get<Differ>();


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

			runtime->SetStackPointer(StackPointer::Runtime, a_lhs.Get<Index>());
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
			//Similar to the way operators and directives are seperated, I'd seek to have 2 types of operator generators,
			// one that's basic and allows one to create values after it, and another that needs them raw


			//No need to deviate for now.
			//InstructType type = OperatorToInstruction(op);

			ITypePolicy* policy;
			

			Number::Settings l_settings = Number::Settings::CreateFromID(lhs.policy->GetTypeID());
			Number::Settings r_settings = Number::Settings::CreateFromID(rhs.policy->GetTypeID());

			//for certain types like boolean, this will not fly. But for most, this works.
			switch (op)
			{
				//These all produce booleans. Might need to look into this a bit more but I do not care.
			case InstructType::GreaterOrEqual:
			case InstructType::GreaterThan:
			case InstructType::LesserThan:
			case InstructType::LesserOrEqual:
			case InstructType::EqualTo:
			case InstructType::EqualAbsTo:
			case InstructType::NotEqualAbsTo:
			case InstructType::NotEqualTo:
			case InstructType::LogicalOR:
			case InstructType::LogicalAND:
				policy = nullptr;
				break;

			default:
				policy = l_settings < r_settings ? lhs.policy : rhs.policy;
			}

			//confirm here that the choosen thing is proper.


			//The problem with this is if the comparison between the 2 solutions uses registers.
			// I can't reach that.


			compiler->GetOperationList().push_back(Operation{ op, out, lhs, rhs });


			logger::trace("<!> inst {}, result policy {}", compiler->GetOperationList().back()._instruct,
				(uint32_t)policy->GetTypeID());
			return Solution{ policy, OperandType::Register, out };
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
				

				//TODO: If left and right are 2 literals register moving isn't super required.
				if (prefered == Register::Result && lhs.Equals<OperandType::Register>(Register::Left) == true)
				{
					//This will make it so the lhs uses the left register only is when it will move it.
					// If I can, I'd like to make it so if the rhs doesn't need the left hand reg, it wont do this either.
					// but that will require injecting.
					
					//This should actually be pushing what's in the lhs into the result register, then
					// making the lh solution the result register.
					//Operation free_reg{ InstructType::Push, Register::Result, lhs };



					//free_reg._instruct = InstructType::Push;
					//free_reg._lhs = reg1;
					//free_reg._out = Register::Result;

					//This statement will become more properly invalid once when I obsure the current vector. Address later.
					//compiler->GetOperationList().emplace_back(free_reg);
					
					//TODO: Make the process of making a mutating a solution by using Move to a different place a function
					compiler->GetOperationList().emplace_back(InstructType::Push, Register::Result, lhs);

					lhs = Solution{ lhs.policy, OperandType::Register, Register::Result };
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
			Solution sol{ result->GetPolicy(), OperandType::Literal, result };
			RGL_LOG(trace, "test2 {}", (uint32_t)sol.policy->GetTypeID());
			return sol;
		}

		Solution VariableProcess(ExpressionCompiler* compiler, Record& target)
		{
			RGL_LOG(critical, "Var__1");
			VariableInfo* var = compiler->GetScope()->GetVariable(target.GetTag());
			
			if (!var) {
				RGL_LOG(critical, "Cannot find variable '{}'.", target.GetTag());
				throw nullptr;
			}
			RGL_LOG(critical, "Var__2");
			Solution result{ var->type, OperandType::Index, var->GetFieldIndex() };
			RGL_LOG(critical, "Var__3");
			//TODO: BIG NOTE, the resulting solution should note that it's a reference type.


			return result;
		}



		void VarDeclareProcess(RoutineCompiler* compiler, Record& target)
		{

			//TODO: Readdress VarDeclareProcess. Also might be an expression?

			Record* type_name = target.FindChild("type");
			
			if (!type_name)
			{
				RGL_LOG(critical, "kill");
				throw nullptr;
			}

			//this is omega temp instead of this, getting the expression of a type would be prefered.
			if (type_name->GetChild(0).GetTag() != "int")
			{
				RGL_LOG(critical, "kill2");
				throw nullptr;
			}
			


			//This shit is even dumber than the above. Please redo.
			constexpr auto offset = Number::Settings::GetOffset(NumeralType::Floating);
			ITypePolicy* policy = IdentityManager::GetTypeByID(offset + 1);//TODO: actually search types pls
			
			if (!policy)
			{
				RGL_LOG(critical, "kill3");
				throw nullptr;
			}

			RGL_LOG(critical, "why error");
			size_t var_index = compiler->GetScope()->CreateVariable(target.GetTag(), policy);	
			
			if (Record* definition = target.FindChild("def"); definition) {
				Solution result = compiler->CompileExpression(definition->GetChild(0), Register::Result);
				//Operation free_reg{ InstructType::Move, Operand{var_index, OperandType::Index}, result };
				compiler->GetOperationList().emplace_back(InstructType::Move, Operand{ var_index, OperandType::Index }, result);
			}

			
		}




		void BlockProcess(RoutineCompiler* compiler, Record& target)
		{
			compiler->CompileBlock(target);
		}



		void ReturnProcess(RoutineCompiler* compiler, Record& target)
		{
			if (target.size() != 0)
			{
				Solution result = compiler->PushExpression(target.GetChild(0), Register::Result);

				//Right here the solutions given type should be evaluated to see if a correct type is being returned.

				//TODO: if the solution isn't in a register that is the result register, push the solution into 
				// the return. Also check type.
				//compiler->GetOperationList().push_back(ret_op);

				//left broken because I just realized even non-abstract types are going to have to worry about
				// inheritence too.
				ITypePolicy* return_policy = compiler->GetReturnType();
				//Basically, if one doesn't exist, and they aren't both just void.
				//TODO: Actually use void for this, at no point should null be used here. Such would be a statement.
				if (!return_policy && return_policy != result.policy) {
					RGL_LOG(critical, "Expecting return value but value is found.");
					throw nullptr;
				}


				ICallableUnit* out = nullptr;

				//TODO: Currently ReturnProcess use equivalence to handle this.
				if (result.policy->IsConvertibleTo(return_policy) == false)
				{
					RGL_LOG(critical, "Expression not convertible to return type.");
					throw nullptr;
				}

				if (out){
					//If out exists, this means there's something that can be used to convert it. 
					// however, this does NOT work when this conversion needs to be baked into the function.

					//Now granted, because this is in real time, I can just make an instruction handle this.
					// But that instruction would have to do it over and over and over again.
					
					
					//So
				}


			}
			



			//Operation ret_op{ InstructionType::Return };

			//This part should probably say what left and right situation should be on display.

			compiler->GetOperationList().emplace_back(InstructionType::Return);
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

			generatorList[SyntaxType::VarDeclare] = VarDeclareProcess;
			generatorList[SyntaxType::VarUsage] = VariableProcess;


			instructList[InstructType::Push] = InstructWorkShop::Push;
			instructList[InstructType::Move] = InstructWorkShop::Move;
			instructList[InstructType::Return] = InstructWorkShop::Ret;
			instructList[InstructType::JumpStack] = InstructWorkShop::JumpStack;
			instructList[InstructType::IncArgStack] = InstructWorkShop::IncArgStack;
			instructList[InstructType::IncVarStack] = InstructWorkShop::IncVarStack;
			instructList[InstructType::DefineVarPolicy] = InstructWorkShop::DefVarPolicy;
			instructList[InstructType::DropStack] = InstructWorkShop::DropStack;


			instructList[InstructType::Addition] = InstructWorkShop::BinaryMath<std::plus<>, false>;
			instructList[InstructType::Subtract] = InstructWorkShop::BinaryMath<std::minus<>, false>;
			instructList[InstructType::Multiply] = InstructWorkShop::BinaryMath<std::multiplies<>, false>;
			instructList[InstructType::Division] = InstructWorkShop::BinaryMath<std::divides<>, false>;
			instructList[InstructType::Modulo] = InstructWorkShop::BinaryMath<std::modulus<>, false>;
			//We not ready for this one.
			//instructList[InstructType::LogicalNOT] = InstructWorkShop::UnaryMath<std::logical_not<void>>;
			instructList[InstructType::UnaryMinus] = InstructWorkShop::UnaryMath<std::negate<void>>;

			__init = 1;


			//IDENTITY MANAGER TEST
			IdentityManager::GenerateID("NUMBER", Number::Settings::length);
			

			constexpr auto offset = Number::Settings::GetOffset(NumeralType::Floating);

			//RGL_LOG(info, "numset lng:{} v off:{}", Number::Settings::length, offset);

			static TypePolicy* float64 = new TypePolicy{ "NUMBER", Number::Settings::GetOffset(NumeralType::Floating) };

			//RGL_LOG(info, "{} float64", (uint32_t)float64.GetTypeID());

			
			RGL_LOG(info, "{} float64?", (uint32_t)IdentityManager::GetTypeByID(offset + 1)->GetTypeID());
			//Read some shit here.
		};

	}
}

//*/

#endif