#pragma once

#include "Lexicon/Engine/MemberInfo.h"
#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/MemberPointer.h"

namespace LEX
{

	struct FunctionInfo : public MemberInfo, public OverloadClause
	{
		using FunctionType = FunctionBase;//
		


		struct Newstructure
		{
			//Since this takes up the same about of space, I think what I'll do is this.
			//it holds function data

			//So the concept here is it always holds function data, and that this is the thing that's used with overloads.
			//This thing to find if something matches will try for equality.

			//the structure

			//There's a slight problem with the idea and the implementation of generics would cause me some issues. So, instead, I think it would probably be better
			// to just have it the way that I have it now, either a function or a method pointer. That being said, I can likely drop the FunctionData and just stick with the member
			// pointer, having an empty function at said location.
			// I might instead spend that information on this being able to know it's owner, OR just require that to be submitted in order to get that.

			FunctionData* signature;

			union
			{
				uint64_t raw = 0;
				MemberPointer virtualMethod;
				FunctionBase* declaredMethod;
			};

			bool isPureVirtual = false;
		};


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

		FunctionType* Get() const
		{
			if (IsVirtual() == false)
				return function;

			return nullptr;
		}

		FieldType GetFieldType() const override { return FieldType::Function; }

		//For now, this is true, there is no way to handle a function's type.
		ITypePolicy* GetType() const override { return nullptr; }


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

		OverloadEntry MatchSuggestedEntry(QualifiedType type, ITypePolicy* scope, size_t offset, size_t index, OverloadFlag& flags) override
		{
			return signature->MatchSuggestedEntry(type, scope, offset, index, flags);

		}
		OverloadEntry MatchDefaultEntry(QualifiedType type, ITypePolicy* scope, std::string name, OverloadFlag& flags) override
		{
			return signature->MatchDefaultEntry(type, scope, name, flags);
		}

		std::vector<OverloadEntry> ResolveEntries(Overload& entries, ITypePolicy* scope, OverloadFlag& flags) override
		{
			return signature->ResolveEntries(entries, scope, flags);
		}


	};

}
