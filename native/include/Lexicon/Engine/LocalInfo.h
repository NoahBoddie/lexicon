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

		ITypeInfo* GetType() const override { return _type; }

		Qualifier GetQualifiers() const override { return qualifiers; }
		Specifier GetSpecifiers() const override { return specifiers; }

		std::string GetFieldName() const override { return std::format("<local var {}>", _index); }


		void MutateReference(Refness ref)
		{
			if (qualifiers.reference == Refness::Generic)
			{
				switch (ref)
				{
				case Refness::Static:
					qualifiers.reference = Refness::Global;
					break;
				

				case Refness::Global:
				case Refness::Local:
				case Refness::Scoped:
					qualifiers.reference = ref;
					break;
					//return true;

				default:
					report::error("cant do this '{}' not recognized", magic_enum::enum_name(ref));
					qualifiers.reference = Refness::Local;
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
		
		ITypeInfo* _type = nullptr;



	};
}
