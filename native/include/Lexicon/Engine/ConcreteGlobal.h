#pragma once

#include "Lexicon/Engine/InfoBase.h"
#include "Lexicon/Engine/GlobalBase.h"
#include "Lexicon/AbstractGlobal.h"


//*src
#include "Lexicon/Engine/Runtime.h"


namespace LEX
{
	class ConcreteGlobal : public GlobalBase, public AbstractGlobal
	{
		ISpecializable* GetSpecializable() override { return nullptr; }

		bool Revert(bool just_default) override
		{
			//*this = _declared.policy.
			if (just_default) {
				Variable& a_this = *this;
				
				auto type = Policy();

				if (Policy() == nullptr) {
					type = _declared.policy->GetTypePolicy((ITemplateBody*)nullptr);
					a_this.SetPolicy(type);
				}

				a_this = type->GetDefault();
			}
			else
			{


				Runtime runtime{ *_init };

				//Variable& a_this = *this;

				Assign(runtime.Run());

			}


			return true;
		}


	protected:
		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<AbstractGlobal>::value):
				return (AbstractGlobal*)this;

			case Hash(TypeName<ConcreteGlobal>::value):
				return this;
			}

			return __super::Cast(name);
		}
	};
}