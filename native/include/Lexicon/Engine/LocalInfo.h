#pragma once

#include "InfoBase.h"

namespace LEX
{

	class LocalInfo : public InfoBase
	{
	public:
		constexpr LocalInfo() noexcept = default;

		LocalInfo(QualifiedType t, uint32_t i) : 
			InfoBase{ t.qualifiers, Specifier{}, i },
			_type {t.policy}
		{
		}

		FieldType GetFieldType() const override { return FieldType::Local; }

		ITypePolicy* GetType() const override { return _type; }

		Qualifier GetQualifiers() const override { return qualifiers; }
		Specifier GetSpecifiers() const override { return specifiers; }

		std::string GetFieldName() const override { return std::format("<local var {}>", _index); }



		operator bool() const override
		{
			//Should this be and?
			return _type;
		}

	protected:
		
		ITypePolicy* _type = nullptr;



	};
}
