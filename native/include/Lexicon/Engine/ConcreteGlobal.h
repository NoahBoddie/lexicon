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
	public:
		DEFINE_COMPONENT_TYPE(ComponentType::ConcreteGlobal)


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
				
				assert(_declared);
				//auto type = a_this.Policy();
				auto type = _declared->GetTypeInfo(nullptr);
				
				assert(type);
				
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

	private:
		const Component* AsComponent() const override final { return this; }

	};
}