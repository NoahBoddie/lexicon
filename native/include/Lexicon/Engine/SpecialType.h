#pragma once


#include "Lexicon/AbstractTypePolicy.h"
#include "Lexicon/Engine/SpecialBase.h"

//*src
#include "Lexicon/Engine/GenericType.h"
#include "Lexicon/Variable.h"

namespace LEX
{
	struct Variable;


	struct SpecialType : public AbstractTypePolicy, public SpecialBase
	{
		ITypePolicy* _self = nullptr;

		SpecialType(GenericBase* base, ITypePolicy* type, ITemplatePart* spec) : SpecialBase{ base, spec }, _self{ type }
		{
			report::info("NewBodyPart {}", (uintptr_t)this);
		}

		SpecialType(GenericType* type, ITemplatePart* spec) : SpecialBase{ type, spec }, _self{ type }
		{
			report::info("NewBodyPart {}", (uintptr_t)this);
		}

		//TODO: The below part about itself seems to maybe be possible to template away. If there's merit I'll do it. 
		// For now, boilerplate. If it's not interface do it later
		ITypePolicy* GetSelf()
		{
			return _self;
		}

		const ITypePolicy* GetSelf() const
		{
			return _self;
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

		virtual AbstractTypePolicy* GetExtends() { report::fault::critical("Not used"); }


		ITypePolicy* CheckTypePolicy(ITemplatePart* args) override
		{
			return dynamic_cast<ITypePolicy*>(ObtainSpecial(args));
		}

		AbstractTypePolicy* GetTypePolicy(ITemplateBody* args) override
		{
			auto special = ObtainSpecial(args)->ToResolved<AbstractTypePolicy>();

			if (!special) {
				//TODO: Scream maybe?
			}

			return special;
		}


	};

	/*
	struct SpecialTypePart_ : public ITypePolicy, public SpecialBase
	{
		ITypePolicy* _self = nullptr;

		SpecialTypePart_(GenericBase* base, ITypePolicy* type, ITemplatePart* spec) : SpecialBase{ base, spec }, _self{ type }
		{
			report::info("NewBody {}", (uintptr_t)this);
		}

		SpecialTypePart_(GenericType* type, ITemplatePart* spec) : SpecialBase{ type, spec }, _self{ type }
		{
			report::info("NewPart {}", (uintptr_t)this);
		}



		ITypePolicy* GetSelf()
		{
			return _self;
		}

		const ITypePolicy* GetSelf() const
		{
			return _self;
		}



		ITypePolicy* CheckTypePolicy(GenericBase* ask, ITemplatePart* args) override
		{
			return dynamic_cast<ITypePolicy*>(ObtainSpec(ask, args));
		}

		AbstractTypePolicy* GetTypePolicy(ITemplateBody* args) override
		{
			return dynamic_cast<AbstractTypePolicy*>(ObtainBody(args));
		}



		virtual TypeID GetTypeID() const { return GetSelf()->GetTypeID(); }

		virtual DataType GetDataType() const { return GetSelf()->GetDataType(); }



		virtual std::string_view GetName() const { return GetSelf()->GetName(); }


		HierarchyData* GetHierarchyData() const override
		{
			return GetSelf()->GetHierarchyData();
		}

	};
	//*/
}