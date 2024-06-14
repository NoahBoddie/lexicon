#pragma once

#include "PolicyBase.h"
#include "AbstractTypePolicy.h"

#include "Variable.h"

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

		bool IsAbstract() const override { return false; }

		//Move this
		//virtual TypeEnum GetTypeEnum() = 0;


		AbstractTypePolicy* GetExtends() override;

		Variable GetDefault() override;

		void OnAttach() override;

	protected:
		void SetDefault(Variable& var) override;
	public:

		void LoadFromRecord(Record& ast) override;


		void CompileExpression_DEPRECATED(Record& ast);


		LinkResult OnLink(LinkFlag flags) override;

        LinkFlag GetLinkFlags() override;


		//Technically, should be in abstract, but abstract actually is supposed to handle this so shrug.
		//virtual TypeInfo* AsTypeInfo() { return nullptr; }

		//When loaded and not trival, added to a map. Should have a function called SetTypeID and that should handle
		// it. Should have a class called TypeIDHandler handle it.

	private:
		Variable _default;

	};


	//using ConcretePolicy = ConcretePolicy;



	
}
