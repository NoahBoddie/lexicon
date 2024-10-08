#pragma once

#include "Lexicon/Engine/PolicyBase.h"
#include "Lexicon/AbstractTypePolicy.h"

#include "Lexicon/Variable.h"

namespace LEX
{

	class ConcretePolicy : public PolicyBase, public AbstractTypePolicy
	{
	public:

		ConcretePolicy();

		ConcretePolicy(uint32_t i);

		ConcretePolicy(std::string_view name, TypeOffset offset);


		/*
		ConcretePolicy(std::string_view name, TypeOffset offset, TypeEnum enm, AbstractTypePolicy* ext = nullptr, Variable a_def = {})
		{
			//TODO:Multiple inheritence is a thing I wish to implement at a later point, for now ConcretePolicy takes one APolicy

			_id = IdentityManager::ClaimID(this, name, offset);
			_enum = enm;
			_extends = ext;

			_default = a_def;
			_default = Variable{ a_def, this };
		}
		//*/

		

		//Move this
		//virtual TypeEnum GetTypeEnum() = 0;


		AbstractTypePolicy* GetExtends() override;

		Variable GetDefault() override;

		void OnAttach() override;


		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<AbstractTypePolicy>::value):
				return (AbstractTypePolicy*)this;

			case Hash(TypeName<ConcretePolicy>::value):
				return this;
			}

			return __super::Cast(name);
		}

	protected:
		void SetDefault(Variable& var) override;
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
		ConvertResult IsConvertibleTo(const ITypePolicy* other, const ITypePolicy* scope, Conversion* out = nullptr, ConversionType type = ConversionType::Implicit) const override
		{
			return __super::IsConvertibleTo(other, scope, out, type);
		}


	private:
		//TODO: unique_ptr this bum. It takes more space than it's owed.
		Variable _default;

	};


	//using ConcretePolicy = ConcretePolicy;



	
}
