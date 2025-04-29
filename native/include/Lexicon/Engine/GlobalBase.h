#pragma once

#include "Lexicon/Global.h"
#include "Lexicon/Engine/Field.h"
#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/GlobalData.h"

namespace LEX
{
	struct ICallableUnit;


	
	struct GlobalBase : public SecondaryElement, public GlobalData, public Field
	{
	public:


		virtual IGlobal* AsGlobal() = 0;
		virtual const IGlobal* AsGlobal() const = 0;

		virtual std::string_view GetName() const = 0;

		//Attempts to revert value. If the global is const or a special part, it will fail to revert.
		// If reverted just with default it will create the default value, if not, it will attempt to use a routine to set
		// information. Will not throw.
		virtual bool Revert(bool just_default) = 0;

		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
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



		virtual std::string GetFieldName() const override
		{
			return _name;
		}

		virtual FieldType GetFieldType() const override
		{
			return FieldType::Global;
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



	//For what it's worth, I really fucking loathe this system all together.
	template <typename T>
	struct PivotGlobalBase : public GlobalBase, public T
	{
		using GlobalBase::GlobalBase;

		IGlobal* AsGlobal() override { return this; }
		const IGlobal* AsGlobal() const override { return this; }



		std::string_view GetName() const override
		{
			return _name;
		}


		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<IGlobal>::value):
				return static_cast<IGlobal*>(this);
			}

			return __super::Cast(name);
		}
	};

	using GenericGlobalBase = PivotGlobalBase<IGlobal>;
	using ConcreteGlobalBase = PivotGlobalBase<Global>;

}