#pragma once

#include "Lexicon/IGlobal.h"
#include "Lexicon/Engine/Field.h"
#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/GlobalData.h"

namespace LEX
{
	struct ICallableUnit;


	
	struct GlobalBase : public virtual IGlobal, public SecondaryElement, public GlobalData, public Field
	{
	public:

		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<IGlobal>::value):
				return static_cast<IGlobal*>(this);

			case Hash(TypeName<GlobalBase>::value):
				return this;
			}

			return nullptr;
		}


		//I may need a new type of linker for this, so the very most default value can be set for globals, then I can use them
		// in functions.
		//Actually, it would probably be something like external would be post definition. But on definition, simple values are filled.
		// to prevent error when trying to use them.

		void LoadFromRecord(SyntaxRecord& target) override;

		void OnAttach() override;

		virtual LinkResult OnLink(LinkFlag flags) override;

		virtual LinkFlag GetLinkFlags() override;



		std::string_view GetName() const override
		{
			return _name;
		}

		virtual std::string GetFieldName() const override
		{
			return _name;
		}

		virtual FieldType GetFieldType() const override
		{
			return FieldType::Variable;
		}

		virtual uint32_t GetFieldIndex() const override
		{
			return -1;
		}


		virtual Qualifier GetQualifiers() const override
		{
			return _declared;
		}

		virtual Specifier GetSpecifiers() const override
		{
			return _declared.CopyWithFlags(SpecifierFlag::Static);
		}

		virtual ITypeInfo* GetType() const
		{
			return _declared.policy;
		}




		
	};

}