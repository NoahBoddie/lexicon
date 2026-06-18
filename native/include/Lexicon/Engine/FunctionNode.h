#pragma once

#include "Lexicon/Engine/FunctionData.h"
#include "Lexicon/MemberPointer.h"

namespace LEX
{

	class FunctionBase;


	//I'd like to rework how FunctionNode will work. I think it will literally just be a checked type and 
	// an object that's used for specialization. The rest of the types can be stored on the s
	struct FunctionNode
	{
		enum Type
		{
			kInvalid,
			kMethod,
			kFunction
		};
		FunctionNode() {};
		FunctionNode(FunctionBase* b, FunctionData* data, const MemberPointer& met) : _type{ kMethod }, base{b}, signature { data }, method{ met } {}
		FunctionNode(FunctionBase* b, FunctionData* data, IFunction* func) : _type{ kFunction }, base{ b }, signature{ data }, function{ func } {}


	private:
		union
		{
			uint64_t raw = 0;
			MemberPointer method;
			IFunction* function;
		};

	public:
		FunctionData* signature = nullptr;
		FunctionBase* base = nullptr;
		ISpecializable* special = nullptr;
		Type _type = kInvalid;


		Type type() const
		{
			return _type;
		}

		FunctionBase* GetBase()
		{
			return base;
		}

		FunctionData* GetSignature()
		{
			return signature;
		}

		MemberPointer GetMethod() const
		{
			if (_type != kMethod)
				return {};

			return method;
		}

		IFunction* GetFunction() const
		{
			if (_type != kFunction)
				return {};


			return function;
		}

		ISpecializable* GetSpecialization()
		{
			if (_type != kFunction)
				return {};


			return function ? function->GetSpecializable() : nullptr;
		}

		constexpr operator bool() const
		{
			return signature && raw;
		}

	};

}