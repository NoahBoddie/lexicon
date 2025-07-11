#pragma once

#include "Expression.h"
#include "ExpressionType.h"
#include "Solution.h"
#include "Instruction.h"
#include "RoutineBase.h"

#include "parse_strings.h"

#include "Lexicon/ITemplatePart.h"

//*src
#include "ConcreteFunction.h"
#include "TargetObject.h"
#include "CompileUtility.h"
#include "Lexicon/Engine/GenericBase.h"
#include "Lexicon/Engine/ParseUtility.h"
namespace LEX
{

	struct Scope;
	struct FunctionData;
	class RoutineCompiler;
	class RoutineCompiler;
	class ExpressionCompiler;

	class Generator;

	struct TargetObject;

	//TODO: Statement generator uses RoutineCompiler. If there's something statement compiler does not have, please address.
	using StatementProcessor = void(*)(RoutineCompiler*, SyntaxRecord&);
	using ExpressionProcessor = Solution(*)(ExpressionCompiler*, SyntaxRecord&);//, Solution, Solution);//, Register);
	
	
	struct Generator : public ConstClassAlias<std::variant<std::monostate, StatementProcessor, ExpressionProcessor>>
	{
		//I don't quite have the word for this yet, so I'm going to leave it. Module might be it, but dunno.
		ALIAS_HEADER;

		Solution GenerateSolution(RoutineCompiler* compiler, SyntaxRecord& target);
	};

	//I would like to rewrite this so that 1 factory can take up multiple expressions, and additionally that there would be a bit flag to control which places have an entry automatically
	// Not entirely sure I'll need but think it over.
	inline std::map<SyntaxType, Generator> generatorList;//Kinda temp.



	struct CompilerBase : public ITemplatePart
	{

		using InstructRecord = std::pair<SyntaxRecord*, uint32_t>;



	protected:
		struct TempListHandle
		{
			std::vector<Instruction>*& current;
			std::vector<Instruction>* prev = nullptr;


			TempListHandle(std::vector<Instruction>& out, std::vector<Instruction>*& target) :current{ target }
			{
				prev = current;
				current = std::addressof(out);
			}

			~TempListHandle()
			{
				current = prev;
			}
		};

	public:
		//Put all information from this into a Base with routine compiler just being the highest level
		friend class Scope;

		virtual ~CompilerBase() = default;


		size_t GetSize() const override
		{
			return _generic ? _generic->GetSize() : 0;
		}

		virtual ITypeInfo* GetPartArgument(size_t i) const 
		{
			return _generic ? _generic->GetPartArgument(i) : 0;
		}



		
		//Important thing to note. This is only for THE CODE. So while everything else is important, all it does is create routine stuff.

		Register GetPrefered()
		{
			return _prefered;
		}

	

		std::vector<Instruction>* GetInstructionListPtr();
	
		std::vector<Instruction>& GetInstructionList();
	private:

		
		void PushInstruction(uint32_t index, Instruction instruct)
		{
			instruct.index = index;
			GetInstructionList().push_back(instruct);
		}

		void InsertInstruction(uint32_t index, Instruction instruct)
		{
			instruct.index = index;
			auto& list = GetInstructionList();
			list.insert(list.begin(), instruct);
		}
		

		void AppendInstructions(uint32_t index, InstructList&& list)
		{
			if (index != -1) {
				for (auto& entry : list) {
					if (entry.index == -1) {
						entry.index = index;
					}
				}
			}

			GetInstructionList().append_range(std::forward<InstructList>(list));
		}
		
		void AppendInstructions(uint32_t index, const InstructList& list)
		{
			return AppendInstructions(index, static_cast<InstructList>(list));
		}


		template <typename... Args> requires (requires(uint32_t index, Args&&... args) { Instruction{ std::forward<Args>(args)..., index }; })
		Instruction& EmplaceInstruction(uint32_t index, Args&&... args) 
		{
			return GetInstructionList().emplace_back(std::forward<Args>(args)..., index);
		}
		
