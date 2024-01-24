#pragma once

#include "TemporaryInfo.h"

namespace LEX
{
	class LocalInfo : public TemporaryInfo
	{
	public:
		LocalInfo(ITypePolicy* t, size_t i) : TemporaryInfo{ i }, _type{ t }
		{

		}

	protected:
		ITypePolicy* _type = nullptr;


		operator bool() const override
		{
			//Should this be and?
			return _type || __super::operator bool();
		}
	};

}
