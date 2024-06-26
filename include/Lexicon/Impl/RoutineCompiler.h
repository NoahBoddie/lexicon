#pragma once

#include "Expression.h"
#include "ExpressionType.h"
#include "Solution.h"
#include "Operation.h"
#include "RoutineBase.h"

//*src
#include "ConcreteFunction.h"
#include "TargetObject.h"
#include "CompileUtility.h"
namespace LEX
{

	using Syntax = Impl::Syntax;
	using SyntaxType = Impl::SyntaxType;


	struct Scope;
	struct FunctionData;
	class RoutineCompiler;
	class RoutineCompiler;
	class ExpressionCompiler;

	class Generator;

	struct TargetObject;

	//TODO: Statement generator uses RoutineCompiler. If there's something statement compiler does not have, please address.
	using StatementGenerator = void(*)(RoutineCompiler*, Record&);
	//This doesn't need an out does it
	using ExpressionGenerator = Solution(*)(ExpressionCompiler*, Record&);//, Solution, Solution);//, Register);

	
	struct Generator : public ConstClassAlias<std::variant<Void, StatementGenerator, ExpressionGenerator>>
	{
		//I don't quite have the word for this yet, so I'm going to leave it. Module might be it, but dunno.
		ALIAS_HEADER;

		Solution _PostPrepFunction(RoutineCompiler* compiler, Record& target)
		{
			//The factory pieces seem like they need something to plug into

			switch (index())
			{
			case 0:
				report::compile::critical("Generator index void dumbass"); break;

				//I'm doing it this way because I'm tired of making source files
			case 1:
				std::get<StatementGenerator>(*this)(reinterpret_cast<RoutineCompiler*>(compiler), target);
				return {};

			case 2:
				return std::get<ExpressionGenerator>(*this)(reinterpret_cast<ExpressionCompiler*>(compiler), target);

			}

			report::compile::critical("Generator index unknown type");
			return {};
		}
	};

	//I would like to rewrite this so that 1 factory can take up multiple expressions, and additionally that there would be a bit flag to control which places have an entry automatically
	// Not entirely sure I'll need but think it over.
	inline std::map<SyntaxType, Generator> generatorList;//Kinda temp.



	struct CompilerBase
	{

		virtual ~CompilerBase() = default;


		//Put all information from this into a Base with routine compiler just being the highest level
		friend class Scope;

		//Important thing to note. This is only for THE CODE. So while everything else is important, all it does is create routine stuff.

		Register GetPrefered()
		{
			return _prefered;
		}

		std::vector<Operation>& GetOperationList()
		{
			return *_current;
		}


		size_t GetArgCount() const
		{
			return argCount[0];
		}

		size_t ModArgCount(int64_t i = 1)
		{
			size_t count = argCount[0];

			if ((argCount[0] += i) > argCount[1])
				argCount[1] = argCount[0];

			if (i)
				GetOperationList().emplace_back(InstructionType::IncrementArgStack, Operand{ i , OperandType::Differ });

			return count;
		}



		QualifiedType GetReturnType()
		{
			//this return type doesn't need to be the specialized one. In fact, for now it's better that it isn't.
			return _targetFunc->GetReturnType();
		}

		Scope* GetScope()
		{
			return currentScope;
		}

		TargetObject* GetTarget()
		{
			return _object;
		}

		void PushTargetObject(TargetObject& obj)
		{
			_object = &obj;
		}

		void PopTargetObject()
		{
			if (_object)
				_object = _object->prev;
		}


		Environment* GetEnvironment()
		{
			return _environment;
		}


	protected:

		//Please merge these 2 functions. make them mod var count, with a boolean to handle it being a param.

		//Mods the count for parameters, not including the increment instructions.
		size_t ModParamCount(int64_t inc, std::vector<ITypePolicy*> policies = {})
		{
			auto size = policies.size();

			if (inc > 0 && size != inc)
			{
				//TODO: make error a Fatal Fault
				report::compile::critical("Size of policies does not equal expected size({} != {})", size, inc);
				throw nullptr;
			}

			//make the below use this.
			size_t count = varCount[0];

			if ((varCount[0] += inc) > varCount[1])
				varCount[1] = varCount[0];


			auto& op_list = GetOperationList();


			if (inc > 0)
			{
				for (auto i = 0; i < size; i++)
				{
					//for each policy, starting at count and increasing by i, each policy needs to be loaded into
					// the respective variable index by instruction, and if the ITypePolicy is generic, then it should
					// have an instruction intend to specialize.

					size_t index = count + i;
					ITypePolicy* policy = policies[i];
					op_list.emplace_back(InstructionType::DefineParameter, Operand{ index , OperandType::Index }, Operand{ policy, OperandType::Type });
				}
			}


			return count;
		}

		
		size_t ModVarCount(int64_t inc, std::vector<ITypePolicy*> policies = {})
		{
			size_t count = varCount[0];

			//If there is no operative list, we're at the very end, and there's no reason to actually decrement.
			if (_current)
			{

				auto size = policies.size();

				if (inc > 0 && size != inc)
				{
					//TODO: make error a Fatal Fault
					report::compile::critical("Size of policies does not equal expected size({} != {})", size, inc);
					throw nullptr;
				}


				if ((varCount[0] += inc) > varCount[1])
					varCount[1] = varCount[0];
				
				auto& op_list = GetOperationList();
				
				op_list.emplace_back(InstructionType::IncrementVarStack, Operand{ inc , OperandType::Differ });
				

				if (inc > 0)
				{
					for (auto i = 0; i < size; i++)
					{
						//for each policy, starting at count and increasing by i, each policy needs to be loaded into
						// the respective variable index by instruction, and if the ITypePolicy is generic, then it should
						// have an instruction intend to specialize.

						size_t index = count + i;
						ITypePolicy* policy = policies[i];
						op_list.emplace_back(InstructionType::DefineVariable, Operand{ index , OperandType::Index }, Operand{ policy, OperandType::Type });
					}
				}
			}


			return count;
		}

