#pragma once

#include "Lexicon/Engine/TypeBase.h"
#include "Lexicon/TypeInfo.h"

#include "Lexicon/Variable.h"

namespace LEX
{
	struct ITemplatePart;
	struct ITemplateBody;

	class ConcreteType : public ConcreteTypeBase
	{
	public:
		DECL_IMPL_FUNC_ENVIRONMENT;
		DEFINE_COMPONENT_TYPE(ComponentType::ConcreteType)
		


		using ConcreteTypeBase::ConcreteTypeBase;

		ISpecializable* GetSpecializable() override { return nullptr; }

		TypeID GetTypeID() const override
		{
			return TypeBase::GetTypeID();
		}
		
		std::string_view GetName() const override
		{
			return TypeBase::GetName();
		}

		DataType GetDataType() const override
		{
			return TypeBase::GetDataType();
		}
		

		//These 2 should largely be inherited from Type.
		ITypeInfo* CheckTypeInfo(ITemplatePart* args) override
		{
			return this;
		}

		TypeInfo* GetTypeInfo(ITemplateBody* args) override
		{
			return this;
		}

		bool IsResolved() const override { return true; }


		/*
		ConcreteType(std::string_view name, TypeOffset offset, TypeEnum enm, TypeInfo* ext = nullptr, Variable a_def = {})
		{
			//TODO:Multiple inheritence is a thing I wish to implement at a later point, for now ConcreteType takes one APolicy

			_id = IdentityManager::ClaimID(this, name, offset);
			_enum = enm;
			_extends = ext;

			_default = a_def;
			_default = Variable{ a_def, this };
		}
		//*/

		

		//Move this
		//virtual TypeEnum GetTypeEnum() = 0;


		TypeInfo* GetExtends() override;

		Variable GetDefault() const override;
		Variable GetVariable() const override;

		void OnAttach() override;

		void LoadFromSyntaxNode(SyntaxRecord& node) override;



	protected:
		
		const Component* GetComponent() const override final { return this; }
	public:

		void LoadFromRecord(SyntaxRecord& ast) override;


		void CompileExpression_DEPRECATED(SyntaxRecord& ast);


		LinkResult OnLink(LinkFlag flags) override;

        LinkFlag GetLinkFlags() override;


		//Technically, should be in abstract, but abstract actually is supposed to handle this so shrug.
		//virtual TypeInfo* AsTypeInfo() { return nullptr; }

		//When loaded and not trival, added to a map. Should have a function called SetTypeID and that should handle
		// it. Should have a class called TypeIDHandler handle it.


		//TODO: This needs to be able to convert to bool, as most types should be capable of such a feat.
		// but I'll need to make something for each type
		ConvertResult GetConvertTo(const ITypeInfo* other, const ITypeInfo* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const override
		{
			return __super::GetConvertTo(other, scope, out, flags);
		}
	};


	//using ConcreteType = ConcreteType;



	
}
