#pragma once


#include "Lexicon/Type.h"
#include "Lexicon/Engine/SpecialBase.h"

//*src
#include "Lexicon/Engine/GenericType.h"
#include "Lexicon/Variable.h"

namespace LEX
{
	struct Variable;


	struct SpecialType : public Type, public SpecialBase
	{
		AbstractType* _self = nullptr;

		SpecialType(GenericBase* base, AbstractType* type, ITemplatePart* spec) : SpecialBase{ base, spec }, _self{ type }
		{
			report::info("NewBodyPart {}", (uintptr_t)this);
		}

		SpecialType(GenericType* type, ITemplatePart* spec) : SpecialBase{ type, spec }, _self{ type }
		{
			report::info("NewBodyPart {}", (uintptr_t)this);
		}

		//TODO: The below part about itself seems to maybe be possible to template away. If there's merit I'll do it. 
		// For now, boilerplate. If it's not interface do it later
		AbstractType* GetSelf()
		{
			return _self;
		}

		const AbstractType* GetSelf() const
		{
			return _self;
		}

		ISpecializable* GetSpecializable() override { return GetGeneric(); }

		bool IsResolved() const override
		{
			return SpecialBase::IsResolved();
		}


		virtual TypeID GetTypeID() const { return GetSelf()->GetTypeID(); }

		virtual DataType GetDataType() const { return GetSelf()->GetDataType(); }



		virtual std::string_view GetName() const { return GetSelf()->GetName(); }



		HierarchyData* GetHierarchyData() const override
		{
			return GetSelf()->GetHierarchyData();
		}


		virtual Variable GetDefault() { report::fault::critical("Not used"); }


		virtual void SetDefault(Variable&) { report::fault::critical("Not used"); }

		virtual Type* GetExtends() { report::fault::critical("Not used"); }


		AbstractType* CheckTypePolicy(ITemplatePart* args) override
		{
			return dynamic_cast<AbstractType*>(ObtainSpecial(args));
		}

		Type* GetTypePolicy(ITemplateBody* args) override
		{
			auto special = ObtainSpecial(args)->ToResolved<Type>();

			if (!special) {
				//TODO: Scream maybe?
			}

			return special;
		}


	};
}