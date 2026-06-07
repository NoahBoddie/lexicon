#pragma once

#include "Lexicon/ITemplateBodyPart.h"

namespace LEX
{
	struct Solution;


	struct TargetObject : public ITemplateBodyPart
	{
		//TODO: Award TargetObject the ability to get RoutineCompiler's target object.
		//TODO: This has less needs than it used to. Please scale TargetObject's set up back a bit
		



		enum Flag : uint8_t
		{
			None = 0 << 0,
			Implicit = 1 << 0,
			Stale = 1 << 1,
		};

		//This should be the ITemplatePart/Body that is used in the MergeTemplate. The array it uses should come from the target
		// solution, being empty if there is no solution. The Solution should use the ITypeInfo to get the templates used on it.

		Solution*			target = nullptr;
		TargetObject* const prev = nullptr;
		TargetObject**		slot = nullptr;
		Flag				flags = Flag::None;
		
		//The compiler would have it's hand on who stores incompletes. So, use that instead of the target
		//RoutineCompiler* compiler = nullptr;
		
		//Might store the span here just to make it less ass to pull.


		size_t GetSize() const override;

		ITypeInfo* GetPartArgument(size_t i) const override;

		TypeInfo* GetBodyArgument(size_t i) const override;


		GenericBase* GetClient() const override;



		bool IsValid() const;

		bool IsResolved() const override;


		void SetFlag(Flag flag, bool value)
		{
			if (value) {
				flags |= flag;
			}
			else {
				flags &= ~flag;
			}
		}

		bool IsStale() const
		{
			return flags & Flag::Stale;
		}

		bool IsExplicit() const
		{
			return !IsImplicit();
		}

		bool IsImplicit() const
		{
			if (!this)
				return true;

			return flags & Flag::Implicit;
		}

		Solution* GetSolution()
		{
			return this ? target : nullptr;
		}
		
		Solution& solution()
		{
			return *target;
		}


		TargetObject(Solution* t, Flag f = Flag::None) : target{ t }, flags{ f }
		{
			if (slot)
				*slot = this;
		}

		TargetObject(Solution* t, TargetObject*& p, Flag f = Flag::None) : target{ t }, slot {&p}, prev{p}, flags{f}
		{
			if (slot)
				*slot = this;
		}


	private:
		TargetObject(const TargetObject&) = default;
	public:
		TargetObject(TargetObject&& other) : TargetObject(other)
		{
			if (*slot) 
				*slot = this;
			
			other.slot = nullptr;
		}

		~TargetObject()
		{
			if (slot)
				*slot = prev;
		}
	};
}