#pragma once

#include "Lexicon/ITemplateBodyPart.h"

namespace LEX
{
	struct Solution;


	struct TargetObject : public ITemplateBodyPart
	{
		//TODO: Award TargetObject the ability to get RoutineCompiler's target object.

		enum Flag : uint8_t
		{
			None = 0 << 0,
			Implicit = 1 << 0,
		};

		//This should be the ITemplatePart/Body that is used in the MergeTemplate. The array it uses should come from the target
		// solution, being empty if there is no solution. The Solution should use the ITypeInfo to get the templates used on it.

		Solution* target = nullptr;
		TargetObject* const prev = nullptr;
		TargetObject*& slot;
		Flag			flag = Flag::None;
		
		//The compiler would have it's hand on who stores incompletes. So, use that instead of the target
		//RoutineCompiler* compiler = nullptr;
		
		//Might store the span here just to make it less ass to pull.


		size_t GetSize() const override;

		ITypeInfo* GetPartArgument(size_t i) const override;

		TypeInfo* GetBodyArgument(size_t i) const override;


		GenericBase* GetClient() const override;



		bool IsValid() const;

		bool IsResolved() const override;


		bool IsExplicit() const
		{
			return !IsImplicit();
		}

		bool IsImplicit() const
		{
			if (!this)
				return true;

			return flag & Flag::Implicit;
		}

		Solution* GetSolution()
		{
			return this ? target : nullptr;
		}

		TargetObject(Solution* t, TargetObject*& p, Flag f = Flag::None) : target{ t }, slot {p}, prev{p}, flag{f}
		{
			slot = this;
		}

		~TargetObject()
		{
			slot = prev;
		}
	};
}