#pragma once

#include "Lexicon/Variable.h"
#include "Routine.h"
#include "ParameterInfo.h"
//Should have parameterinfos?

#include "Lexicon/Engine/QualifiedType.h"

#include "Lexicon/Engine/OverloadFlag.h"

//*src
#include "Lexicon/Engine/parse_strings.h"
#include "Lexicon/Engine/ThisInfo.h"
namespace LEX
{
	class ParameterInfo;
	
	struct ProcedureData;

	struct Overload;
	struct OverloadEntry;

	//Definition is the name of the struct that holds either routine data, or caller data.
	//Or maybe it should be an enum.
	using Procedure = void(*)(RuntimeVariable&, Variable*, std::span<Variable*>, ProcedureData&);
	

	enum struct FunctionBody
	{
		None,
		Routine,
		Procedure,
	};
	
#ifdef OLD_FUNCTION_DATA

	struct BasicCallableData
	{
		QualifiedType _returnType = nullptr;


		//The actual object for this has severely changed. It's "ParamInfo" I think? But this isn't terribly important
		// here yet so you know.
		//std::vector<DeclaredParam> reqParams;
		//std::vector<DefinedParam> defParams;

		//TODO: I would like to represent "this" as a unique pointer to a ParamInfo
		std::unique_ptr<ParameterInfo> _thisInfo;

	//protected:
		std::vector<ParameterInfo> parameters;
	public:

		Routine _routine;  //actually needs to be a pointer
		uint32_t vardIndex = (uint32_t)-1;
		uint32_t defaultIndex = (uint32_t)-1;

		void VisitParameters(std::function<void(ParameterInfo&)> func)
		{
			if (_thisInfo)
				func(*_thisInfo);

			for (auto& param : parameters)
			{
				func(param);
			}
		}
		

		
		Routine* GetRoutine()
		{
			//This plans to be a pointer later, as this will end up just being
			return &_routine;
		}


	public:


		//const std::vector<ParameterInfo>& GetParameters() const
		//{
			//return parameters;
			//return { _ParamBegin(), _ParamEnd() };
		//}


		uint32_t GetParamCount() const
		{
			return GetArgCount() + HasTarget();
		}

		uint32_t GetArgCount() const
		{
			return parameters.size();
		}


		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


		bool HasVadiadic() const
		{
			return vardIndex != -1;
		}

		ParameterInfo* GetVariadicParameter()
		{
			if (vardIndex != -1) {
				return &parameters[vardIndex];
			}

			return nullptr;
		}

	public:


		uint32_t GetArgCountReq() const
		{
			if (vardIndex != -1)
				return vardIndex;

			return GetArgCount();
		}
		uint32_t GetArgCountMax() const
		{
			if (vardIndex != -1)
				return -1;

			return GetArgCount();
		}


		std::array<uint32_t, 2> GetArgRange() const
		{
			return { GetArgCountReq(), GetArgCountMax() };
		}

		uint32_t GetParamCountReq() const
		{
			return GetArgCountReq() + HasTarget();
		}

		uint32_t GetParamCountMax() const
		{
			auto res = GetArgCountMax();
			return std::max(res + HasTarget(), res);
		}

		std::array<uint32_t, 2> GetParamRange() const
		{
			return { GetParamCountReq(), GetParamCountMax() };
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



		//Needs to be moved into IFunction/Function.
		QualifiedType GetReturnType() const
		{
			return _returnType;
		}

		QualifiedType GetReturnType(ITemplatePart* part) const
		{
			return _returnType.QualifySpecial(part);
		}


		QualifiedType GetTargetType() const
		{
			return _thisInfo ? _thisInfo->GetQualifiedType() : nullptr;;
		}

		bool HasTarget() const
		{
			return !!_thisInfo;
		}

		//These sorts of things should be protected, not used up front.
		//std::string name(){return _name;}

		ParameterInfo* FindParameter(const std::string_view& a_name)
		{
			if (a_name == parse_strings::this_word) {
				return _thisInfo.get();
			}

			auto end = parameters.end();
			auto it = std::find_if(parameters.begin(), end, [&](ParameterInfo& q) { return q.GetName() == a_name; });
			if (it != end) {
				return &*it;
			}
			return nullptr;
		}



		//-1 is for this
		ParameterInfo* FindParameterByPos(size_t index)
		{
			if (index == -1) {
				return _thisInfo.get();
			}

			return std::addressof(parameters.at(index));
		}



	protected:

		RuntimeVariable BasicExecute(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def, 
			std::optional<Procedure> prod = std::nullopt);

	
		RuntimeVariable BasicInvoke(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, RuntimeVariable* def,
			std::optional<Procedure> prod = std::nullopt);

	};

	struct FunctionData : public BasicCallableData
	{
		//A struct to be owned protected.

