#pragma once

namespace LEX
{
	ENUM(Register, uint8_t)
	{
		Reg0,
		Reg1,
		Reg2,
		Reg3,
		Reg4,
		Total,

		//Alias names.
		Result		= Register::Reg0,
		Middle		= Register::Reg1,
		Left		= Register::Reg2,
		Right		= Register::Reg3,
		MultComp	= Register::Reg4,
		Invalid = Register::Total,
	};

}