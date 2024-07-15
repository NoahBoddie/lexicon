#pragma once

#include "Lexicon/Variable.h"
#include "RoutineBase.h"
#include "ParameterInfo.h"
//Should have parameterinfos?

#include "Lexicon/QualifiedType.h"



namespace LEX
{
	struct ITypePolicy;
	class ParameterInfo;
	
	struct ProcedureData;

	//Definition is the name of the struct that holds either routine data, or caller data.
	//Or maybe it should be an enum.
	using Procedure = void(*)(RuntimeVariable&, Variable* target, std::vector<Variable*>, ProcedureData&);
	
	struct BasicCallableData
	{
		QualifiedType _returnType = nullptr;

		//Without a target type, this is a static function.
		//Target type remains implicit
		ITypePolicy* _targetType = nullptr;

		//The actual object for this has severely changed. It's "ParamInfo" I think? But this isn't terribly important
		// here yet so you know.
		//std::vector<DeclaredParam> reqParams;
		//std::vector<DefinedParam> defParams;

		//TODO: I would like to represent "this" as a unique pointer to a ParamInfo
		std::unique_ptr<ParameterInfo> __thisInfo;
		std::vector<ParameterInfo> parameters;


		RoutineBase _routine;  //actually needs to be a pointer


		
		RoutineBase* GetRoutine()
		{
			//This plans to be a pointer later, as this will end up just being
			return &_routine;
		}

		//I won't care about any of these I'll keep it a buck.
	protected:
		auto _ParamBegin()
		{
			auto begin = parameters.begin();

			return begin;
		}

		auto _ParamEnd()
		{
			auto end = parameters.end();
			return end;
		}

		auto _ParamBegin() const
		{
			auto begin = parameters.begin();

			return begin;
		}

		auto _ParamEnd() const
		{
			auto end = parameters.end();
			return end;
		}

	public:
		std::vector<ParameterInfo> GetParameters()
		{
			return { _ParamBegin(), _ParamEnd() };
		}

		std::vector<ParameterInfo> GetParameters() const
		{
			return { _ParamBegin(), _ParamEnd() };
		}

		size_t GetParamCount() const
		{
			return parameters.size();
		}

		size_t GetParamAllocSize() const
		{
			return parameters.size();
		}

		//For now, the maximum and minimum is both the same. Later, defaults will exist, so they don't have to
		//be exact, and params keyword will hopefully exist at some point.
		size_t GetReqArgCount()
		{
			return GetParamCount();
		}
		size_t GetMaxArgCount()
		{
			return GetParamCount();
		}

		std::array<size_t, 2> GetArgRange()
		{
			return { GetReqArgCount(), GetMaxArgCount() };
		}

		//Needs to be moved into ITypePolicy/AbstractTypePolicy.
		QualifiedType GetReturnType() const
		{
			return _returnType;
		}

		ITypePolicy* GetTargetType() const
		{
			return _targetType;
		}

		//These sorts of things should be protected, not used up front.
		//std::string name(){return _name;}

		ParameterInfo* FindParameter(std::string a_name)
		{
			auto end = parameters.end();
			auto it = std::find_if(parameters.begin(), end, [&](ParameterInfo& q) { return q.GetFieldName() == a_name; });
			if (it != end) {
				return &*it;
			}
			return nullptr;
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

		
		size_t defaultIndex = (size_t)-1;  //max_value<size_t>;//basically whenever the defaults start.
		
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

	protected:
		auto _ParamBegin()
		{
			auto begin = parameters.begin();

			if (HasTarget() == true)
				begin++;

			return begin;
		}

		auto _ParamEnd()
		{
			auto end = parameters.end();
			return end;
		}
		
		auto _ParamBegin() const
		{
			auto begin = parameters.begin();

			if (HasTarget() == true)
				begin++;

			return begin;
		}

		auto _ParamEnd() const
		{
			auto end = parameters.end();
			return end;
		}

	public:


		std::vector<ParameterInfo> GetParameters()
		{
			return { _ParamBegin(), _ParamEnd() };
		}


		std::vector<ParameterInfo> GetParameters() const
		{
			return { _ParamBegin(), _ParamEnd()};
		}

		size_t GetParamCount() const
		{
			return parameters.size() - HasTarget();
		}

		size_t GetParamAllocSize() const
		{
			return parameters.size();
		}

		//For now, the maximum and minimum is both the same. Later, defaults will exist, so they don't have to
		//be exact, and params keyword will hopefully exist at some point. 
		size_t GetReqArgCount()
		{
			return GetParamCount();
		}
		size_t GetMaxArgCount()
		{
			return GetParamCount();
		}

		std::array<size_t, 2> GetArgRange()
		{
			return { GetReqArgCount(), GetMaxArgCount() };
		}

		//Needs to be moved into ITypePolicy/AbstractTypePolicy.
		QualifiedType GetReturnType() const
		{
			return _returnType;
		}

		ITypePolicy* GetTargetType() const
		{
			return _targetType;
		}

		//These sorts of things should be protected, not used up front.
		//std::string name(){return _name;}

		ParameterInfo* FindParameter(std::string a_name)
		{
			auto end = parameters.end();
			auto it = std::find_if(parameters.begin(), end, [&](ParameterInfo& q) {return q.GetFieldName() == a_name; });
			if ( it != end) {
				return &*it;
			}
			return nullptr;
		}

		bool HasTarget() const
		{
			return _targetType;
		}

		//AbstractTypePolicy* GetConcreteReturnType();//move to abstractFunction


		//Possible use in deductions with generics, then I realized this isn't C++ and auto cannot exactly exist
		// like I think it would.
		//void SetReturnType(ITypePolicy*);
	};

}
