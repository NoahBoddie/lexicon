#pragma once


#include "Field.h"
#include "Element.h"
#include "IVariable.h"

namespace LEX
{

	struct Global :
		public Element,
		public IVariable,
		public Field//Might be more here, dunno.
	{
		std::string name;

		//Determines if this is a literal, constant, or property. The 2 former of cannot be changed, while the later can.
		//For now though, all of these will be constant.
		bool constant{};//SHould be qualifier

		//Move this below please.
		std::strong_ordering operator<=>(const Global& a_rhs) const
		{
			//Too lazy to set this up right now.
			return std::strong_ordering::equal;
		}


	};
}