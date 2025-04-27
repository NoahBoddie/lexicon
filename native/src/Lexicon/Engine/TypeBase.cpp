#include "Lexicon/Engine/TypeBase.h"

#include "Lexicon/Engine/ConcreteType.h"
#include "Lexicon/Interfaces/IdentityManager.h"

#include "Lexicon/Engine/Declaration.h"
#include "Lexicon/Engine/parse_strings.h"

namespace LEX
{



	TypeBase::TypeBase()
	{
		IdentityManager::instance->ObtainID(this);
	}

	TypeBase::TypeBase(uint32_t i)
	{
		//SetTypeID(0); return;

		IdentityManager::instance->ClaimID(this, i);
	}

	TypeBase::TypeBase(std::string_view name, TypeOffset offset)
	{
		//SetTypeID(0); return;
		IdentityManager::instance->ClaimID(this, name, offset);
	}


	void TypeBase::CheckDeriveFrom(ITypeInfo* other)
	{
		auto l_type = GetDataType();
		auto r_type = other->GetDataType();

		get_switch (GetDataType())
		{
		case DataType::Invalid:
			report::compile::error("{} has an invalid data type", GetName());

		case DataType::Class:
		case DataType::Struct:
			if (r_type == DataType::Interface) {
				return;
			}
			[[fallthrough]];
		case DataType::Interface:
			if (l_type == r_type){
				return;
			}
			break;
		}

		report::error("{} as a {} cannot derive from {} a {}.",
			GetName(), magic_enum::enum_name(l_type),
			other->GetName(), magic_enum::enum_name(r_type));
	}

	void TypeBase::HandleInheritance()
	{
		if (IsInheritHandled() == true)
			return;

		auto ast = GetSyntaxTree();
		
		if (!ast) {
			report::fault::debug("Type {} has a missing syntax tree", GetName());
			MarkInheritHandled();
		}
		else {
			SyntaxRecord* settings = ast->FindChild(parse_strings::settings);

			if (!settings) {
				report::fault::critical("setting not found in type policy record");
			}

			//Doing this early makes circular inheritance not crash things, the inheriting will inheritant 
			MarkInheritHandled();//Hopefully at least.


			//This should be handled after declaration.
			//static_assert(false);
			if (auto derives = settings->FindChild(parse_strings::derives)) {
				for (auto& inherit : derives->children()) {
					TypeBase* type = GetParent()->FetchEnvironment()->SearchTypePath(inherit);

					Access access = Access::None;

					if (auto decl = inherit.FindChild(parse_strings::declare_specifier); decl) {
						auto buff = GetSpecifiersFromStrings(*decl);
						access = buff.access;
					}

					if (access == Access::None) {
						access = Access::Private;
					}

					if (!type)  //I'd actually rather report.
						report::critical("no type, I dont actually know where I am right now, please figure out what this means later thx");

					SetDerivesTo(type->AsAbstract(), access);
				}


			}
		}
		FinalizeAndSort();

		PrintInheritance();

	}
}