		//I think these versions will by my primary way of using them. Almost all instructions will have something associated with them.
		// Making a simple way to state it's actually nothing might be helpful.  something kinda like nullopt, maybe literally that.
	public:

		void PushInstruction(Instruction instruct)
		{
			return PushInstruction(UseRecord(), instruct);
		}
		void InsertInstruction(Instruction instruct)
		{
			return InsertInstruction(UseRecord(), instruct);
		}

		void AppendInstructions(const InstructList& list)
		{
			return AppendInstructions(UseRecord(), static_cast<InstructList>(list));
		}

		void AppendInstructions(InstructList&& list)
		{
			return AppendInstructions(UseRecord(), std::forward<InstructList>(list));
		}

		template <typename... Args>
		Instruction& EmplaceInstruction(Args&&... args) requires (requires(Args&&... args) { Instruction{ std::forward<Args>(args)... }; })
		{
			return EmplaceInstruction(UseRecord(), std::forward<Args>(args)...);
		}


		void PushInstruction(SyntaxRecord& record, Instruction instruct)
		{
			auto temp = ReadyRecord(record);
			return PushInstruction(instruct);
		}



		void InsertInstruction(SyntaxRecord& record, Instruction instruct)
		{
			auto temp = ReadyRecord(record);
			return InsertInstruction(instruct);
		}

		void AppendInstructions(SyntaxRecord& record, const InstructList& list)
		{
			auto temp = ReadyRecord(record);
			AppendInstructions(list);
		}

		void AppendInstructions(SyntaxRecord& record, InstructList&& list)
		{
			auto temp = ReadyRecord(record);
			AppendInstructions(std::forward<InstructList>(list));
		}

		template <typename... Args>
		Instruction& EmplaceInstruction(SyntaxRecord& record, Args&&... args) requires (requires(Args&&... args) { Instruction{ std::forward<Args>(args)... }; })
		{
			auto temp = ReadyRecord(record);
			return EmplaceInstruction(std::forward<Args>(args)...);
		}

		/*

		void PushInstruction(std::nullopt_t, Instruction instruct)
		{
			auto temp = ReadyNoRecord();
			PushInstruction(instruct);
		}

		void AppendInstructions(std::nullopt_t, const InstructList& list)
		{
			auto temp = ReadyNoRecord();
			AppendInstructions(list);
		}

		void AppendInstructions(std::nullopt_t, InstructList&& list)
		{
			auto temp = ReadyNoRecord();
			AppendInstructions(std::forward<InstructList>(list));
		}

		template <typename... Args>
		Instruction& EmplaceInstruction(std::nullopt_t, Args&&... args) requires (requires(Args&&... args) { Instruction{ std::forward<Args>(args)... }; })
		{
			return EmplaceInstruction(std::forward<Args>(args)...);
		}

		//*/


		size_t GetArgCount() const
		{
			return argCount[0];
		}

		void ShiftArgCount(int64_t i)
		{
			//I'm thinking of undoing the bit once I get out of the parameter.
			if ((argCount[0] += i) > argCount[1])
				argCount[1] = argCount[0];
		}

		size_t ModArgCount(int64_t x, int64_t y, bool append = true)
		{
			//Try to unionize this when you can.
			size_t count = argCount[0];

			//i >= 0 || 
			if (!delayDec[0])
			{
				ShiftArgCount(x + y);
			
			}
			else if(x > 0) {// if (i < 0) {
				delayDec[1] += x;
			}

			if (x && append) {
				EmplaceInstruction(InstructionType::ModArgStack, Operand{ x , OperandType::Differ });
			}

			return count;
		}


		void DelayArgDecrement()
		{
			delayDec[0]++;
		}

		[[nodiscard]] int64_t ResumeArgDecrement()
		{
			//dec 1 isn't being incremented

			//The concept of important
			int64_t result = 0;
			if (--delayDec[0] == 0){
				//ShiftArgCount(delayDec[1]);
				//delayDec[1] = 0;
				std::swap(result, delayDec[1]);
			}

			assert(delayDec[0] >= 0);

			return result;
		}
		