		//weren't functions environments? A: They don't need to be anymore.
		//Data



		//These are FunctionData: Functions, GenericFunctions, and Formulas.
		// Yes, formulas are function data. I thought to make CallableUnit handle the setting of things,
		// but I realized that I want IExecutableUnit to be an information and invoking interface, not one made
		// for setting features or flags.

		std::string _name;

		//TODO: Please move Overload functionality back to base. Instead, give this a generic base
		GenericBase* base = nullptr;
		
		uint32_t defaultIndex = (uint32_t)-1;  //max_value<size_t>;//basically whenever the defaults start.





		//
		std::optional<Procedure> _procedure = std::nullopt;

		//formulas won't have defaults, they don't have names, and they don't have procedures (such would defy the point of them.


		Routine* GetRoutine()
		{
			//This plans to be a pointer later, as this will end up just being 
			return &_routine;
		}

		std::optional<Procedure> GetProcedure()
		{
			return _procedure;
		}

		std::string_view name() const
		{
			return _name;
		}

	public:

		uint32_t GetArgCountReq() const
		{
			if (defaultIndex != -1)
				return defaultIndex;

			return __super::GetArgCountReq();
		}


		std::array<uint32_t, 2> GetArgRange()
		{
			return { GetArgCountReq(), GetArgCountMax() };
		}


		void CheckDefault(size_t index, size_t offset, OverloadFlag& flags)
		{
			if (defaultIndex <= index && !offset) {

				flags |= OverloadFlag::DefFilled;
			}
		}
		
		void CheckVariadic(size_t& index, size_t offset)
		{
			if (vardIndex <= index && !offset) {

				index = vardIndex;
			}
		}


		bool MatchImpliedEntryConcrete(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags);

		bool MatchStatedEntryConcrete(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags);




		bool CanMatch(const QualifiedType& target, size_t callArgs, size_t tempArgs, OverloadFlag flags);

		bool MatchImpliedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags);

		bool MatchStatedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags);


		bool ResolveOverload(Overload& result, OverloadFlag& flags);



		void QualifyOverload(Overload& out);


		RuntimeVariable BasicExecute(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def)
		{
			return __super::BasicExecute(self, body, args, caller, def, GetProcedure());
		}

		//TypeInfo* GetConcreteReturnType();//move to abstractFunction


		//Possible use in deductions with generics, then I realized this isn't C++ and auto cannot exactly exist
		// like I think it would.





	};

