#pragma once

#include "Lexicon/Variable.h"
#include "RoutineBase.h"
#include "ParameterInfo.h"
//Should have parameterinfos?

#include "Lexicon/Engine/QualifiedType.h"

#include "Lexicon/Engine/OverloadFlag.h"

//*src
#include "Lexicon/Engine/parse_strings.h"

namespace LEX
{
	class ParameterInfo;
	
	struct ProcedureData;

	struct Overload;
	struct OverloadEntry;

	//Definition is the name of the struct that holds either routine data, or caller data.
	//Or maybe it should be an enum.
	using Procedure = void(*)(RuntimeVariable&, Variable*, std::span<Variable*>, ProcedureData&);
	
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

		RoutineBase _routine;  //actually needs to be a pointer


		void VisitParameters(std::function<void(ParameterInfo&)> func)
		{
			if (_thisInfo)
				func(*_thisInfo);

			for (auto& param : parameters)
			{
				func(param);
			}
		}
		

		
		RoutineBase* GetRoutine()
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
			return (uint32_t)GetArgCount() + !!_thisInfo;
		}

		size_t GetArgCount() const
		{
			return parameters.size();
		}


		//For now, the maximum and minimum is both the same. Later, defaults will exist, so they don't have to
		//be exact, and params keyword will hopefully exist at some point.
		size_t GetArgCountReq()
		{
			return GetArgCount();
		}
		size_t GetArgCountMax()
		{
			return GetArgCount();
		}


		std::array<size_t, 2> GetArgRange()
		{
			return { GetArgCountReq(), GetArgCountMax() };
		}

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

		ParameterInfo* FindParameter(std::string_view a_name)
		{
			if (a_name == parse_strings::this_word) {
				return _thisInfo.get();
			}

			auto end = parameters.end();
			auto it = std::find_if(parameters.begin(), end, [&](ParameterInfo& q) { return q.GetFieldName() == a_name; });
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

	};

	struct FunctionData : public BasicCallableData
	{
		//A struct to be owned protected.

		//weren't functions environments? A: They don't need to be anymore.
		//Data



		//These are FunctionData: Functions, GenericFunctions, and Formulas.
		// Yes, formulas are function data. I thought to make CallableUnit handle the setting of things,
		// but I realized that I want ICallableUnit to be an information and invoking interface, not one made
		// for setting features or flags.

		std::string _name;

		//TODO: Please move Overload functionality back to base. Instead, give this a generic base
		GenericBase* base = nullptr;
		





		size_t defaultIndex = (size_t)-1;  //max_value<size_t>;//basically whenever the defaults start.
		size_t paramsIndex = (size_t)-1;
		//
		Procedure _procedure = nullptr;

		//formulas won't have defaults, they don't have names, and they don't have procedures (such would defy the point of them.


		RoutineBase* GetRoutine()
		{
			//This plans to be a pointer later, as this will end up just being 
			return &_routine;
		}

		Procedure GetProcedure()
		{
			return _procedure;
		}

		std::string_view name() const
		{
			return _name;
		}
	public:


		size_t GetArgCountReq()
		{
			if (defaultIndex != -1)
				return defaultIndex;

			return GetArgCount();
		}
		size_t GetArgCountMax()
		{
			if (paramsIndex != -1)
				return -1;

			return GetArgCount();
		}


		std::array<size_t, 2> GetArgRange()
		{
			return { GetArgCountReq(), GetArgCountMax() };
		}




		void CheckDefault(size_t index, size_t offset, OverloadFlag& flags)
		{
			if (defaultIndex <= index && !offset) {

				flags |= OverloadFlag::DefFilled;
			}
		}
		

		bool MatchImpliedEntryConcrete(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags);

		bool MatchStatedEntryConcrete(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags);




		bool CanMatch(const QualifiedType& target, size_t callArgs, size_t tempArgs, OverloadFlag flags);

		bool MatchImpliedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags);

		bool MatchStatedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags);


		bool ResolveOverload(Overload& result, OverloadFlag& flags);



		void QualifyOverload(Overload& out);


		//TypeInfo* GetConcreteReturnType();//move to abstractFunction


		//Possible use in deductions with generics, then I realized this isn't C++ and auto cannot exactly exist
		// like I think it would.






	};

}
