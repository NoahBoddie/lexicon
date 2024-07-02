#pragma once

#include "InfoBase.h"

namespace LEX
{

	class TemporaryInfo : public InfoBase
	{
	public:
		//I don't think I need this, may merge into local info.
		TemporaryInfo(size_t i) : _index{ i } {}



		virtual Qualifier GetQualifiers() const override 
		{
			return GetData<Settings>().flags;
		}

	protected:
		size_t _index = max_value<size_t>;

	};

}