		QualifiedType GetReturnType() const
		{
			//this return type doesn't need to be the specialized one. In fact, for now it's better that it isn't.
			return _callData->GetReturnType();
		}

		std::optional<bool> IsReturnReference() const
		{
			return GetReturnType().IsReference();
		}

		
		Scope* GetScope()
		{
			//TODO: GetScope should be const if it's in an expression compiler. Reason being it can only query things, not add stuff like variables.
			return currentScope;
		}



		TargetObject* GetTarget()
		{
			return _object;
		}


		Element* GetElement()
		{
			return _element;
		}


	protected:


		size_t ModVarCount(int64_t inc);


		size_t InitVariables(const std::vector<ITypeInfo*>& types, bool param)
		{
			//TODO: This function doesn't properly handle the inability to use it
			auto size = types.size();

			//make the below use this.
			size_t count = ModVarCount(static_cast<int64_t>(size));


			//auto& op_list = GetInstructionList();

			auto instruct = param ? InstructType::DefineParameter : InstructType::DefineVariable;

			for (auto i = 0; i < size; i++)
			{
				//for each policy, starting at count and increasing by i, each policy needs to be loaded into
				// the respective variable index by instruction, and if the ITypeInfo is generic, then it should
				// have an instruction intend to specialize.

				size_t index = count + i;
				ITypeInfo* policy = types[i];
				
				EmplaceInstruction(instruct, Operand{ index , OperandType::Index }, Operand{ policy, OperandType::Type });
				//op_list.emplace_back(instruct, Operand{ index , OperandType::Index }, Operand{ policy, OperandType::Type });
			}

			return count;
		}

		size_t InitLocals(std::vector<ITypeInfo*> types){return InitVariables(types, false);}

		size_t InitParams(std::vector<ITypeInfo*> types){return InitVariables(types, true);}

		size_t InitLocal(ITypeInfo* type){return InitVariables({ type }, false);}

		size_t InitParam(ITypeInfo* type){return InitVariables({ type }, true);}


		bool IsDetached() const
		{
			//Here's the concept, this remains null if the scope is what handles the object. Basically, it's detached if this is true.
			// If the code is detached, it will send a fault warning when trying to add a variable or attach a scope. 
			// Namely, these 2 activities should expect none of the previous code loaded to be attached
			return _current != nullptr;
		}

		//I'll try to set this up a bit later.
		//friend std::vector<Instruction>& Scope::GetInstructionList();


		CompilerBase(SyntaxRecord& ast, FunctionData* owner, Element* elem, GenericBase* base) : CompilerBase { ast, owner, elem, base, owner->_name }
		{

		}

		CompilerBase(SyntaxRecord& ast, BasicCallableData* owner, Element* elem, GenericBase* base, std::string_view name = "<no name>") :
			funcRecord{ ast },
			_name{ name },
			_callData{ owner },
			_generic{ base },
			_element{ elem }
		{

		}




		//Don't use this no more. Each must provide their own container
		//std::vector<Instruction> _cache;



		//First is current, second is largest.

		std::array<int64_t, 2> delayDec{ 0, 0 };//Decrements delayed due to functions. Want something that handles this better
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
		//FunctionData* _targetFunc = nullptr;

		//Replacement for targetFunc
		BasicCallableData* _callData = nullptr;
		GenericBase* _generic = nullptr;
		std::string_view _name;

		size_t GetParamAllocSize() const
		{
			return  _callData ? _callData->GetParamCount() : 0;
		}


		QualifiedType GetTargetType() const
		{
			return _callData ? _callData->GetTargetType() : nullptr;
		}

		std::string_view name()
		{
			return _name;
		}


		ParameterInfo* FindParameter(std::string a_name)
		{
			return _callData ? _callData->FindParameter(a_name) : nullptr;
		}


	public:



