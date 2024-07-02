#pragma once

namespace LEX
{
	struct Origin
	{
		std::type_info*		type = nullptr;
		HMODULE				program = nullptr;
	};
}