#pragma once


#include "Lexicon/TypeInfo.h"
#include "Lexicon/Engine/SpecialBase.h"



//*src
#include "Lexicon/Engine/GenericType.h"
#include "Lexicon/Variable.h"

namespace LEX
{
	struct Variable;


	//TODO: SpecialType should derive from IHierarchyTree

	struct SpecialType : public TypeInstance<TypeInfo>, public SpecialBase
	{
		DEFINE_COMPONENT_TYPE(ComponentType::SpecialType)
	public:



		ITypeInfo* _self = nullptr;
		


		//SpecialType(GenericBase* base, ITypeInfo* type, ITemplatePart* spec) : SpecialBase{ base, spec }, _self{ type }
		//{
		//	report::info("NewBodyPart {}", (uintptr_t)this);
		//}

		SpecialType(GenericType* type, ITemplatePart* spec) : SpecialBase{ type, spec }, _self{ type }
		{
			report::info("NewBodyPart {}", (uintptr_t)this);
		}


		std::vector<TemplateType*> GetTemplateInputs() override
		{
			if (GetState() == State::kBody) {
				return {};
			}
			
			auto ts = types();
			
			std::vector<TemplateType*> result{};

			result.reserve(ts.size());

			for (auto type : types()){
				result.append_range(type->GetTemplateInputs());
			}
			
			return result;
		}

		//TODO: The below part about itself seems to maybe be possible to template away. If there's merit I'll do it. 
		// For now, boilerplate. If it's not interface do it later
		ITypeInfo* GetSelf()
		{
			return _self;
		}

		const ITypeInfo* GetSelf() const
		{
			return _self;
		}

		ISpecializable* GetSpecializable() override { return this; }

		bool IsResolved() const override
		{
			return SpecialBase::IsResolved();
		}


		virtual TypeID GetTypeID() const { return GetSelf()->GetTypeID(); }

		DataType GetDataType() const override { return GetSelf()->GetDataType(); }



		std::string_view GetName() const override { return GetSelf()->GetName(); }


		
		IHierarchyTree* GetHierarchyTree() const override
		{
			//TODO: this is completely insufficent, this needs an IHierarchyTree
			return _self->GetHierarchyTree();
		}


		Variable GetDefault() const override { report::fault::critical("Not used"); }
		Variable GetVariable() const override { report::fault::critical("Not used"); }

		

		virtual TypeInfo* GetExtends() { report::fault::critical("Not used"); }


		void SetSelfQualifiers(Qualifier& qualifiers) const override { return _self->SetSelfQualifiers(qualifiers); }

		ITypeInfo* CheckTypeInfo(ITemplatePart* args) override
		{
			return dynamic_cast<ITypeInfo*>(ObtainSpecial(args));
		}

		TypeInfo* GetTypeInfo(ITemplateBody* args) override
		{
			auto special = ObtainSpecial(args)->ToResolved<TypeInfo>();

			if (!special) {
				//TODO: Scream maybe?
			}

			return special;
		}


		ObjectPolicy* GetObjectPolicy() const
		{
			return nullptr;
		}

		ITemplatePart* GetTemplatePart() override
		{
			//This exists so someone can get the part of a generic type, either before or after specializing.
			return this;
		}

		private:
			const Component* GetComponent() const override final 
			{ 
				auto generic = GetGeneric();

				return generic ? generic->GetComponent() : nullptr;
			}
	};
}