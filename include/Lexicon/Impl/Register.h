#pragma once

namespace LEX::Impl
{
	enum struct Register
	{
		Reg0,
		Reg1,
		Reg2,
		Reg3,
		Total,

		//Alias names.
		Result = Reg0,
		Left = Reg1,
		Right = Reg2,
		Array = Reg3,
		Invalid = Total,
	};

}