#pragma once


namespace LEX
{
	struct Solution;


	struct TargetObject
	{
		enum Flag : uint8_t
		{
			None		= 0 << 0,
			Explicit	= 1 << 0,
			Assign		= 1 << 1,//When the target object is instead the expected assigning type.
		};


		//Size isn't used to its fullest

		//So issue, this actually used to use a solution, and rightfully so. Target object
		// as itself 





		Solution*		target = nullptr;
		TargetObject*	prev = nullptr;
		Flag			flag = Flag::Explicit;

		//Purges the use of assigning TargetObjects. This is used when returning or when assigning for implicit
		// constuction knowledge.
		TargetObject* GetCallTarget()
		{
			if (this){
				return flag & Flag::Assign ? prev->GetCallTarget() : this;
			}
			
			return nullptr;
		}

		//Reverse of call target, this gets the target of an assignment or return. Useful for ternary statements
		TargetObject* GetAssignTarget()
		{
			if (this) {
				return flag & Flag::Assign ? this : prev->GetAssignTarget();
			}

			return nullptr;
		}

		TargetObject(Solution& t, TargetObject* p, Flag f) : target{ &t }, prev{ p }, flag{ f }
		{

		}

		TargetObject(Solution& t, TargetObject* p = nullptr, bool b = false) : target{ &t }, prev{ p }, flag{ b ? Explicit : None }
		{

		}
	};
}