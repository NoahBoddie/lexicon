#pragma once

#include "PolicyBase.h"
#include "AbstractTypePolicy.h"

namespace LEX
{

	class ConcretePolicy : public PolicyBase, public AbstractTypePolicy
	{
	public:
		using PolicyBase::PolicyBase;

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


		AbstractTypePolicy* GetExtends() override
		{
			//I think I could have this in 2 forms. One where you output to an array and one where you just get at a point.
			// I kinda don't really super want this though cause I'd have to define it twice, so maybe a const vector or something?
			// I think an out vector might be the best solution. Idk.

			return _extends->GetTypePolicy(nullptr);
		}

		Variable GetDefault() override
		{
			if (policy)
				return policy->CreateObject(GetTypeID());

			return _default;
		}

	protected:
		void SetDefault(Variable& var) override
		{
			_default = var;
		}
	public:

		void LoadFromRecord(Record& ast) override;


		void CompileExpression_DEPRECATED(Record& ast)
		{
			//Keeping this because I may have need of it in the future.
			for (auto& node : ast.GetChildren())
			{
				switch (ast.SYNTAX().type)
				{
				case SyntaxType::Function:
					AddFunction(Component::Create<Function>(node));
					break;


				case SyntaxType::Variable:
					//This is incorrect, this is in policy.
					AddVariable(Component::Create<Global>(node));
					break;


				default:
					report::compile::critical("unexpected syntax was invalid.");
				}
			}




			//tba
			//This expects a type of
		}


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
