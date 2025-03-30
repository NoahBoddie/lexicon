#pragma once

#include "InfoBase.h"

namespace LEX
{

	class LocalInfo : public InfoBase
	{
	public:
		constexpr LocalInfo() noexcept = default;

		LocalInfo(QualifiedType t, uint32_t i) : 
			InfoBase{ t, Specifier{}, i },
			_type {t.policy}
		{
		}

		FieldType GetFieldType() const override { return FieldType::Local; }

		AbstractType* GetType() const override { return _type; }

		Qualifier GetQualifiers() const override { return qualifiers; }
		Specifier GetSpecifiers() const override { return specifiers; }

		std::string GetFieldName() const override { return std::format("<local var {}>", _index); }


		void MutateReference(Reference ref)
		{
			if (qualifiers.reference == Reference::Generic)
			{
				switch (ref)
				{
				case Reference::Static:
					qualifiers.reference = Reference::Global;
					break;
				

				case Reference::Global:
				case Reference::Local:
				case Reference::Scoped:
					qualifiers.reference = ref;
					break;
					//return true;

				default:
					report::error("cant do this '{}' not recognized", magic_enum::enum_name(ref));
					qualifiers.reference = Reference::Local;
					break;
					//return false;
				}
			}

			//return 
		}

		operator bool() const override
		{
			//Should this be and?
			return _type;
		}

	protected:
		
		AbstractType* _type = nullptr;



	};
}