		uint32_t GetRecordIndex(SyntaxRecord* record)
		{
			if (!record)
			{

				auto begin = _instructRecords.begin();
				auto end = _instructRecords.end();
				auto it = std::find(begin, end, record);


				if (it != end) {
					return std::distance(begin, it);
				}
			}
			return -1;
		}



		[[nodiscard]] origin_value<InstructRecord> ReadyRecord(SyntaxRecord* record)
		{
			size_t size = _instructRecords.size();

			origin_value<InstructRecord> result{ _currentRecord, [=](origin_value<InstructRecord>& origin)
				{
					origin.OriginDefault();

					if (_currentRecord.first)
					{
						auto& slot = _currentRecord.second;

						if (slot == -1 && _instructRecords.size() != size)
						{
							if (auto index = GetRecordIndex(record); index != -1) {
								slot = index;
							}
						}
					}
				}
			};

			_currentRecord = std::make_pair(record, GetRecordIndex(record));
			
			return result;
		}
		[[nodiscard]] origin_value<InstructRecord> ReadyRecord(SyntaxRecord& record)
		{
			return ReadyRecord(&ParseUtility::PeekFirstRecord(record));
		}


		[[nodiscard]] origin_value<InstructRecord> ReadyNoRecord()
		{
			return ReadyRecord(nullptr);
		}

		uint32_t AddRecord(SyntaxRecord* record)
		{
			auto value = _instructRecords.size();

			if (value >= 0x00FFFFFF) {
				record->error<IssueType::Fault>("can't be biger than max index, better log later");
			}

			_instructRecords.push_back(record);
			return static_cast<uint32_t>(value);
		}

		uint32_t UseRecord()
		{
			auto record = _currentRecord.first;

			if (!record)
				return -1;

			auto& result = _currentRecord.second;


			if (_currentRecord.second == -1) {
				result = AddRecord(record);
			}

			return result;
		}
		uint32_t UseRecord(SyntaxRecord& record)
		{
			auto result = GetRecordIndex(&record);

			if (result == -1) {
				result = AddRecord(&record);
			}

			return result;
		}
		///////////////




		/*

		[[nodiscard]] origin_value<uint32_t> SetNoRecord()
		{
			origin_value result{ _currRecordIndex };

			_currRecordIndex = -1;

			return result;
		}


		uint32_t GetRecordIndex(SyntaxRecord& record)
		{

			auto begin = _instructRecords.begin();
			auto end = _instructRecords.end();
			auto it = std::find(begin, end, &record);


			if (it == end) {
				return -1;
			}
			else {
				return std::distance(begin, it);
			}
		}

		[[nodiscard]] origin_value<SyntaxRecord*> ReadyRecord(SyntaxRecord& record)
		{
			size_t size = _instructRecords.size();

			{
				origin_value<SyntaxRecord*> result{ _tenativeRecord, [=](origin_value<SyntaxRecord*>& origin)
					{
						if (_instructRecords.size() != size)
						{
							if (auto index = GetRecordIndex(); index == -1) {
								_currRecordIndex = index;
								return;
							}
							
						}

						origin.OriginDefault();
					} 
				};

				auto begin = _instructRecords.begin();
				auto end = _instructRecords.end();
				auto it = std::find(begin, end, &record);


				if (GetRecordIndex() == -1) {
					_tenativeRecord = &record;

				}

				return result;
			}

			origin_value result{ _currRecordIndex };

			_currRecordIndex = static_cast<uint32_t>(new_value);

			return result;
		}

		[[nodiscard]] origin_value<uint32_t> AddRecord(SyntaxRecord& record)
		{
			size_t new_value;

			{
				auto begin = _instructRecords.begin();
				auto end = _instructRecords.end();
				auto it = std::find(begin, end, &record);


				if (it == end) {
					new_value = _instructRecords.size();

					if (new_value >= 0x00FFFFFF) {
						record.error<IssueType::Fault>("can't be biger than max index, better log later");
					}

					_instructRecords.push_back(&record);
				}
				else {
					new_value = std::distance(begin, it);
				}
			}

			origin_value result{ _currRecordIndex };

			_currRecordIndex = static_cast<uint32_t>(new_value);

			return result;
		}


		uint32_t UseRecord(SyntaxRecord& record)
		{
			auto temp = record;
			return _currRecordIndex;

		}
		//*/
	protected:




