#pragma once


#include "LocalInfo.h"

namespace LEX
{

	class ParamInfo : public LocalInfo
	{
		struct Settings : public __super::Settings
		{
			bool isDefault = false;
		};

	};
}
