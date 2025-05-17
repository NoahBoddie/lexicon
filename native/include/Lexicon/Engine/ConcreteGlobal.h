#pragma once

#include "Lexicon/Engine/InfoBase.h"
#include "Lexicon/Engine/GlobalBase.h"
#include "Lexicon/Global.h"


//*src
#include "Lexicon/Engine/Runtime.h"


namespace LEX
{
	class ConcreteGlobal : public ConcreteGlobalBase
	{
		RuntimeVariable value{ std::in_place_type_t<Variable>{} };

		ISpecializable* GetSpecializable() override { return nullptr; }

		bool IsResolved() const override final { return true; }

		Global* GetGlobal(ITemplateBody* args) override { return this; }

		IGlobal* CheckGlobal(ITemplatePart* args) override { return this; }

		RuntimeVariable GetValue() override { return value.Ref(); }
		RuntimeVariable GetReference() override { return value.AsRef(); }

		
		bool Set(const RuntimeVariable& value) override
		{
			//currently won't be doing anything with this
			return false;
		}

		bool Revert(bool just_default) override
		{
			if (just_default) {
				Variable& a_this = value.Ref();//this shouldn't work
				
				//auto type = a_this.Policy();
				auto type = _declared->GetTypePolicy(nullptr);

				a_this = type->GetDefault();
			}
			else
			{


				Runtime runtime{ *_init };

				//Variable& a_this = *this;

				value->Assign(runtime.Run());

			}


			return true;
		}


	protected:
		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<Global>::value):
				return static_cast<Global*>(this);

			case Hash(TypeName<ConcreteGlobal>::value):
				return this;
			}

			return __super::Cast(name);
		}
	};
}