		//SyntaxRecord* _tenativeRecord = nullptr;
		//uint32_t _currRecordIndex = -1;

		InstructRecord _currentRecord{ nullptr, -1 };

		std::vector<SyntaxRecord*> _instructRecords;

		//I would like to remove environment from play, and replace it with just the element being given.
		Element* _element = nullptr;
		//ICallableUnit* routine = nullptr;

		//I need to figure out what exactly this is, I may need more places to hold records, in the event that I'm not compiling a function, but like a parameter or something.
		SyntaxRecord& funcRecord;
		
		//Here's the concept, this remains null if the scope is what handles the object. Basically, it's detached if this is true.
		std::vector<Instruction>* _current = nullptr;

		Scope* currentScope{};//Scopes are the thing that should handle how many variables are in use, that sort of schtick I think.
		// as well as the concept of memory being freed.

		//Solution* _target = nullptr;//to be deprecated
		TargetObject* _object = nullptr;

		bool implicitReturn = false;
		
		Register _prefered = Register::Invalid;
	};

	
	struct ExpressionCompiler : public CompilerBase
	{
		using CompilerBase::CompilerBase;
		
		//ExpressionCompiler(SyntaxRecord& ast, FunctionData* owner = nullptr) : CompilerBase{ ast, owner }{}


		//Obscures statement compiling functions so that  statements can never be compiled where Expressions
		// are expected. Differed by simply expecting or not execting a solution.

		//TODO: Unvirtual CompileExpression, just private some of the important functions.
		virtual Solution CompileExpression(SyntaxRecord& node, Register pref) = 0;


		Solution CompileExpression(SyntaxRecord& node, Register pref, std::vector<Instruction>& out)
		{
			TempListHandle handle{ out, _current };

			auto result = CompileExpression(node, pref);
			return result;

		}


		Solution CompileExpression(SyntaxRecord& node, Register pref, Solution tar, TargetObject::Flag flags = TargetObject::None)
		{
			//Consider making this self managing like how scope does.

			TargetObject target{ &tar, _object, flags };

			Solution result = CompileExpression(node, pref);

			return result;
		}



		Solution PushExpression(SyntaxRecord& node, Register pref, std::optional<bool> is_ref)//is_ref is false by default I guess??
		{
			//A convinience function that checks if a solution is in a register and if not, will use move to place it into
			// one.
			Solution result = CompileExpression(node, pref);


			if (result.type != OperandType::Register) {
				//TODO: this should use Mutate, Scratch, it will want to use Load, maybe a combo of the 2

				
				//GetInstructionList().emplace_back(CompUtil::Transfer(Operand{ pref, OperandType::Register }, result));
				AppendInstructions(node, CompUtil::Load(Operand{ pref, OperandType::Register }, result, is_ref));
				
				//This uses the policy of the solution, but uses the register that the above uses.
				return Solution{ result, OperandType::Register, pref };
			}

			return result;
		}


