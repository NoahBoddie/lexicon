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

		FieldType GetFieldType() const override { return FieldType::Local; }

		ITypePolicy* GetTypePolicy() const override { return _type; }

	protected:
		ITypePolicy* _type = nullptr;



		operator bool() const override
		{
			//Should this be and?
			return _type || __super::operator bool();
		}
	};

}
