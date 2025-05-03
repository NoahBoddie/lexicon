#pragma once

#include "Lexicon/Engine/MemberInfo.h"
#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/MemberPointer.h"

namespace LEX
{




	struct FunctionNode
	{
		enum Type
		{
			kInvalid,
			kMethod,
			kFunction
		};

		FunctionNode(FunctionData* data, const MemberPointer& met) : _type{ kMethod }, signature{ data }, method{ met } {}
		FunctionNode(FunctionData* data, IFunction* func) : _type{ kFunction }, signature{ data }, function{ func } {}


	private:
		union
		{
			uint64_t raw = 0;
			MemberPointer method;
			IFunction* function;
		};

	public:
		FunctionData* signature = nullptr;


		Type _type = kInvalid;


		Type type() const
		{
			return _type;
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

	struct FunctionInfo : public MemberInfo, public OverloadClause
	{
		using FunctionType = FunctionBase;
		

		struct {

			FunctionData* signature = nullptr;
			

			struct
			{
				uint64_t _raw = 0;
				FunctionType* function;
				MemberPointer method;//prefered, works with the other.
				
			};
		};

		bool IsVirtual() const
		{
			return specifiers.flags & SpecifierFlag::Virtual;
		}

		FunctionNode CreateNode(ITemplatePart* part)
		{
			if (IsVirtual() == true)
				return FunctionNode{ signature, method };
			else
				return FunctionNode{ signature, function->AsFunction()->CheckFunction(part) };
		}


		FunctionData* tmpSignature()
		{
			return signature;
		}

		MemberPointer GetMethod() const
		{
			return method;
		}


		IFunction* GetFunction() const
		{
			return function->AsFunction();
		}

		IFunction* GetFunction(ITemplatePart* part) const
		{
			return function->AsFunction()->CheckFunction(part);
		}

		FunctionType* Get() const
		{
			if (IsVirtual() == false)
				return function;

			return nullptr;
		}

		FieldType GetFieldType() const override { return FieldType::Function; }

		//For now, this is true, there is no way to handle a function's type.
		ITypeInfo* GetType() const override { return nullptr; }


		Qualifier GetQualifiers() const override { return Qualifier{ Reference{}, Constness::Const, {} }; }	//Functions do not have qualifiers.
		Specifier GetSpecifiers() const override { return specifiers; }

		std::string GetFieldName() const override { return std::string{ Get()->GetName() }; }

		operator bool() const override
		{
			return _raw || signature;
		}


		bool CanMatch(QualifiedType type, size_t suggested, size_t optional, OverloadFlag flag) override
		{
			return signature->CanMatch(type, suggested, optional, flag);
		}


		//Fuck it, these return non-booleans and use something else to denote their failures.

		OverloadEntry MatchSuggestedEntry(QualifiedType type, ITypeInfo* scope, size_t offset, size_t index, OverloadFlag& flags) override
		{
			return signature->MatchSuggestedEntry(type, scope, offset, index, flags);

		}
		OverloadEntry MatchDefaultEntry(QualifiedType type, ITypeInfo* scope, std::string name, OverloadFlag& flags) override
		{
			return signature->MatchDefaultEntry(type, scope, name, flags);
		}

		std::vector<OverloadEntry> ResolveEntries(Overload& entries, ITypeInfo* scope, OverloadFlag& flags) override
		{
			return signature->ResolveEntries(entries, scope, flags);
		}


	};


}
