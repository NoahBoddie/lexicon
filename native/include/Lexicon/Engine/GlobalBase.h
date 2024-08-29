#pragma once

#include "Lexicon/IGlobal.h"
#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/RoutineBase.h"
#include "Lexicon/Engine/Declaration.h"

namespace LEX
{
	struct ICallableUnit;
	struct RoutineBase;

	struct GlobalData_
	{
		std::string _name;

		//Variable stores this sure, but each special global will have a different variable, so I might as well use this
		// for interface
		Declaration _declared;

		//This is the default for a given global. Call to reset value. But, do note, this should be a formula, not a callable unit.
		//For this I may need something that can be specialized.
		
		
		//This is the initialization for the function. Should be tied to a function called reset that can be used on
		// globals.
		std::unique_ptr<RoutineBase> _init;

	};

	

	struct GlobalBase : public virtual IGlobal, public SecondaryElement, public GlobalData_
	{
	public:
		IGlobal* AsGlobal() override { return this; }


		//I may need a new type of linker for this, so the very most default value can be set for globals, then I can use them
		// in functions.
		//Actually, it would probably be something like external would be post definition. But on definition, simple values are filled.
		// to prevent error when trying to use them.

		void LoadFromRecord(Record& target) override;

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
			return _declared.flags;
		}

		virtual Specifier GetSpecifiers() const override
		{
			return _declared.declare | Specifier::Static;
		}

		virtual ITypePolicy* GetType() const
		{
			return _declared.policy;
		}




		
	};

}