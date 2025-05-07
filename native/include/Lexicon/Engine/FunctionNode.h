#pragma once

#include "Lexicon/Engine/FunctionData.h"
#include "Lexicon/MemberPointer.h"

namespace LEX
{

	class FunctionBase;


	struct FunctionNode
	{
		enum Type
		{
			kInvalid,
			kMethod,
			kFunction
		};
		FunctionNode() = default;
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

		constexpr operator bool() const
		{
			return signature && raw;
		}

	};

}