		size_t ModVarCount(ITypePolicy* policy)
		{
			return ModVarCount(1, { policy });
		}


		size_t ModParamCount(ITypePolicy* policy)
		{
			return ModParamCount(1, { policy });
		}

		CompilerBase(Record& ast, FunctionData* owner = nullptr, Environment* env = nullptr) :
			funcRecord{ ast },
			_targetFunc{ owner },
			_environment{ env }
		{

		}

		//Don't use this no more. Each must provide their own container
		//std::vector<Operation> _cache;

		std::vector<Operation>* _current = nullptr;


		//First is current, second is largest.
		std::array<size_t, 2> argCount{ 0, 0 };
		std::array<size_t, 2> varCount{ 0, 0 };


		//Remember to have this record it's most arguments possible.


		//CallableUnit is the bare minimum, but it's not an accurate pivot. Whatever this is using, needs to be something that has function data.
		//FunctionData holds the routine data, parameter data, and access to the element. It's not a callable unit, sure, but a dynamic cast can make it
		// so. Not to mention, a lot of the come from function base originate here. By doing this as well, I can make a set up similar to a context chain, 
		// which for function compiling, would be valuable.

		//FunctionData should also exist to be able to answer some questions about how the actual function functions. Stuff like generics and stuff may be better
		// suited there, especially since AbstractFunctions aren't generic, only specialized.

		//<!>So FunctionData is an element, formulas are not elements

		//Fell like it should hold the target element.
		FunctionData* _targetFunc = nullptr;
		Environment* _environment = nullptr;
		//ICallableUnit* routine = nullptr;

		//I need to figure out what exactly this is, I may need more places to hold records, in the event that I'm not compiling a function, but like a parameter or something.
		Record& funcRecord;

		Scope* currentScope{};//Scopes are the thing that should handle how many variables are in use, that sort of schtick I think.
		// as well as the concept of memory being freed.

		//Solution* _target = nullptr;//to be deprecated
		TargetObject* _object = nullptr;

		Register _prefered = Register::Invalid;
	};

	
	struct ExpressionCompiler : public CompilerBase
	{
		using CompilerBase::CompilerBase;
		
		//ExpressionCompiler(Record& ast, FunctionData* owner = nullptr) : CompilerBase{ ast, owner }{}


		//Obscures statement compiling functions so that  statements can never be compiled where Expressions
		// are expected. Differed by simply expecting or not execting a solution.


		virtual Solution CompileExpression(Record& node, Register pref, std::vector<Operation>& out) = 0;

		Solution CompileExpression(Record& node, Register pref)
		{
			//uses the current loaded op vector. if non-exists error
			return CompileExpression(node, pref, *_current);
		}

		Solution CompileExpression(Record& node, Register pref, Solution tar, TargetObject::Flag flags = TargetObject::Explicit)
		{
			//Consider making this self managing like how scope does.

			TargetObject target{ tar, GetTarget(), flags };

			PushTargetObject(target);

			Solution result = CompileExpression(node, pref, *_current);

			PopTargetObject();

			return result;
		}



		Solution PushExpression(Record& node, Register pref, std::vector<Operation>& out)
		{
			//A convinience function that checks if a solution is in a register and if not, will use move to place it into
			// one.
			Solution result = CompileExpression(node, pref, out);


			if (result.type != OperandType::Register) {
				//TODO: this should use Mutate

				//GetOperationList().emplace_back(InstructType::Forward, Operand{ pref, OperandType::Register }, result);
				GetOperationList().emplace_back(CompUtil::Transfer(Operand{ pref, OperandType::Register }, result));
				
				//This uses the policy of the solution, but uses the register that the above uses.
				return Solution{ result.policy, OperandType::Register, pref };
			}

			return result;
		}

		Solution PushExpression(Record& node, Register pref)
		{
			return PushExpression(node, pref, *_current);
		}

		Solution PushExpression(Record& node, Register pref, Solution tar, bool is_expl = true)
		{

			TargetObject target{ tar, GetTarget(), is_expl };

			PushTargetObject(target);

			Solution result = PushExpression(node, pref, *_current);

			PopTargetObject();

			return result;
		}



	};


	//The above don't use the actual function that compiles syntax. Please note.

	struct RoutineCompiler : public ExpressionCompiler
	{
		using ExpressionCompiler::ExpressionCompiler;

