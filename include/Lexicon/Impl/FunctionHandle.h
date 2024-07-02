#pragma once 

#include "MemberPointer.h"

namespace LEX
{
	class AbstractFunction;


	class FunctionHandle
	{
		//I completely forgot, this can hold onto methods. This is GOING to need to be an index instead. Consult the method function syntax instead.

		std::variant<Void, AbstractFunction*, MemberPointer> _data{};

		//AbstractFunction is a function, the MemberPointer is a method.

	public:
		//I won't fuck with this shit for right now.
		constexpr std::strong_ordering operator <=> (const FunctionHandle& rhs) const
		{
			return {};
			//return _ptr <=> rhs._ptr;
		}


		constexpr bool operator==(const FunctionHandle& a_rhs) const
		{
			return {};
			//return operator<=>(a_rhs) == std::strong_ordering::equal;
		}

		//The abstract function is baiscally what's supposed to be used above all else. It's basically the handler or something, to prevent real interaction with the abstract 
		// function, or just to spruce it the process of calling some, as well as handling and ditating the return. (I could also make a template handle that).
	};

}