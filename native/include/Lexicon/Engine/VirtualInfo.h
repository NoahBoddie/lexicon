#pragma once

#include "Lexicon/Engine/MemberInfo.h"
#include "Lexicon/Engine/FunctionBase.h"


#include "Lexicon/Specifier.h"
#include "Lexicon/MemberPointer.h"
#include "Lexicon/Engine/FunctionData.h"
#include "Lexicon/Engine/OverloadInfo.h"
#include "Lexicon/Engine/TemplateContainer.h"


namespace LEX
{


	struct VirtualInfo : public OverloadInfo
	{
		~VirtualInfo()
		{
			if (IsPureVirtual() == true) {
				delete thisInfo;
				delete callSign;
			}

			if (genSign)
				genSign->Destroy();
		}


		void Destroy() override { delete this; }
		std::string name;
		MemberPointer method;
		ThisInfo* thisInfo = nullptr;
		BasicCallSignature* callSign = nullptr;
		TemplateContainer* genSign = nullptr;
		Specifier specifiers{};


		IFunction* GetFunction()
		{
			if (IsPureVirtual() == true)
				return nullptr;

			FunctionBase* base = static_cast<FunctionBase*>(callSign);

			return base->AsFunction();
		}

		bool IsPureVirtual() const
		{
			//The virtual flag is removed once when this is created, and is added in whenever transfered
			// if it's still press
			return specifiers.flags & SpecifierFlag::Virtual;
		}



		/*
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
		//*/

	};
}