#else



	//One half of the signature that would make up the signature that 
	struct BasicCallSignature
	{
		QualifiedType _returnType = nullptr;
		std::vector<ParameterInfo> parameters;

	};



		
		



	struct BasicCallableData : public BasicCallSignature
	{
		std::unique_ptr<ThisInfo> _thisInfo;

	public:

		union
		{
			intptr_t raw = 0;
			Routine* _routine;
			Procedure _procedure;
		};



		uint32_t vardIndex = (uint32_t)-1;
		uint32_t defaultIndex = (uint32_t)-1;

		void VisitParameters(std::function<void(IVarIndexInfo&)> func)
		{
			if (_thisInfo)
				func(*_thisInfo);

			for (auto& param : parameters)
			{
				func(param);
			}
		}


		uint32_t GetParamCount() const
		{
			return GetArgCount() + HasTarget();
		}

		uint32_t GetArgCount() const
		{
			return parameters.size();
		}


		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


		bool HasVadiadic() const
		{
			return vardIndex != -1;
		}

		ParameterInfo* GetVariadicParameter()
		{
			if (vardIndex != -1) {
				return &parameters[vardIndex];
			}

			return nullptr;
		}

	public:


		uint32_t GetArgCountReq() const
		{
			if (vardIndex != -1)
				return vardIndex;

			return GetArgCount();
		}
		uint32_t GetArgCountMax() const
		{
			if (vardIndex != -1)
				return -1;

			return GetArgCount();
		}


		std::array<uint32_t, 2> GetArgRange() const
		{
			return { GetArgCountReq(), GetArgCountMax() };
		}

		uint32_t GetParamCountReq() const
		{
			return GetArgCountReq() + HasTarget();
		}

		uint32_t GetParamCountMax() const
		{
			auto res = GetArgCountMax();
			return std::max(res + HasTarget(), res);
		}

		std::array<uint32_t, 2> GetParamRange() const
		{
			return { GetParamCountReq(), GetParamCountMax() };
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



		//Needs to be moved into IFunction/Function.
		QualifiedType GetReturnType() const
		{
			return _returnType;
		}

		QualifiedType GetReturnType(ITemplatePart* part) const
		{
			return _returnType.QualifySpecial(part);
		}


		QualifiedType GetTargetType() const
		{
			return _thisInfo ? _thisInfo->GetQualifiedType() : nullptr;;
		}

		bool HasTarget() const
		{
			return !!_thisInfo;
		}

		//These sorts of things should be protected, not used up front.
		//std::string name(){return _name;}

		IVarIndexInfo* FindParameter(const std::string_view& a_name)
		{
			if (a_name == parse_strings::this_word) {
				return _thisInfo.get();
			}

			auto end = parameters.end();
			auto it = std::find_if(parameters.begin(), end, [&](ParameterInfo& q) { return q.GetName() == a_name; });
			if (it != end) {
				return &*it;
			}
			return nullptr;
		}



		//-1 is for this
		IVarIndexInfo* FindParameterByPos(size_t index)
		{
			if (index == -1) {
				return _thisInfo.get();
			}

			return std::addressof(parameters.at(index));
		}



	protected:
		Routine* ObtainRoutine()
		{
			if (!_routine) {
				_routine = new Routine;
			}

			return _routine;
		}


		RuntimeVariable BasicExecute(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def,
			FunctionBody type);


		RuntimeVariable BasicInvoke(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, RuntimeVariable* def,
			FunctionBody type);

	};


	//This is basically something that only handles routines, and will create and destroy the routine when loaded and destroyed
	// This is for formulas primarily.
	struct RoutineData : public BasicCallableData
	{
		RoutineData()
		{
			ObtainRoutine();
		}

		~RoutineData()
		{
			if (_routine)
				delete _routine;
		}

		Routine& GetRoutine()
		{
			//This plans to be a pointer later, as this will end up just being
			return *_routine;
		}



		RuntimeVariable BasicExecute(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def)
		{
			return BasicCallableData::BasicExecute(self, body, args, caller, def, FunctionBody::Routine);
		}

		RuntimeVariable BasicInvoke(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, RuntimeVariable* def)
		{
			return BasicCallableData::BasicInvoke(self, body, args, def, FunctionBody::Routine);
		}
	};

	struct FunctionData : public BasicCallableData
	{

		~FunctionData()
		{
			switch (GetBodyType())
			{
			case FunctionBody::Routine:
				delete _routine;
				break;
			}
		}



		std::string _name;

		//TODO: Please move Overload functionality back to base. Instead, give this a generic base
		GenericBase* base = nullptr;
		
		Specifier specifiers;
	protected:
		//If not 0, this is a procedure. This data is used by the procedure. May use to help handle if 
		//If -1 its likely waiting for a procedure to claim it.
		uint64_t procedureData = 0;

	public:


		//formulas won't have defaults, they don't have names, and they don't have procedures (such would defy the point of them.


		constexpr FunctionBody GetBodyType() const noexcept
		{
			if (procedureData) {
				return FunctionBody::Procedure;
			}
			else if (_routine) {
				return FunctionBody::Routine;
			}
			else {
				return FunctionBody::None;
			}
		}

		constexpr bool HasProcedure() const noexcept
		{
			return procedureData;
		}

		std::optional<Procedure> GetProcedure()
		{
			return HasProcedure() ? std::make_optional(_procedure) : std::nullopt;
		}


		Routine* GetRoutine()
		{
			//This plans to be a pointer later, as this will end up just being 
			return !HasProcedure() ? _routine : nullptr;
		}


		std::string_view name() const
		{
			return _name;
		}





		uint32_t GetArgCountReq() const
		{
			if (defaultIndex != -1)
				return defaultIndex;

			return __super::GetArgCountReq();
		}


		std::array<uint32_t, 2> GetArgRange()
		{
			return { GetArgCountReq(), GetArgCountMax() };
		}


		void CheckDefault(size_t index, size_t offset, OverloadFlag& flags)
		{
			if (defaultIndex <= index && !offset) {

				flags |= OverloadFlag::DefFilled;
			}
		}

		void CheckVariadic(size_t& index, size_t offset)
		{
			if (vardIndex <= index && !offset) {

				index = vardIndex;
			}
		}

		//Try to move these back if you can.

		bool MatchImpliedEntryConcrete(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags);

		bool MatchStatedEntryConcrete(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags);



		//TODO: These functions need to have TemplateContainers sent instead of using the base. 
		// This way I can get a signature to handle this as well. Lo
		bool CanMatch(const QualifiedType& target, size_t callArgs, size_t tempArgs, OverloadFlag flags);
		
		bool MatchImpliedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags);

		bool MatchStatedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags);


		bool ResolveOverload(Overload& result, OverloadFlag& flags);



		void QualifyOverload(Overload& out);


		RuntimeVariable BasicExecute(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def)
		{
			return BasicCallableData::BasicExecute(self, body, args, caller, def, GetBodyType());
		}

	};


#endif
}
