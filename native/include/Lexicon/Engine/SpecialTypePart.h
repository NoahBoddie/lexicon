#pragma once

#include "Lexicon/ITypePolicy.h"
#include "Lexicon/Engine/SpecialPart.h"

//*src
#include "Lexicon/Engine/GenericType.h"

namespace LEX
{
	struct SpecialTypePart : public ITypePolicy, public SpecialPart
	{
		ITypePolicy* _self = nullptr;

		SpecialTypePart(GenericBase* base, ITypePolicy* type, ITemplatePart* spec) : SpecialPart{ base, spec }, _self{ type }
		{
			report::info("NewBody {}", (uintptr_t)this);
		}

		SpecialTypePart(GenericType* type, ITemplatePart* spec) : SpecialPart{ type, spec }, _self{ type }
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

}