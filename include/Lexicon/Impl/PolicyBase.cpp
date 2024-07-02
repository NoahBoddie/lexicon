#include "PolicyBase.h"

#include "ConcretePolicy.h"
#include "IdentityManager.h"

#include "Declaration.h"
#include "parse_strings.h"

namespace LEX
{



	PolicyBase::PolicyBase()
	{
		IdentityManager::instance->ObtainID(this);
	}

	PolicyBase::PolicyBase(uint32_t i)
	{
		//SetTypeID(0); return;

		IdentityManager::instance->ClaimID(this, i);
	}

	PolicyBase::PolicyBase(std::string_view name, TypeOffset offset)
	{
		//SetTypeID(0); return;
		IdentityManager::instance->ClaimID(this, name, offset);
	}

	void PolicyBase::HandleInheritance()
	{
		if (IsInheritHandled() == true)
			return;

		auto ast = GetSyntaxTree();
		
		if (!ast) {
			report::compile::critical("Type has a missing syntax tree");
		}

		Record* settings = ast->FindChild(parse_strings::settings);

		if (!settings) {
			report::compile::critical("setting not found in type policy record");
		}

		//Doing this early makes circular inheritance not crash things, the inheriting will inheritant 
		MarkInheritHandled();//Hopefully at least.


		//This should be handled after declaration.
		//static_assert(false);
		if (auto derives = settings->FindChild(parse_strings::derives)) {
			for (auto& inherit : derives->GetChildren()) {
				PolicyBase* type = GetParent()->FetchEnvironment()->SearchTypePath(inherit);

				Access access = Access::None;

				if (auto decl = inherit.FindChild(parse_strings::declare_specifier); decl) {
					auto buff = GetSpecifiersFromStrings(*decl);
					access = Misc::DeclareToAccess(buff);
				}

				if (access == Access::None) {
					access = Access::Private;
				}

				if (!type)  //I'd actually rather report.
					throw nullptr;

				SetDerivesTo(type, access);
			}

			FinalizeAndSort();

			PrintInheritance();


		}

	}
}