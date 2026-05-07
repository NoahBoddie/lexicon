#pragma once

#include "Lexicon/Interfaces/FunctionInfo.h"
#include "Lexicon/Engine/OverloadParameter.h"

#include "Lexicon/Engine/MemberInfo.h"
namespace LEX
{

	struct FunctionNode;
	struct ITemplatePart;

    struct OverloadInfo : public FunctionInfo, public OverloadParameter
    {
        virtual void Destroy() {}


		virtual FunctionNode CreateNode(ITemplatePart* part) = 0;
		virtual bool IsOverloadUsuable() const { return true; }
		//virtual BasicCallSignature* GetCallSignature() = 0;
		//virtual TemplateContainer* GetGenericSignature() = 0;
    };


#ifdef KILL_THIS_WHEN_DONE
	//Siphon data from this pls
	struct FunctionInfo_OLD : public MemberInfo, public OverloadParameter
	{
		using FunctionType = FunctionBase;


		struct {

			FunctionData* signature = nullptr;
			//uint64_t signatureCode = 0;

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

		FunctionNode CreateNode(ITemplatePart* part);
		//{
		//	if (IsVirtual() == true)
		//		return FunctionNode{ nullptr, signature, method };
		//	else
		//		return FunctionNode{ function, signature, function->AsFunction()->CheckFunction(part) };
		//}


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
			if (IsVirtual() == false)
				return function->AsFunction();
			return nullptr;
		}

		IFunction* GetFunction(ITemplatePart* part) const
		{
			if (IsVirtual() == false)
				return function->AsFunction()->CheckFunction(part);
			nullptr;
		}

		FunctionType* Get() const
		{
			if (IsVirtual() == false)
				return function;

			return nullptr;
		}


		bool IsValid() const
		{
			if (auto base = Get())
				return !base->InvalidFlag();

			return true;
		}


		FieldType GetFieldType() const override { return FieldType::Function; }

		//For now, this is true, there is no way to handle a function's type.
		ITypeInfo* GetType() const override { return nullptr; }


		Qualifier GetQualifiers() const override { return Qualifier{ Refness{}, Constness::Const, {} }; }	//Functions do not have qualifiers.
		Specifier GetSpecifiers() const override { return specifiers; }

		std::string GetFieldName() const override { return std::string{ Get()->GetName() }; }

		operator bool() const override
		{
			return _raw || signature;
		}



		bool CanMatch(const QualifiedType& target, size_t callArgs, size_t tempArgs, OverloadFlag flags) override
		{
			return signature->CanMatch(target, callArgs, tempArgs, flags);
		}

		bool MatchImpliedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags) override
		{
			return signature->MatchImpliedEntry(out, type, scope, overload, index, offset, flags);
		}


		bool MatchStatedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags) override
		{
			return signature->MatchStatedEntry(out, type, scope, overload, name, flags);
		}


		void QualifyOverload(Overload& overload) override
		{
			return signature->QualifyOverload(overload);
		}

		bool ResolveOverload(Overload& entries, OverloadFlag& flags) override
		{
			return signature->ResolveOverload(entries, flags);
		}
	};


#endif
}