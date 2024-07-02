#pragma once 


namespace LEX
{
	class FunctionHandle;

	class Delegate
	{
		//It's possible I opt to make my own container for this, just to cut on space
		//Either way, to get what type ID array is using, one need only (but only within source)

		//notice to the above, delegate will need 

		//Structure is very close to that of a delegate
		std::vector<FunctionHandle> _callbacks;


		AbstractTypePolicy* type;

	public:
		constexpr std::partial_ordering operator <=> (const Delegate& rhs) const
		{
			//Similar to array, but should be measured by it's count of callbacks.
			return std::strong_ordering::equal;
		}


		constexpr bool operator==(const Delegate& a_rhs) const
		{
			return operator<=>(a_rhs) == std::strong_ordering::equal;
		}

	};

}