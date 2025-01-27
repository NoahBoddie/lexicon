#pragma once


#include "Lexicon/AbstractTypePolicy.h"
#include "Lexicon/Engine/SpecialBody.h"

//*src
#include "Lexicon/Engine/GenericType.h"
#include "Lexicon/Variable.h"

namespace LEX
{
	struct Variable;


	struct SpecialTypeBody : public AbstractTypePolicy, public SpecialBody
	{
		ITypePolicy* _self = nullptr;

		SpecialTypeBody(GenericBase* base, ITypePolicy* type, ITemplateBody* spec) : SpecialBody{ base, spec }, _self{ type }
		{
			report::info("NewBody {}", (uintptr_t)this);
		}

		SpecialTypeBody(GenericType* type, ITemplateBody* spec) : SpecialBody{ type, spec }, _self{ type }
		{
			report::info("NewBody {}", (uintptr_t)this);
		}

		ITypePolicy* GetSelf()
		{
			return _self;
		}

		const ITypePolicy* GetSelf() const
		{
			return _self;
		}

		AbstractTypePolicy* GetTypePolicy(ITemplateBody* args) override
		{
			return this;
		}



		virtual TypeID GetTypeID() const { return GetSelf()->GetTypeID(); }

		virtual DataType GetDataType() const { return GetSelf()->GetDataType(); }



		virtual std::string_view GetName() const { return GetSelf()->GetName(); }



		HierarchyData* GetHierarchyData() const override
		{
			return GetSelf()->GetHierarchyData();
		}


		virtual Variable GetDefault() { throw nullptr; }


		virtual void SetDefault(Variable&) { throw nullptr; }

		virtual AbstractTypePolicy* GetExtends() { throw nullptr; }

	};

}