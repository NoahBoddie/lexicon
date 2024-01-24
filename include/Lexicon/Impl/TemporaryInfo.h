#pragma once

#include "InfoBase.h"

namespace LEX
{

	class TemporaryInfo : public InfoBase
	{
	public:
		TemporaryInfo(size_t i) : _index{ i } {}

		BasicQualifier GetBasicFlags() const override
		{
			return GetData<Settings>().basic;
		}

		RuntimeQualifier GetRuntimeFlags() const override
		{
			return GetData<Settings>().runtime;
		}

	protected:
		size_t _index = max_value<size_t>;

	};

}