		//Put all information from this into a Base with routine compiler just being the highest level
		friend class Scope;


		//There's one thing I need last. One where it explicitly tries to compile as a statement, and another where it tries to compile as an
		// statement. Compile as statement can basically be CompileLine. But CompileExpression on the other hand can't be what process wrap is, because process statement uses that
		// Hence, there's a problem in there being no pivot term.

	

		Solution _InteralProcess(Generator& factory, Record& node, std::vector<Operation>& out)
		{


			//this function returns the Expectation instead, and has an out for the vector. As such is the convention for such.

			Solution result;

			auto old = _current;
			_current = &out;


			//Func records use is temporary
			result = factory._PostPrepFunction(this, node);


			_current = old;
			
			return result;
		}



		//TODO: I would like try versions of these, mainly for an if statement that could take an statement
		// in its first part, then expect an expression after. Maybe. Idk

		Solution CompileExpression(Record& node, Register pref, std::vector<Operation>& out) override
		{
			//This and process line are basically the same function, maybe make 1 function to rule them both?

			Solution result;

			Register prev = _prefered;

			_prefered = pref;


			auto it = generatorList.find(node.SYNTAX().type);

			if (generatorList.end() == it)
			{
				report::compile::critical("SyntaxType unaccounted for whatever whatever. Type: {}", ExpressionToString(node.SYNTAX().type));
			}

			if (false)//Confirm that the factory is actually made for expressions
			{
				report::compile::critical("Syntax is not a expression");
			}

			logger::debug("RoutineCompiler::CompileExpression: Processing {} . . .", ExpressionToString(node.SYNTAX().type));


			//result from expressions are discarded
			result = _InteralProcess(it->second, node, out);

			_prefered = prev;

			return result;
		}
		using ExpressionCompiler::CompileExpression;
		
		/*
		Solution CompileExpression(Record& node, Register pref)
		{
			//uses the current loaded op vector. if non-exists error
			return CompileExpression(node, pref, *_current);
		}

		Solution CompileExpression(Record& node, Register pref, Solution tar)
		{
			TargetObject target{ tar, GetTarget_(), true };
			
			PushTargetObject(target);
			
			Solution result = CompileExpression(node, pref, *_current);

			PopTargetObject();

			return result;
		}
		//*/


		void CompileStatement(Record& node, Register pref, std::vector<Operation>& out)
		{
			//This and process line are basically the same function, maybe make 1 function to rule them both?

			Solution result;

			Register prev = _prefered;

			_prefered = pref;

			auto it = generatorList.find(node.SYNTAX().type);

			if (generatorList.end() == it)
			{

				report::compile::critical("SyntaxType unaccounted for whatever whatever");
			}

			if (false)//Confirm that the factory is actually made for expressions
			{
				report::compile::critical("Syntax is not a statement");
			}

			logger::debug("RoutineCompiler::CompileStatement: Processing {} . . .", ExpressionToString(node.SYNTAX().type));

			//result from expressions are discarded
			_InteralProcess(it->second, node, out);

			_prefered = prev;
		}

		void CompileStatement(Record& node, Register pref)
		{
			//uses the current loaded op vector. if non-exists error
			return CompileStatement(node, pref, *_current);
		}

		void CompileStatement(Record& node, Register pref, Solution tar)
		{
			TargetObject target{ tar, GetTarget(), true };

			PushTargetObject(target);

			CompileStatement(node, pref, *_current);

			PopTargetObject();
		}



		//This is probably going to be private. Rule is you set a new prefered with it, then do your business.
		std::vector<Operation> CompileLine(Record& node, Register pref)
		{
			//NEW NAME: ProcessNode

			//Process line doesn't expect an out, and even if it's an expression it's result will be tossed.

			std::vector<Operation> result;

			Register prev = _prefered;

			_prefered = pref;

			//Do stuff
			// The first thing it would need to do is find who processes this stuff.
			// I think for the vast majority of simple stuff like variable declarations I can do it, but for stuff like operators I'd like to 
			// resort to the old system.

			auto it = generatorList.find(node.SYNTAX().type);

			if (generatorList.end() == it)
			{
				report::compile::critical("SyntaxType unaccounted for whatever whatever {}", (uint8_t)node.SYNTAX().type);
			}

			//result from expressions are discarded
			_InteralProcess(it->second, node, result);

			_prefered = prev;

			return result;
		}
		//I will begin passing the result vector through this as well.

		std::vector<Operation> CompileBlock(Record& data);
		std::vector<Operation> CompileHeader();


		RoutineBase CompileRoutine();


		//RoutineCompiler(Record& ast, FunctionData* owner = nullptr) : ExpressionCompiler{ast, owner }{}

		//function data is no longer an ask, it's a requirement now.
		// additional. FunctionData can hold its own record.
		static RoutineBase Compile(Record& ast, FunctionData* owner = nullptr, Environment* env = nullptr)
		{
			RoutineCompiler compiler{ ast, owner, env };
			RoutineBase result = compiler.CompileRoutine();
			RGL_LOG(debug, "Compilation complete.");
			return result;
		}

	};
}