#include "Lexicon/Engine/TempConstruct.h"



#include "Lexicon/Engine/RoutineCompiler.h"
#include "Lexicon/Engine/Expression.h"
#include "Lexicon/Engine/LiteralManager.h"
#include "Lexicon/Engine/OperatorType.h"




#include "Lexicon/RuntimeVariable.h"
#include "Lexicon/Engine/Solution.h"

#include "Lexicon/Engine/Instruction.h"
#include "Lexicon/Engine/InstructionType.h"
#include "Lexicon/Engine/Runtime.h"
#include "Lexicon/Engine/ConcretePolicy.h"
//Move me you idiot.

#include "Lexicon/Engine/Scope.h"

#include "Lexicon/Interfaces/IdentityManager.h"

#include "Lexicon/Engine/OverloadInput.h"

#include "Lexicon/Engine/Declaration.h"


#include "Lexicon/Engine/VoidPolicy.h"

#include "Lexicon/Engine/CompileUtility.h"
#include "Lexicon/Engine/RuntimeUtility.h"

#include "Lexicon/Engine/parse_strings.h"

#include "Lexicon/Engine/FunctionInfo.h"

#include "Lexicon/ProcedureData.h"

#include "Lexicon/Impl/common_type.h"
#include "Lexicon/Impl/ref_wrapper.h"

#include "Lexicon/Number.h"