		Solution PushExpression(SyntaxRecord& node, Register pref, Solution tar, TargetObject::Flag flags = TargetObject::None)
		{

			TargetObject target{ &tar, _object, flags };

			Solution result = PushExpression(node, pref, false);

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

	




		//TODO: I would like try versions of these, mainly for an if statement that could take an statement
		// in its first part, then expect an expression after. Maybe. Idk

		Solution CompileExpression(SyntaxRecord& node, Register pref) override
		{
			//This and process line are basically the same function, maybe make 1 function to rule them both?

			Solution result;

			Register prev = _prefered;

			_prefered = pref;


			auto it = generatorList.find(node.SYNTAX().type);

			if (generatorList.end() == it)
			{
				report::compile::critical("SyntaxType unaccounted for whatever whatever. Type: {}", magic_enum::enum_name(node.SYNTAX().type));
			}

			if (false)//Confirm that the factory is actually made for expressions
			{
				report::compile::critical("Syntax is not a expression");
			}

			report::compile::trace("RoutineCompiler::CompileExpression: Processing {} . . .", magic_enum::enum_name(node.SYNTAX().type));


			//result from expressions are discarded
			//result = _InteralProcess(it->second, node);
			
			result = it->second.GenerateSolution(this, node);
			
			_prefered = prev;

			return result;
		}
		using ExpressionCompiler::CompileExpression;
		
		/*
		Solution CompileExpression(SyntaxRecord& node, Register pref)
		{
			//uses the current loaded op vector. if non-exists error
			return CompileExpression(node, pref, *_current);
		}

		Solution CompileExpression(SyntaxRecord& node, Register pref, Solution tar)
		{
			TargetObject target{ tar, GetTarget_(), true };
			
			PushTargetObject(target);
			
			Solution result = CompileExpression(node, pref, *_current);

			PopTargetObject();

			return result;
		}
		//*/


		void CompileStatement(SyntaxRecord& node, Register pref)
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

			logger::debug("RoutineCompiler::CompileStatement: Processing {} . . .", magic_enum::enum_name(node.SYNTAX().type));

			//result from expressions are discarded
			//_InteralProcess(it->second, node);

			it->second.GenerateSolution(this, node);

			_prefered = prev;
		}

		void CompileStatement(SyntaxRecord& node, Register pref, Solution tar)
		{
			TargetObject target{ &tar, _object };

			CompileStatement(node, pref);
		}



		//This is probably going to be private. Rule is you set a new prefered with it, then do your business.
		Solution CompileLine(SyntaxRecord& node, Register pref)
		{
			//Process line doesn't expect an out, and even if it's an expression it's result will be tossed.

			
			Register prev = _prefered;

			_prefered = pref;

			//Do stuff
			// The first thing it would need to do is find who processes this stuff.
			// I think for the vast majority of simple stuff like variable declarations I can do it, but for stuff like operators I'd like to 
			// resort to the old system.

			auto it = generatorList.find(node.SYNTAX().type);

			if (generatorList.end() == it)
			{
				report::fault::critical("SyntaxType '{}' unaccounted for", magic_enum::enum_name(node.SYNTAX().type));
			}

			//result from expressions are discarded
			//out = _InteralProcess(it->second, node, result);
			Solution result = it->second.GenerateSolution(this, node);

			_prefered = prev;

			return result;
		}

		//I will begin passing the result vector through this as well.

		void CompileBlock(SyntaxRecord& data);
		void CompileBlock(SyntaxRecord& data, ScopeType type);


		bool CompileRoutine(RoutineBase& routine);


		//RoutineCompiler(SyntaxRecord& ast, FunctionData* owner = nullptr) : ExpressionCompiler{ast, owner }{}

		//function data is no longer an ask, it's a requirement now.
		// additional. FunctionData can hold its own record.
		
		
		static bool Compile(RoutineBase& routine, SyntaxRecord& ast, FunctionBase* owner)
		{
			return Compile(routine, ast, owner, owner, owner->AsGenericElement(), owner->GetName());
		}
		
		static bool Compile(RoutineBase& routine, SyntaxRecord& ast, FunctionData* owner, Element* elem)
		{
			return Compile(routine, ast, owner, elem, nullptr, owner->name());
		}
		
		static bool Compile(RoutineBase& routine, SyntaxRecord& ast, BasicCallableData* owner, Element* elem, GenericBase* gen, std::string_view name = parse_strings::no_name)
		{
			report _{ IssueType::Compile };
			RoutineCompiler compiler{ ast, owner, elem, gen, name };
			bool result = compiler.CompileRoutine(routine);
			
			if (!result) {
				
				report::compile::failure("Compilation of '{}' failure. Address logs", name), (IsDebuggerPresent() ? __debugbreak() : void());
			}
			else {
				report::compile::debug("Compilation of '{}' complete.", name);
			}

			return result;
		}
		private:
			bool _success = true;
	};
}