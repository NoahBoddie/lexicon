#pragma once

namespace LEX::Impl
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
		Left		= Register::Reg1,
		Right		= Register::Reg2,
		Array		= Register::Reg3,
		MultComp	= Register::Reg4,
		Invalid = Register::Total,
	};

}

using Register = LEX::Impl::Register;