void TestFunction()
{

}

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

			//
			//The below needs to curb "class std::" from the below
			report::runtime::trace("{} {} {} = {}", back.PrintString(), typeid(Operatable).name(), front.PrintString(), result.PrintString());
			//logger::trace("{} {} {} = {}", back, typeid(Operatable).name(), front, result);

			//Why do I do this again?
			// ANSWER: because it literally will crash if I do not.
			//a_rhs = Variable{ result, back.GetOffset() > front.GetOffset() ? a_lhs->Policy() : a_rhs->Policy() };
			a_rhs = Variable{ result };

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
			report::runtime::trace("{} on {} = {}", typeid(Operatable).name(), back.PrintString(), result.PrintString());


			a_rhs = Variable{ result };

			return a_rhs;
		}

		template<class Operatable, StringLiteral Symbol>
		static RuntimeVariable BinaryCompare(RuntimeVariable& a_lhs, RuntimeVariable a_rhs, InstructType type, const Runtime*)
		{
			//Don't really need a unary compare much, it's basically just '!'. So I could use an std::conditional for it.

			//This covers basically most of the below stuff.
			//Number back = a_lhs->AsNumber();
			//Number front = a_rhs->AsNumber();

			Operatable op{};

			//bool result = op(back, front);
			bool result = op(a_lhs.Ref(), a_rhs.Ref());
			//The below needs to curb "class std::" from the below
			report::runtime::trace("{} {} {} = {}", a_lhs->PrintString(), Symbol.value, a_rhs->PrintString(), result);

			//a_rhs = Variable{ result, back.Cmp(front) > 0 ? nullptr : nullptr };
			a_rhs = Variable{ result };

			return a_rhs;
		}

		

		//Directives-These will have to be moved

		static void Call(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType instruct, Runtime* runtime)
		{
			//This is old and I forget how it's done.


			IFunction* itfc = a_lhs.Get<IFunction*>();

			AbstractFunction* func = itfc->GetFunction(runtime);

			Index count = a_rhs.Get<Index>();

			
			{//Needs to be scoped for now so args don't maintain references longer than they should
				std::vector<RuntimeVariable> args = runtime->GetArgsInRange(count);

				ret = func->Execute(args, runtime, nullptr);
			}
			//I may actually just include the decrement in here myself.

			if (count) {//Only needs to do this if it had arguments. Handles reference snags basically.
				runtime->AdjustStackPointer(StackPointer::Argument, -static_cast<int64_t>(count));
				
				//auto& arg_var = runtime->GetArgument(runtime->GetStackPointer(StackPointer::Argument) - count);
				//if (arg_var.IsRuntimeRef() == true)//Ideally, all call stuff is a reference. Im just checking 
				//	arg_var.Clear();
			}
		}

		static void Convert(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType instruct, Runtime* runtime)
		{
			//This is old and I forget how it's done.


			ICallableUnit* func = nullptr;

			std::vector<RuntimeVariable> from { a_rhs.GetVariable(runtime) };

			get_switch (a_lhs.type)
			{
				case OperandType::Function:
				{
					IFunction* itfc = a_lhs.Get<IFunction*>();
					
					func = itfc->GetFunction(runtime);
					
					

				}
				break;
			
				case OperandType::Callable:
				{
					func = a_lhs.Get<ICallableUnit*>();
				}
				break;

				case OperandType::Type:
				{
					auto& var = from[0];
					
					//This is the real solution, I just want to check if I was right.
					auto from_type = LEX::GetVariableType(var.Ref());
					//auto from_type = var->Policy();

					if (!from_type)
						report::runtime::critical("NO FROM");

					auto to_type = a_lhs.Get<ITypePolicy*>()->FetchTypePolicy(runtime);

					
					if (auto convert_result = from_type->IsConvertibleTo(to_type, from_type, nullptr, ConversionFlag::Explicit); convert_result > convertFailure)
					{
						ret = var.Ref();
						ret->SetPolicy(to_type->FetchTypePolicy(runtime));
					}
					else
					{
						ret = to_type->GetDefault();
					}
					
				}
				break;


				//If it's a type it will check if it's current type can be converted into the given thing. if not, it will return it as a null.

				default:
					report::runtime::critical("Invalid operand type detected. {}", magic_enum::enum_name(a_lhs.type));
					break;

			}
			
			if (func)
				ret = func->Execute(from, runtime, nullptr);
		}

		
		//Confirms that a reference promotion is valid
		static void CheckPromotion(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType instruct, Runtime* runtime)
		{
			RuntimeVariable variable = a_lhs.GetVariable(runtime);

			auto type = (Reference)a_rhs.Get<Index>();
			
			auto result = RunUtil::CantPromote(variable, runtime, type);

			switch (result)
			{
			case 0:
				break;

			case 1:
				report::runtime::error("Not reference, maybe ref can't be promoted");
				break;

			case 2:
				report::runtime::error("Reference located in current runtime, local ref can't be promoted");
				break;

			case 3:
				report::runtime::error("Reference located within a runtime, scoped ref can't be promoted");
				break;

			default: 
				report::runtime::error("Ref promotion error '{}' unknown.", result);
				break;
			}
		
		}


		//void(*)(RuntimeVariable&, Operand, Operand, InstructType, Runtime*);

		//TODO: Merge mod arg stack and mod var stack, they roughly do the same thing.

		static void ModArgStack(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{
			//Pretty simple honestly. Increase by the amount.
			runtime->AdjustStackPointer(StackPointer::Argument, a_lhs.Get<Differ>());
		}

		static void ModVarStack(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{

			auto dif = a_lhs.Get<Differ>();

			//Pretty simple honestly. Increase by the amount.
			runtime->AdjustStackPointer(StackPointer::Variable, dif);
		}
		

		static void DefineVar(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{ 
			//logger::critical(STRINGIZE(CONCAT(__hit, __COUNTER__)));
			
		
			//RuntimeVariable& var = runtime->GetVariable(a_lhs.Get<Index>());
			RuntimeVariable& var = a_lhs.AsVariable(runtime);
			AbstractTypePolicy* policy = a_rhs.Get<ITypePolicy*>()->FetchTypePolicy(runtime);
			
			//if no policy, fatal fault
			if (!policy){
				report::runtime::critical("no policy found.");
			}

			

			var = policy->GetDefault();

		}

		static void DefineParam(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{
			//While this may seem useless, this prevents things like where a float is given where a number is expected, causing any giving of a non-float number
			// to result in an error within assign.

			RuntimeVariable& var = a_lhs.AsVariable(runtime);//s runtime->GetVariable(a_lhs.Get<Index>());
			AbstractTypePolicy* policy = a_rhs.Get<ITypePolicy*>()->FetchTypePolicy(runtime);

			//if no policy, fatal fault
			if (!policy) {
				report::runtime::critical("no policy found.");
			}

			logger::debug(" index of set {}", a_lhs.Get<Index>());

			var->SetPolicy(policy);

		}
		


		static void Transfer(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType instruct, Runtime* runtime)
		{
			//Moves the right hand into the left hand.

			if (a_lhs.type == OperandType::Literal) {
				//This is hard enforced because this will impact so much more than this function.
				report::fault::critical("Transfer attempted to set literal value.");
			}



			//Note, these likely are reversed. Move replaces, forward copies.
			switch (instruct)
			{
			case InstructType::Reference://Actually reference
				//Attempts to get a reference to the variable and will present a copy if not possible (think literals and such).
				// Best used on a call target, or when a parameter is a reference
				a_lhs.ObtainAsVariable(runtime).AssignRef(a_rhs.GetVariable(runtime));
				break;

			case InstructType::Move:
				//Similar copy, but moves the resources
				// Best used when pulling something from a register
				a_lhs.ObtainVariable(runtime)->Assign(std::move(a_rhs.GetVariable(runtime).Ref()));
				break;
			
			case InstructType::Forward:
				//Sets the lhs to be the exact type as the left hand side.
				// best used when it's unknown whether the rhs is a reference or not
				a_lhs.ObtainAsVariable(runtime).AssignRef(a_rhs.AsVariable(runtime));
				break;


			case InstructType::ForwardMove:
				//Same as Forward but uses a move instead
				// best used when it's unknown whether the rhs is a reference or not, AND when coming from a temporary location such as a register
				a_lhs.ObtainAsVariable(runtime).AssignRef(std::move(a_rhs.AsVariable(runtime)));
				break;

			case InstructType::Copy:
				//Copies the value of the rhs into the lhs.
				// Best used when not transfering between registers and references aren't involved
				
				
				//a_lhs.ObtainVariable(runtime)->Assign(a_rhs.CopyVariable(runtime));
				a_lhs.ObtainVariable(runtime)->Assign(a_rhs.GetVariable(runtime).Ref());//Trying it this way because it doesn't need to copy
				break;

			default:
				report::runtime::critical("Instruct type {} not accounted for.", magic_enum::enum_name(instruct));
				break;
			}
		}


		//Test is probably deprecated.
		static void Test(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{
			//Not actually sure how this would work, the idea is basically that I implicit bool cast to it, and output the result
			// to the process qualifiers.



			//bool value = process->GetVariable(a_lhs).GetImplicit<ValueType::Boolean>();
			//process->qualifiers.Set(ProcessFlags::TestBit, value);
		}

		static void Ret(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{
			//This is going to have to have an official function to handle this
			runtime->_flags.Set(RuntimeFlag::RetBit, true);
			report::runtime::trace("Routine returned");
		}

		static void DropStack(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType type, Runtime* runtime)
		{
			//Drop should converge with a climb as well.

			if (a_rhs.type != OperandType::None) {
				Number num = a_rhs.GetVariable(runtime)->AsNumber();
				
				bool be_true;

				switch (type)
				{
				case InstructType::DropStack:
					be_true = true; break;
				
				case InstructType::DropStackN:
					be_true = false; break;
				}

				if (num.Visit([be_true](auto it) { return be_true ? it == 0 : it != 0; }) == true)
					return;
			}

			//Rename to drop runtime pointer
			int64_t move = a_lhs.Get<Differ>();


			runtime->AdjustStackPointer(StackPointer::Runtime, move);
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



		static void Convert_OLD(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{

			RuntimeVariable from = a_rhs.GetVariable(runtime);
			AbstractTypePolicy* to = a_lhs.Get<ITypePolicy*>()->FetchTypePolicy(runtime);
			//from->
		}

		static void __GenericCtor(RuntimeVariable& result, Variable* target, std::vector<Variable*> args, ProcedureData& data)
		{
			FunctionBase* base = const_cast<FunctionBase*>(dynamic_cast<const FunctionBase*>(data.function));

			if (!base)
				report::runtime::critical("No function base on source function");

			Environment* env = base->GetEnvironment();

			//Have a AsScript/AsPolicy possibly?

			ConcretePolicy* policy = dynamic_cast<ConcretePolicy*>(env);

			result = policy->GetDefault();
		}


		static void Construct(RuntimeVariable& result, Operand a_lhs, Operand, InstructType, Runtime* runtime)
		{
			
			AbstractTypePolicy* policy = a_lhs.Get<ITypePolicy*>()->FetchTypePolicy(runtime);

			if (!policy)
				report::runtime::critical("No policy could be fetched.");

			result = policy->GetDefault();

			logger::critical("type is in result? {}, is void {}", !!result->Policy(), result->IsVoid());
		}


	private:


		static void DetachRef(RuntimeVariable& result, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
		{
			if (a_lhs.type == OperandType::Literal || a_rhs.type == OperandType::Literal) {
				//This is hard enforced because this will impact so much more than this function.
				report::runtime::critical("DetachRef attempted to set literal value.");
			}

			auto& variable = a_lhs.AsVariable(runtime);

			//This second one is checking for local
			if (!variable->IsPolymorphic() && !variable->IsPolymorphic())
			{
				decltype(auto) out = variable.Detach();

				if (a_rhs.type != OperandType::None) {
					
					a_rhs.AsVariable(runtime) = std::move(out);
				}
			}
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
		//The main function that uses the above is effectively processing ranges. Remember that.

		//The idea of this this is what interprets OPerators and pushes back instructions for the instruct list to use.
		//TODO: (MakeManager) The Operator Constructor list
		

		InstructType GetOperatorType(SyntaxRecord& target)
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
				target.Note("construct symbol '=='");
				logger::debug("construct symbol '=='");
				return InstructType::EqualTo;

			case "==="_h:
				logger::debug("construct symbol '==='");
				return InstructType::EqualAbsTo;

			case "^"_h:
			case "XOR"_h:
				logger::debug("construct symbol '^' / 'OR'");
				return InstructType::BitwiseXOR;


			case "^^"_h:
			case "pow"_h:
				logger::debug("construct symbol '^' / 'pow'");
				return InstructType::Exponent;

			case "*"_h:
				logger::debug("construct symbol \'*\'");
				return binary ? InstructType::Multiply : InstructType::Promote;

			case "+"_h:
				logger::trace("construct symbol '+' ({})", binary ? "binary" : "unary");
				return binary ? InstructType::Addition : InstructType::UnaryPlus;

			case "-"_h:
				logger::trace("construct symbol \'-\'");
				return binary ? InstructType::Subtract : InstructType::UnaryMinus;

			case "/"_h:
				logger::trace("construct symbol \'/\'");
				return InstructType::Division;

			//case "**"_h:
			//	logger::trace("construct symbol \'^\'");
			//	return InstructType::Exponent;

			case "%"_h:
				logger::trace("construct symbol \'%\'");
				return InstructType::Modulo;

			case "."_h:
				logger::trace("construct symbol \'.\'");
				return InstructType::Access;

			case "="_h:
				logger::trace("construct symbol \'=\'");
				return InstructType::Assign;//Assign is not a real instruction as it turns out, transfer gets used in it. But it works 2 jobs ig

			case "=>"_h:
			case "then"_h:
				logger::trace("construct symbol \'then/=>\'");
				return InstructType::Then;

			default:
				//ARTHMETIC_LOGGER(info, "Failure to handle operator, value \'{}\'", data->view[0]);
				return InstructType::Invalid;
			}
		}




		Solution BasicUnaryGenerator(ExpressionCompiler* compiler, InstructType op, Solution it, Register out)
		{
			//Similar to the way operators and directives are seperated, I'd seek to have 2 types of operator generators,
			// one that's basic and allows one to create values after it, and another that needs them raw


			//No need to deviate for now.
			//InstructType type = OperatorToInstruction(op);

			ITypePolicy* policy;

			assert(it.policy);

			Number::Settings it_settings = Number::Settings::CreateFromID(it.policy->GetTypeID());



			//constexpr auto bool_settings = Number::Settings::CreateFromType<bool>();

			//for certain types like boolean, this will not fly. But for most, this works.
			switch (op)
			{
			
			case InstructType::LogicalNOT:
				//I will make no checks here for now, but I want this to make sure that the two things are convertible.
				// a helper function should help with this.

				policy = common_type::boolean();
				break;

			case InstructType::UnaryMinus:
			default:


				if (!it_settings) {
					report::compile::error("Unary operand must be a number, '{}' found.", it->GetName());
				}
				policy = it.policy;
				break;
			}

			//confirm here that the choosen thing is proper.


			//The problem with this is if the comparison between the 2 solutions uses registers.
			// I can't reach that.


			compiler->GetOperationList().push_back(Operation{ op, out, it, it });


			logger::trace("<!> inst {}, result policy {}", compiler->GetOperationList().back()._instruct,
				(uint32_t)policy->GetTypeID());
			return Solution{ QualifiedType{policy}, OperandType::Register, out };
		}


		Solution BasicBinaryGenerator(ExpressionCompiler* compiler, InstructType op, Solution lhs, Solution rhs, Register out)
		{
			//Similar to the way operators and directives are seperated, I'd seek to have 2 types of operator generators,
			// one that's basic and allows one to create values after it, and another that needs them raw


			//No need to deviate for now.
			//InstructType type = OperatorToInstruction(op);

			ITypePolicy* policy;

			assert(lhs.policy);
			assert(rhs.policy);

			Number::Settings l_settings = Number::Settings::CreateFromID(lhs.policy->GetTypeID());
			Number::Settings r_settings = Number::Settings::CreateFromID(rhs.policy->GetTypeID());



			//constexpr auto bool_settings = Number::Settings::CreateFromType<bool>();

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
				
				//I will make no checks here for now, but I want this to make sure that the two things are convertible.
				// a helper function should help with this.
				
				policy = common_type::boolean();
				break;

			case InstructType::Modulo:
				if (l_settings.IsInteger() == false || r_settings.IsInteger() == false) {
					report::compile::error("Both operands must be a number.", "Left", lhs->GetName());
				}
				
				policy = common_type::integer64();
				break;

			default:
				

				if (!l_settings) {
					report::compile::error("{} operand must be a number, '{}' found.", "Left", lhs->GetName());
				}
				if (!r_settings) {
					report::compile::error("{} operand must be a number, '{}' found.", "Right", rhs->GetName());
				}

				policy = l_settings > r_settings ? lhs.policy : rhs.policy;
				break;
			}

			//confirm here that the choosen thing is proper.
			

			//The problem with this is if the comparison between the 2 solutions uses registers.
			// I can't reach that.


			compiler->GetOperationList().push_back(Operation{ op, out, lhs, rhs });


			logger::trace("<!> inst {}, result policy {}", compiler->GetOperationList().back()._instruct,
				(uint32_t)policy->GetTypeID());
			return Solution{ QualifiedType{policy}, OperandType::Register, out };
		}


		struct OpProcessors
		{
			static Solution GenericProcess(ExpressionCompiler* compiler, SyntaxRecord& target, InstructType op)
			{
				//TODO:Note, this operator stuff WILL fail when it comes to member access, since it needs
				// what's on the left to process what's on the right. In this specific instance, I'm going to make 
				// something else handle thing

				//Honestly, in general, maybe this should be taking control a bit less. Consider a set up
				// like how the functional part of operators work, 2 template to do all the jobs.

				//Should be an expression compiler but I'm pretending that it's a routine compiler.
				Register prefered = compiler->GetPrefered();

				//OperatorType op = GetOperatorType_Old(target);
				//InstructType op = GetOperatorType(target);

				//if (op == OperatorType::Invalid) {
				if (op == InstructType::Invalid) {
					
					target.LogCritical("Invalid instruction for operation.");
					report::compile::error("Invalid instruction for operation.");
				}

				Solution lhs{};
				Solution rhs{};

				if (target.SYNTAX().type == SyntaxType::Binary)
				{
					//Which is done first should be handled by which is more complex
					std::string str1 = parse_strings::lhs;
					std::string str2 = parse_strings::rhs;

					Register reg1 = Register::Left;
					Register reg2 = Register::Right;

					SyntaxRecord& left = target.FindChild(str1)->GetChild(0);
					
					lhs = compiler->CompileExpression(left, reg1);
					


					//TODO: If left and right are 2 literals register moving isn't super required.
					if (prefered == Register::Result && lhs.Equals<OperandType::Register>(Register::Left) == true)
					{
						//This will make it so the lhs uses the left register only is when it will move it.
						// If I can, I'd like to make it so if the rhs doesn't need the left hand reg, it wont do this either.
						// but that will require injecting.

						//This should actually be pushing what's in the lhs into the result register, then
						// making the lh solution the result register.

						//compiler->GetOperationList().emplace_back(InstructType::Push, Register::Result, lhs);
						//lhs = Solution{ lhs.policy, OperandType::Register, Register::Result };

						compiler->GetOperationList().push_back(CompUtil::Mutate(lhs, Operand{ Register::Result, OperandType::Register }));
					}

					
					rhs = compiler->CompileExpression(target.FindChild(str2)->GetChild(0), reg2);



					//Do operation here. Needs solutions, outputs solution.
					//return operatorCtorList[op](compiler, op, lhs, rhs, prefered);
					return BasicBinaryGenerator(compiler, op, lhs, rhs, prefered);
					
				}
				else// if syntax == unary blah blah blah
				{
					//Unary is a lot simpler to deal with
					
					lhs = compiler->CompileExpression(target.GetChild(0), Register::Left);
					
					return BasicUnaryGenerator(compiler, op, lhs, prefered);
				}


				

			}


			static Solution AccessProcess(ExpressionCompiler* compiler, SyntaxRecord& target)
			{

				//TODO:Note, this operator stuff WILL fail when it comes to member access, since it needs
				// what's on the left to process what's on the right. In this specific instance, I'm going to make 
				// something else handle thing

				//Honestly, in general, maybe this should be taking control a bit less. Consider a set up
				// like how the functional part of operators work, 2 template to do all the jobs.

				//Should be an expression compiler but I'm pretending that it's a routine compiler.

				//----------------------------------------------

				//So the interesting thing about member access is it doesn't need to do one side, then do the other,
				// instead, all I've actually gotta do continue using the prefered register
				Register prefered = compiler->GetPrefered();


				SyntaxRecord& left = target.FindChild(parse_strings::lhs)->GetFront();
				SyntaxRecord& right = target.FindChild(parse_strings::rhs)->GetFront();



				Solution tar = compiler->CompileExpression(left, compiler->GetPrefered());

				//Something is wrong here, member access isn't getting the right thing and I haven't a clue why

				Solution result = compiler->CompileExpression(right, compiler->GetPrefered(), tar);



				return result;
			}


			static Solution AssignProcess(ExpressionCompiler* compiler, SyntaxRecord& target)
			{
				Register prefered = compiler->GetPrefered();

				std::string str1 = parse_strings::lhs;
				std::string str2 = parse_strings::rhs;


				Register reg1 = Register::Left;
				Register reg2 = Register::Right;

				SyntaxRecord& left = target.FindChild(str1)->GetFront();
				SyntaxRecord& right = target.FindChild(str2)->GetFront();



				Solution to = compiler->CompileExpression(left, prefered);

				if (to.IsReadOnly() == true) {

					report::compile::error("target solution is read only.");
				}

				if (prefered == Register::Result && to.Equals<OperandType::Register>(Register::Left) == true) {
					compiler->GetOperationList().push_back(CompUtil::Mutate(to, Operand{ Register::Result, OperandType::Register }));
				}




				//TODO: At a later point, the AssignProcess will need to use the constructor directly in cases of no-name constructor syntax
				Solution from = compiler->CompileExpression(right, reg2);


				//Conversion out;
				//auto convert = from.IsConvertToQualified(to, nullptr, &out);
				//if (convert <= ConvertResult::Failure) {
				//	report::compile::error("Cannot initialize. Error {}", magic_enum::enum_name(convert));
				//}
				//CompUtil::HandleConversion(compiler, out, from, to, convert);

				CompUtil::HandleConversion(compiler, from, to, target);

				//compiler->GetOperationList().push_back(CompUtil::MutateCopy(from, to));

				//return from;
				//TRANSFOR
				
				//compiler->GetOperationList().push_back(Operation{ InstructType::Move, to, from });
				compiler->GetOperationList().push_back(CompUtil::Mutate(from, to));

				return to;
			}




			static Solution ThenProcess(ExpressionCompiler* compiler, SyntaxRecord& target)
			{
				Register prefered = compiler->GetPrefered();

				//This is thrown away.
				Solution waste = compiler->CompileExpression(target.FindChild(parse_strings::lhs)->GetFront(), prefered);

				Solution result = compiler->CompileExpression(target.FindChild(parse_strings::rhs)->GetFront(), prefered);

				return result;
			}


			static Solution PromoteProcess(ExpressionCompiler* compiler, SyntaxRecord& target)
			{


				Solution result = compiler->CompileExpression(target.GetChild(0), compiler->GetPrefered());

				if (result.PromoteRefness() == false) {
					report::compile::error("'{}' Reference type cannot be promoted.", magic_enum::enum_name(result.reference));
				}

				return result;
			}
		};


		//RecordProcessors
		Solution OperatorProcess(ExpressionCompiler* compiler, SyntaxRecord& target)
		{
			InstructType op = GetOperatorType(target);

			//Check with the operator list that it can actually handle an operator type that's been gotten.

			switch (op)
			{
			case InstructType::Invalid:
				report::compile::error("Unhandled operator"); break;

			case InstructType::Access:
				return OpProcessors::AccessProcess(compiler, target);


			case InstructType::Assign:
				return OpProcessors::AssignProcess(compiler, target);
			
			case InstructType::Then:
				return OpProcessors::ThenProcess(compiler, target);
				
			case InstructType::Promote:
				return OpProcessors::PromoteProcess(compiler, target);

			default:
				return OpProcessors::GenericProcess(compiler, target, op);
			}
		}








		struct CondProcessors 
		{
			static void IfElseProcess(RoutineCompiler* compiler, SyntaxRecord& target)
			{
				//'if' <Syntax: Conditional (col: 5/ line: 84>
				//    '<:express:>' <Syntax: Header>
				//        'true' <Syntax: Boolean (col: 9/ line: 84>
				//    '<:statement:>' <Syntax: Header>
				//        '' <Syntax: Script (col: 0/ line: 0>
				//    '<:alt:>' <Syntax: Header>
				//        'false' <Syntax: Boolean (col: 11/ line: 88>

				//For the period of while each statement exists, that is a new scope that will exist.
				//This one exists for the possibility of making new variables within the if, but that isn't supported right now.
				//Scope a_scope{ compiler, ScopeType::Conditional };


				//*Slated for deletion
				//static ITypePolicy* boolean = nullptr;
				//if (!boolean) {
				//	//This may use a different boolean type eventually.
				//	boolean = IdentityManager::instance->GetTypeByOffset("NUMBER", GetNumberOffsetFromType<bool>());
				//}
				
				Solution query = compiler->CompileExpression(target.FindChild(parse_strings::expression_block)->GetFront(), compiler->GetPrefered());
				
				//Conversion out;
				//auto convert = query.IsConvertToQualified(QualifiedType{ boolean }, nullptr, &out);
				//if (convert <= ConvertResult::Failure) {
				//	report::compile::error("Cannot initialize. Error {}", magic_enum::enum_name(convert));
				//}
				//CompUtil::HandleConversion(compiler, out, query, convert);

				CompUtil::HandleConversion(compiler, query, QualifiedType{ common_type::boolean() }, target);

				auto& list = compiler->GetOperationList();

				std::vector<Operation> back_list;

				//TODO: I feel I should actually use an out on the dependent one, but do it first. By doing this, I can actually increase the jump
				// size, and reduce a cycle. Optimizations for AFTER this starts working again


				//This order is no longer an important thing, arrange at leisure.

				if (auto else_block = target.FindChild(parse_strings::alternate_block); else_block)
				{
					//NEW
					Scope scope{ compiler, ScopeType::Depedent, back_list };
					compiler->CompileBlock(*else_block);
					CompUtil::SkipScope(compiler, Operand::None(), false);

					if constexpr (0)
					{
						std::vector<Operation> ops;
						{
							//This set up will allow for us to include the deallocations included in the death of a scope.
							Scope a_scope{ compiler, ScopeType::Depedent, ops };

							//Allow for some variation here, if it's detected the else was an expression it should parse it like an expression,
							// if not, it should parse it like a block.
							//This needs to create a new scope to end, a place to place the 

							// ops = compiler->CompileBlock(*else_block);
						}
						//This is to skip over the else, this will not care about anything.

					
						auto back_size = (int64_t)ops.size();

						if (back_size) {

							back_list.emplace_back(InstructType::DropStack, Operand{ back_size + 1, OperandType::Differ }, Operand::None());

							back_list = std::move(ops);


						}
						
					}
				}



				//NEW
				Scope scope{ compiler, ScopeType::Conditional };
				compiler->CompileBlock(*target.FindChild(parse_strings::statement_block));
				//CompUtil::SkipScope(compiler, query, true, back_list.size());
				CompUtil::SkipScope(compiler, query, true, !back_list.empty());


				if (back_list.empty() == false){
					list.append_range(back_list);
				}
				
				//I feel like this should... do something?

				if constexpr (0)
				{
					std::vector<Operation> ops;
					{
						Scope a_scope{ compiler, ScopeType::Conditional, ops };

						//ops = compiler->CompileBlock(*target.FindChild(parse_strings::statement_block));
					}


					auto size = ops.size();

					if (size) {
						//We'll only place these if there's actually somethin

						list.emplace_back(InstructType::DropStackN, Operand{ (int64_t)size + 1, OperandType::Differ }, query);

						list.insert_range(list.end(), std::move(ops));
					}

					if (back_list.empty() == false)
						list.insert_range(list.end(), std::move(back_list));
				}
				
			}
		};


		void ConditionalProcess(RoutineCompiler* compiler, SyntaxRecord& target)
		{
			switch (Hash(target.GetTag()))
			{
				case "if"_h:
				//case "else"_h:
					return CondProcessors::IfElseProcess(compiler, target);
				default:
					report::compile::error("invalid conditional tag '{}' unable to be processed", target.GetTag());
			}
		}










		Solution LiteralProcess(ExpressionCompiler* compiler, SyntaxRecord& target)
		{
			//Combine with the use of variables.
			Literal result = LiteralManager::ObtainLiteral(target);

			//TODO: I think GetVariableType would be prefered here
			//Solution sol{ QualifiedType{ result->Policy(), Constness::Const }, OperandType::Literal, result };
			Solution sol{ QualifiedType{ result->GetVariableType(), Constness::Const }, OperandType::Literal, result };
			
			return sol;
		}

		Solution FieldProcess(ExpressionCompiler* compiler, SyntaxRecord& target)
		{
			//Currently, this cannot be handled
			QualifiedField var = compiler->GetScope()->SearchFieldPath(target);

			if (!var) {//Check if the var was this specifically.
				report::compile::error("Cannot find variable '{}'.", target.GetTag());
			}

			//XTOR
			//Solution result{ var->GetType(), OperandType::Index, var->GetFieldIndex() };
			Solution result = var.AsSolution();



			assert(result.policy);

			//TODO: BIG NOTE, the resulting solution should note that it's a reference type.


			return result;
		}


	
		


		Solution HandleCall_V2(ExpressionCompiler* compiler, SyntaxRecord& target)
		{

			//The argument check has to happen first.

			SyntaxRecord* arg_record = target.FindChild(parse_strings::args);

			if (!arg_record) {//If no args assume no args.
				report::compile::error("no args record in '{}' detected.", target.GetTag());
			}

			//This is used so that we know what size we're to after the fact, and place it at the head.
			// By default it starts with 1, so we don't have to resize when we add the last (and first) piece.
			//Due to realizing that it will still need to grow in a piece meal fashion, this is getting axed.
			//std::vector<Operation> ops{1};

			TargetObject* self = compiler->GetTarget();

			std::vector<Solution> args;
			std::vector<std::vector<Operation>> operations;

			size_t alloc_size = arg_record->size();


			args.resize(alloc_size);
			operations.resize(alloc_size);



			//At a later point, one will get the ability to define default arg out of order, via the below:
			// function(arg1, arg2, def_param4 = arg3);
			// This sort of thing is known as a default argument, and instead of processing it here,
			// it will process it later, when it is processing default arguments. 
			//std::vector<SyntaxRecord*> def_args;



			//I do the index this way in prep for explicit default arguments, where any default argument found
			// will NOT increment the index, instead storing the record for later use.
			//Additionally, I use get arg count so that the index being pushed
			//*Turns out, the I was not required.
			for (size_t i = 0; auto & arg : arg_record->children())
			{

				//Solution result = compiler->CompileExpression(arg, compiler->GetPrefered(), operations[i]);//, ops);
				Solution result = compiler->CompileExpression(arg, Register::Right, operations[i]);//, ops);

				//compiler->GetOperationList().push_back(CompUtil::Mutate(result, Operand{ compiler->ModArgCount(), OperandType::Argument }));

				args[i] = result;

				i++;
			}


			//'function' <Expression: Call>
			//		'args' <Expression: Header>


			OverloadInput input;
			input.object = self;
			input.paramInput = args;//can probably move here.

			Overload instructions;

			//Around here, you'd use the args.
			FunctionInfo* info = compiler->GetScope()->SearchFunctionPath(target, input, instructions);

			//TODO: Field check in CallProcess should probably use enum, but on the real, I'm too lazy.

			if (!info) {
				report::compile::error("'{}' Not found. Could be either invalid overload or incorrect name. Needs more details.", target.GetTag());
			}

			FunctionBase* func = info->Get();

			if (!func) {
				report::compile::error("No callable for info at '{}' detected.", target.GetTag());
			}

			//Other checks should occur here, such as is static to determine how many arguments will be loaded.

			size_t req_args = func->GetArgCountReq();


			if constexpr (1)
			{
				if (args.size() < req_args) {
					report::compile::error("Requires {} arguments for '{}', only {} submitted.", req_args, target.GetTag(), args.size());
				}

				if (func->GetTargetType().policy != nullptr) {
					//Increase the allocation size to include the "this" argument.
					//alloc_size++;
				}
			}

			bool has_tar = func->GetTargetType();

			alloc_size += instructions.defaults.size() + has_tar;

			auto& list = compiler->GetOperationList();


			auto start = compiler->ModArgCount(alloc_size);


			if (func->GetTargetType().policy != nullptr) {
				//This will push itself into the arguments, but it will only be used under certain situations.
				list.push_back(CompUtil::MutateRef(*self->target, Operand{ start, OperandType::Argument }));
			}


			for (size_t i = 0; i < args.size(); i++)
			{
				auto& o_entry = instructions.given[i];
				auto& arg = args[i];
				auto& ops = operations[i];

				list.append_range(std::move(ops));

				//This should basically already be successful, no real need for checks
				CompUtil::HandleConversion(compiler, o_entry.convert, arg, o_entry.type, o_entry.convertType, Register::Right);

				//list.push_back(CompUtil::Mutate(arg, Operand{ start + i + has_tar, OperandType::Argument }));
				list.append_range(CompUtil::MutateLoad(arg, Operand{ start + i + has_tar, OperandType::Argument }, o_entry.type.IsReference()));
			}

			//default is dealt with here.


			size_t dealloc_size = alloc_size;

			list.emplace_back(InstructType::Call, compiler->GetPrefered(),
				Operand{ func, OperandType::Function },
				Operand{ alloc_size, OperandType::Index });

			compiler->ModArgCount(-static_cast<int64_t>(alloc_size), false);



			return Solution{ func->GetReturnType(), OperandType::Register, compiler->GetPrefered() };
		}



		Solution HandleCall(ExpressionCompiler* compiler, SyntaxRecord& target)
		{

			//The argument check has to happen first.

			SyntaxRecord* arg_record = target.FindChild(parse_strings::args);

			if (!arg_record) {
				report::compile::error("no args record in '{}' detected.", target.GetTag());
			}

			//This is used so that we know what size we're to after the fact, and place it at the head.
			// By default it starts with 1, so we don't have to resize when we add the last (and first) piece.
			//Due to realizing that it will still need to grow in a piece meal fashion, this is getting axed.
			//std::vector<Operation> ops{1};

			TargetObject* self = compiler->GetTarget();

			std::vector<Solution> args;

			size_t alloc_size = arg_record->size();
			size_t dealloc_size = alloc_size;

			args.resize(alloc_size);



			if (self) {
				//This will push itself into the arguments, but it will only be used under certain situations.
				compiler->GetOperationList().push_back(CompUtil::MutateRef(*self->target, Operand{ compiler->ModArgCount(), OperandType::Argument }));
				dealloc_size++;
			}

			//At a later point, one will get the ability to define default arg out of order, via the below:
			// function(arg1, arg2, def_param4 = arg3);
			// This sort of thing is known as a default argument, and instead of processing it here,
			// it will process it later, when it is processing default arguments. 
			//std::vector<SyntaxRecord*> def_args;



			//I do the index this way in prep for explicit default arguments, where any default argument found
			// will NOT increment the index, instead storing the record for later use.
			//Additionally, I use get arg count so that the index being pushed
			//*Turns out, the I was not required.
			for (size_t i = 0; auto & arg : arg_record->children())
			{

				Solution result = compiler->CompileExpression(arg, compiler->GetPrefered());//, ops);

				compiler->GetOperationList().push_back(CompUtil::Mutate(result, Operand{ compiler->ModArgCount(), OperandType::Argument }));

				args[i++] = result;
			}


			//'function' <Expression: Call>
			//		'args' <Expression: Header>


			OverloadInput input;
			input.object = self;
			input.paramInput = args;

			Overload instructions;

			//Around here, you'd use the args.
			FunctionInfo* info = compiler->GetScope()->SearchFunctionPath(target, input, instructions);

			//TODO: Field check in CallProcess should probably use enum, but on the real, I'm too lazy.

			if (!info) {
				report::compile::error("'{}' Not found. Could be either invalid overload or incorrect name. Needs more details.", target.GetTag());
			}

			FunctionBase* func = info->Get();

			if (!func) {
				report::compile::error("No callable for info at '{}' detected.", target.GetTag());
			}

			//Other checks should occur here, such as is static to determine how many arguments will be loaded.

			size_t req_args = func->GetArgCountReq();

			if (args.size() < req_args) {
				report::compile::error("Requires {} arguments for '{}', only {} submitted.", req_args, target.GetTag(), args.size());
			}

			if (func->GetTargetType().policy != nullptr) {
				//Increase the allocation size to include the "this" argument.
				alloc_size++;
			}


			compiler->GetOperationList().emplace_back(InstructType::Call, compiler->GetPrefered(),
				Operand{ func, OperandType::Function },
				Operand{ alloc_size, OperandType::Index });

			compiler->ModArgCount(-static_cast<int64_t>(dealloc_size));



			return Solution{ func->GetReturnType(), OperandType::Register, compiler->GetPrefered() };
		}


		bool HandleCtor(Solution& result, ExpressionCompiler* compiler, SyntaxRecord& target)
		{

			//OverloadInput input;
			//input.object = self;
			//input.paramInput = args;

			//TODO: Should be in error if has an explicit target.

			ITypePolicy* type = compiler->GetScope()->SearchTypePath(target);

			if (type)
			{
				//TODO: Give this a compiler utility function, in case it has a manually defined constructor.
				compiler->GetOperationList().emplace_back(InstructType::Construct, compiler->GetPrefered(),
					Operand{ type, OperandType::Type });

				result = Solution{ QualifiedType{type}, OperandType::Register, compiler->GetPrefered() };
			}


			return type;
		}





		Solution CallProcess(ExpressionCompiler* compiler, SyntaxRecord& target)
		{
			//I seek to merge the functionality of these 2 functions into this one, seeing as they need to tread the same ground.

			Solution result;

			if (HandleCtor(result, compiler, target) == false) {
				//result = HandleCall(compiler, target);
				result = HandleCall_V2(compiler, target);
			}


			return result;
		}




		void VariableProcess(RoutineCompiler* compiler, SyntaxRecord& target)
		{

			//TODO: Readdress VariableProcess. Also might be an expression?



			SyntaxRecord* head_rec = target.FindChild(parse_strings::header);

			if (!head_rec)
				report::compile::error("No record named header.");

			Declaration header{ *head_rec, compiler->GetElement(), Reference::Generic, Reference::Auto };

			//TODO: I can allow this to be static, but it'll be something interesting I'll likely handle later
			// Notably, exclusively if given a space that can facilitate it. IE an error should happen if you make static variables within a formula.
			// Should be a compartment that a function gets that a formula doesn't (mostly because formulas can be temporary, and don't really link).
			if (header.Matches(DeclareMatches::Constness | DeclareMatches::Reference) == false) {
				report::compile::error("Either unexpected qualifiers/specifiers or no type when type expected.");
			}

			LocalInfo* loc = compiler->GetScope()->CreateVariable(target.GetTag(), header);

			size_t loc_index = loc->_index;
			if (SyntaxRecord* definition = target.FindChild(parse_strings::def_expression); definition) {
				auto& def = definition->GetChild(0);
				Solution result = compiler->CompileExpression(def, Register::Result);

				//TODO: Here' try to adapt this to be the reference type loaded if it's generic


				loc->MutateReference(result.reference);//TODO: This needs some point of failure

				//-QUAL_CONV
				//Conversion out;
				//auto convert = result.IsConvertToQualified(header, nullptr, &out);
				//if (convert <= ConvertResult::Failure) {
				//	report::compile::error("Cannot initialize. Error {}", magic_enum::enum_name(convert));
				//}
				//CompUtil::HandleConversion(compiler, out, result, header, convert);
				
				CompUtil::HandleConversion(compiler, result, header, def, ConversionFlag::Initialize);


				
				//compiler->GetOperationList().push_back(CompUtil::Transfer(Operand{ loc_index, OperandType::Index }, result));
				compiler->GetOperationList().append_range(CompUtil::Load(Operand{ loc_index, OperandType::Index }, result, loc->GetQualifiers().IsReference()));
			}


		}




		void StatementProcess(RoutineCompiler* compiler, SyntaxRecord& target)
		{
			Scope _{ compiler, ScopeType::Required };
			compiler->CompileBlock(target);
		}


		Solution ExpressionProcess(ExpressionCompiler* compiler, SyntaxRecord& target)
		{
			return compiler->CompileExpression(target.GetFront(), compiler->GetPrefered());
		}



		void ReturnProcess(RoutineCompiler* compiler, SyntaxRecord& target)
		{
			QualifiedType return_policy = compiler->GetReturnType();

			auto& list = compiler->GetOperationList();

			if (target.size() != 0)
			{
				auto& ret = target.GetChild(0);

				Solution result = compiler->PushExpression(target.GetChild(0), Register::Result, compiler->IsReturnReference());

				//Right here the solutions given type should be evaluated to see if a correct type is being returned.

				//TODO: if the solution isn't in a register that is the result register, push the solution into 
				// the return. Also check type.
				//compiler->GetOperationList().push_back(ret_op);

				//left broken because I just realized even non-abstract types are going to have to worry about
				// inheritence too.



				//Basically, if one doesn't exist, and they aren't both just void.
				//TODO: Actually use void for this, at no point should null be used here. Such would be a statement.
				if (!return_policy) {// && return_policy != result.policy) {
					report::compile::error("Expecting return value but value is found.");
				}

				assert(result.policy);


				//Conversion out;
				//auto convert_result = result.IsConvertToQualified(return_policy, nullptr, &out);
				//if (convert_result <= convertFailure){
				//	report::compile::error("Expression not convertible to return type.");
				//}
				//CompUtil::HandleConversion(compiler, out, result, return_policy, convert_result);

				CompUtil::HandleConversion(compiler, result, return_policy, ret, ConversionFlag::Return);

			}
			else if (return_policy->CheckRuleset(TypeRuleset::NoReturn) == false)
			{
				report::compile::error("Expecting return expression");
			}
			else
			{ 
				CompUtil::PrepareReturn(compiler, return_policy, {});
			}
			

			auto scope = compiler->GetScope();

			scope->FlagReturn();

			//Operation ret_op{ InstructionType::Return };

			//This part should probably say what left and right situation should be on display.
			compiler->GetOperationList().emplace_back(InstructionType::Return);
		}


		Solution CastProcess(ExpressionCompiler* compiler, SyntaxRecord& target)
		{

			auto& rhs = target.FindChild(parse_strings::rhs)->GetFront();

			Declaration header{ target.FindChild(parse_strings::rhs)->GetFront(), compiler->GetElement(), Reference::Temp };

			if (!header) {
				report::compile::error("No type found for cast");
			}

			if (header.Matches(DeclareMatches::Constness) == false) {
				report::compile::error("Unexpected qualifiers detected.");
			}

			auto& lhs = target.FindChild(parse_strings::lhs)->GetFront();

			Solution expression = compiler->PushExpression(target.FindChild(parse_strings::lhs)->GetFront(), compiler->GetPrefered(), false);




			Conversion out;

			assert(expression.policy);
			
			//report::compile::debug("result {:X}", (uint64_t)expression.policy);

			if (1 || target.GetView() != "maybe")
			{
				if (auto convert_result = expression.IsConvertToQualified(header, nullptr, &out, ConversionFlag::Explicit); convert_result > convertFailure)
				{
					//If this can convert, it's basically going to be something automatic.
					CompUtil::HandleConversion(compiler, out, expression, header, convert_result);
				}
				//if the type we're trying to go to can convert into ours with only type conversions
				else if (auto convert_result = header.IsConvertToQualified(expression, nullptr, nullptr, ConversionFlag::Explicit); convert_result > convertFailure)
				{
					expression = Solution{ header, OperandType::Register, compiler->GetPrefered() };

					compiler->GetOperationList().emplace_back(
						InstructionType::Convert,
						compiler->GetPrefered(),
						Operand{ header.policy, OperandType::Type },
						expression);
				}
				else
				{
					report::compile::error("Expression not convertible to cast type.");
				}
			}
			else
			{



				if (auto convert_result = header.IsConvertToQualified(expression, nullptr, nullptr, ConversionFlag::Explicit); convert_result > convertFailure)
				{
					expression = Solution{ header, OperandType::Register, compiler->GetPrefered() };

					compiler->GetOperationList().emplace_back(
						InstructionType::Convert,
						compiler->GetPrefered(),
						Operand{ header.policy, OperandType::Type },
						expression);
				}

				//The 2 classes must both 
			}

			
		

			return expression;
		}





		void not_actually_call__CallProcess(RoutineCompiler* compiler, SyntaxRecord& target)
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




		struct StrConvert final : public ICallableUnit
		{
			using Self = StrConvert;

			static Self& GetSingleton()
			{
				static Self singleton{};

				return singleton;
			}


			inline static Self* instance = &GetSingleton();

			RuntimeVariable Execute(api::vector<RuntimeVariable> args, Runtime*, RuntimeVariable*) override
			{
				return (double)args[0]->AsString().size();
			}
		};



		namespace
		{
			//Unsure if this should be using a reference to the runtime variable. I guess not? This doesn't intend to
// actually alter the register, just the value the register holds.
			using ConvertFunc = RuntimeVariable(*)(RuntimeVariable);


			template<ConvertFunc Func>
			struct Convert final : public ICallableUnit
			{
				using Self = Convert<Func>;

				static Self& GetSingleton()
				{
					static Self singleton{};

					return singleton;
				}


				inline static Self* instance = &GetSingleton();

				virtual RuntimeVariable Execute(api::vector<RuntimeVariable> args, Runtime*, RuntimeVariable*)
				{

					//Convert should be real simple.
					//here, it's supposed to return the conversion.
					return Func(args[0]);
				}


				Convert(ConvertFunc func) {}

			protected:
				constexpr Convert() = default;

				Convert(const Convert&) = delete;
				Convert(Convert&&) = delete;
				Convert& operator=(const Convert&) = delete;
				Convert& operator=(Convert&&) = delete;

			};




			//struct NumberConvert : public ICallableUnit


			std::array<ICallableUnit*, Number::Settings::length> convertMap;

			//uint32_t is 0:1:1:0
			template <NumeralType A, Signage B, Size C, Limit D>
			RuntimeVariable ConvNum(RuntimeVariable var)
			{
				Number number = var->AsNumber();

				number = number.Convert(Number::Settings{ A, C, B, D });

				return number;
			}





			template <NumeralType A, Signage B, Size C, Limit D = (Limit)0>
			inline void FillLimit(int& i)
			{

				convertMap[i++] = Convert<ConvNum<A, B, C, D>>::instance;

				constexpr auto _d = (Limit)(D + 1);

				if constexpr (_d < Limit::Total)
				{
					FillLimit<A, B, C, _d>(i);
				}
			}

			template <NumeralType A, Signage B, Size C = (Size)0>
			inline void FillSize(int& i)
			{
				FillLimit<A, B, C>(i);

				constexpr auto _c = (Size)(C + 1);

				if constexpr (_c < Size::Total)
				{
					FillSize<A, B, _c>(i);
				}
			}

			template <NumeralType A, Signage B = (Signage)0>
			inline void FillSign(int& i)
			{
				FillSize<A, B>(i);

				constexpr auto _b = (Signage)(B + 1);

				if constexpr (_b < Signage::Total)
				{
					FillSign<A, _b>(i);
				}
			}

			template <NumeralType A = (NumeralType)0>
			inline void FillNumeral(int& i)
			{
				FillSign<A>(i);

				constexpr NumeralType _a = (NumeralType)(A + 1);

				if constexpr (_a < NumeralType::Total)
				{
					FillNumeral<_a>(i);
				}
			}


			void FillNumberConvertMap()
			{
				int i = 0;
				FillNumeral(i);
			}


			INITIALIZE()
			{
				FillNumberConvertMap();
			}
		}


		struct StringType : public ConcretePolicy
		{
			using ConcretePolicy::ConcretePolicy;

			ConvertResult IsConvertibleTo(const ITypePolicy* other, const ITypePolicy* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const override
			{
				//For now, this will be very specific. It won't even exist later. But for now, the idea is that this should be able to transfer into a string.
				//Later, I'm going to just make a thing that manages conversions akin to a dispatcher.

				static StrConvert converter{};

				ConvertResult result = __super::IsConvertibleTo(other, scope, out, flags);


				if (out && result <= ConvertResult::Failure)
				{
					auto double_offset = Number::Settings::CreateFromType<double>().GetOffset();
					
					if (IdentityManager::instance->GetTypeByOffset("NUMBER", double_offset) == other)
					{
						out->implDefined = std::addressof(converter);
						result = ConvertResult::ImplDefined;
					}
				}

				return result;
			}
		};


		struct NumberType : public ConcretePolicy
		{
			NumberType(std::string_view name, Number::Settings settings) : ConcretePolicy{ name, settings.GetOffset() }, _settings{settings}
			{}

			//please, make this with a setting attached.


			ConvertResult IsConvertibleTo(const ITypePolicy* other, const ITypePolicy* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const override
			{
				//For now, this will be very specific. It won't even exist later. But for now, the idea is that this should be able to transfer into a string.
				//Later, I'm going to just make a thing that manages conversions akin to a dispatcher.

				ConvertResult result = __super::IsConvertibleTo(other, scope, out, flags);
				
				
				
				
				if (out && result <= ConvertResult::Failure)
				{
					

					auto identity = other->FetchTypeID().GetIdentity();

					auto index = IdentityManager::instance->GetIndexFromName("NUMBER");


					if (identity.index == index )
					{

						//With this, I might as well just be able to convert it and ignore the rest of this shit innit?
						const NumberType* other_num = dynamic_cast<const NumberType*>(other);

						if (other_num && (flags & ConversionFlag::Explicit || !other_num->_settings.IsInteger() || _settings.IsInteger()))
						{
							if (identity.offset >= Number::Settings::length)
								report::fault::critical("Offset greater than number type length.");

							out->implDefined = convertMap[--identity.offset];

							result = ConvertResult::ImplDefined;
						}
					}
				}

				return result;
			}


			const Number::Settings _settings;
		};


		struct CoreType : public ConcretePolicy
		{
			//Core types basically will just not have manual types loaded.

			using ConcretePolicy::ConcretePolicy;

			std::vector<ITypePolicy*> GetPostAffixedTypes() const override
			{
				return {};
			}
		};


		struct exponent {
			//TODO: constexpr the exponent function when you can.
			Number operator()(Number& lhs, Number& rhs) const {
				return lhs.pow(rhs);
			}

		};



		INITIALIZE()
		{
			//I would like something to make this assign a fuck ton easier

			generatorList[SyntaxType::Return] = ReturnProcess;
			generatorList[SyntaxType::StateBlock] = StatementProcess;
			generatorList[SyntaxType::ExpressBlock] = ExpressionProcess;

			generatorList[SyntaxType::Unary] = OperatorProcess;
			generatorList[SyntaxType::Binary] = OperatorProcess;
			generatorList[SyntaxType::Cast] = CastProcess;
			

			generatorList[SyntaxType::Number] = LiteralProcess;
			generatorList[SyntaxType::Integer] = LiteralProcess;
			generatorList[SyntaxType::Boolean] = LiteralProcess;
			generatorList[SyntaxType::String] = LiteralProcess;
			generatorList[SyntaxType::Object] = LiteralProcess;


			generatorList[SyntaxType::Variable] = VariableProcess;
			generatorList[SyntaxType::Field] = FieldProcess;
			generatorList[SyntaxType::Call] = CallProcess;
			generatorList[SyntaxType::Conditional] = ConditionalProcess;



			instructList[InstructType::Call] = InstructWorkShop::Call;
			instructList[InstructType::Convert] = InstructWorkShop::Convert;
			instructList[InstructType::Construct] = InstructWorkShop::Construct;


			//These 2 are one in the same.
			instructList[InstructType::Move] = InstructWorkShop::Transfer;
			instructList[InstructType::Copy] = InstructWorkShop::Transfer;
			instructList[InstructType::Reference] = InstructWorkShop::Transfer;
			instructList[InstructType::Forward] = InstructWorkShop::Transfer;
			instructList[InstructType::ForwardMove] = InstructWorkShop::Transfer;


			instructList[InstructType::Return] = InstructWorkShop::Ret;
			instructList[InstructType::JumpStack] = InstructWorkShop::JumpStack;
			instructList[InstructType::ModArgStack] = InstructWorkShop::ModArgStack;
			instructList[InstructType::ModVarStack] = InstructWorkShop::ModVarStack;
			instructList[InstructType::DefineVariable] = InstructWorkShop::DefineVar;
			instructList[InstructType::DefineParameter] = InstructWorkShop::DefineParam;
			instructList[InstructType::DropStack] = InstructWorkShop::DropStack;
			instructList[InstructType::DropStackN] = InstructWorkShop::DropStack;


			instructList[InstructType::Addition] = InstructWorkShop::BinaryMath<std::plus<>, false>;
			instructList[InstructType::Subtract] = InstructWorkShop::BinaryMath<std::minus<>, false>;
			instructList[InstructType::Multiply] = InstructWorkShop::BinaryMath<std::multiplies<>, false>;
			instructList[InstructType::Division] = InstructWorkShop::BinaryMath<std::divides<>, false>;
			instructList[InstructType::Modulo] = InstructWorkShop::BinaryMath<std::modulus<>, false>;
			instructList[InstructType::Exponent] = InstructWorkShop::BinaryMath<exponent, false>;

			instructList[InstructType::EqualTo] = InstructWorkShop::BinaryCompare<std::equal_to<>, "==">;
			instructList[InstructType::NotEqualTo] = InstructWorkShop::BinaryCompare<std::not_equal_to<>, "!=">;
			instructList[InstructType::LesserThan] = InstructWorkShop::BinaryCompare<std::less<>, "<">;
			instructList[InstructType::LesserOrEqual] = InstructWorkShop::BinaryCompare<std::less_equal<>, "<=">;
			instructList[InstructType::GreaterThan] = InstructWorkShop::BinaryCompare<std::greater<>, ">">;
			instructList[InstructType::GreaterOrEqual] = InstructWorkShop::BinaryCompare<std::greater_equal<>, ">=">;

			instructList[InstructType::Modulo] = InstructWorkShop::BinaryMath<std::modulus<>, false>;
			//We not ready for this one.
			//instructList[InstructType::LogicalNOT] = InstructWorkShop::UnaryMath<std::logical_not<void>>;
			instructList[InstructType::UnaryMinus] = InstructWorkShop::UnaryMath<std::negate<void>>;
			instructList[InstructType::Promote] = InstructWorkShop::CheckPromotion;

			

			

			//IDENTITY MANAGER TEST
			
			//Core contains all fundemental "auto" types. Types that indicate something as being an object, an external object, an interface, a struct,
			// a class, etc etc. largely these are all


			IdentityManager::instance->GenerateID("CORE", 0);


			IdentityManager::instance->GenerateID("NUMBER", Number::Settings::length);
			IdentityManager::instance->GenerateID("STRING", 0);


			constexpr auto offset = Number::Settings::GetOffset(NumeralType::Floating);

			//RGL_LOG(info, "numset lng:{} v off:{}", Number::Settings::length, offset);

			//TODO: This type of instantiation should be reserved squarely for intrinsic types like numbers, strings etc.
			// Other than that, no type should be created knowing what it is already.

			static ConcretePolicy* NUMBER = new ConcretePolicy{ "NUMBER", 0 };

			//I'd like to make a trival ID. The trival id is a singular empty id for a type that cannot be searched for
			// such as a function signature or 

			static ConcretePolicy* string8 = new ConcretePolicy{ "STRING", 0 };


			//For the love of god, automate making these. I beg.
			static ConcretePolicy* float64 = new NumberType{ "NUMBER", Number::Settings::CreateFromType<double>() };
			static ConcretePolicy* float32 = new NumberType{ "NUMBER", Number::Settings::CreateFromType<float>() };
			static ConcretePolicy* uBoolean = new NumberType{ "NUMBER", Number::Settings{NumeralType::Integral, Size::Bit, Signage::Unsigned, Limit::Bound} };
			static ConcretePolicy* sBoolean = new NumberType{ "NUMBER", Number::Settings{NumeralType::Integral, Size::Bit, Signage::Signed, Limit::Bound} };
			static ConcretePolicy* sInt32 = new NumberType{ "NUMBER", Number::Settings::CreateFromType<int32_t>() };
			static ConcretePolicy* uInt32 = new NumberType{ "NUMBER", Number::Settings::CreateFromType<uint32_t>() };
			static ConcretePolicy* sInt64 = new NumberType{ "NUMBER", Number::Settings::CreateFromType<int64_t>() };
			static ConcretePolicy* uInt64 = new NumberType{ "NUMBER", Number::Settings::CreateFromType<uint64_t>() };
			

			static ConcretePolicy* _coreObject = new CoreType{ "CORE", 0 };
			
			_coreObject->SetInheritFrom(common_type::voidable());
			

			
			float64->EmplaceDefault(static_cast<double>(0));
			float32->EmplaceDefault(static_cast<float>(0));
			uBoolean->EmplaceDefault(static_cast<bool>(0));
			
			uInt32->EmplaceDefault(static_cast<uint32_t>(0));
			sInt32->EmplaceDefault(static_cast<int32_t>(0));
			sInt64->EmplaceDefault(static_cast<int64_t>(0));
			uInt64->EmplaceDefault(static_cast<uint64_t>(0));
			
			sBoolean->EmplaceDefault(Number{ Number::Settings{NumeralType::Integral, Size::Bit, Signage::Signed, Limit::Bound} });
		
			float64->SetInheritFrom(NUMBER);
			float32->SetInheritFrom(NUMBER);
			float64->PrintInheritance();

			string8->EmplaceDefault("");
			
			//Read some shit here.
		};

	}
}

//*/

