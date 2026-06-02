#include "Lexicon/Engine/TypeBase.h"

#include "Lexicon/Engine/ConcreteType.h"
#include "Lexicon/Interfaces/IdentityManager.h"

#include "Lexicon/Engine/Declaration.h"
#include "Lexicon/Engine/parse_strings.h"
#include "Lexicon/Interfaces/ObjectPolicyManager.h"

#include "Lexicon/MergeTemplate.h"

namespace LEX
{

	size_t CheckOverload2(OverloadArgument& input, std::vector<OverloadInfo*> clauses, Overload& ret)
	{
		Overload* last = nullptr;

		size_t result = -1;

		for (auto i = 0; i < clauses.size(); i++)
		{
			auto clause = clauses[i];

			Overload buffer;

			auto bias = input.Match(clauses[i], nullptr, buffer, last);

			switch (bias)
			{
			case OverloadBias::kAmbiguous:
				result = -1;
				break;

			case OverloadBias::kCurrent:
				ret = std::move(buffer);
				last = &ret;
				result = i;
				break;
			}
		}

		//if (last) {
		//	last->param->ResolveOverload()
		//}

		//if (last)
		//	ret = *last;//this should move

		//return last ? result : -1;
		return result;
	}


	TypeBase::TypeBase()
	{
		IdentityManager::instance->ObtainID(this);
	}

	TypeBase::TypeBase(uint32_t i)
	{
		//SetTypeID(0); return;

		IdentityManager::instance->ClaimID(this, i);
	}

	TypeBase::TypeBase(std::string_view name, TypeOffset off)
	{
		policy = ObjectPolicyManager::instance->GetObjectPolicyFromName(name);
		IdentityManager::instance->ClaimID(this, name, off);
	}


	void TypeBase::CheckDeriveFrom(IHierarchyTree* tree)
	{
		auto other = tree->GetHierarchyType();

		if (!other) {
			return;
		}

		auto l_type = GetDataType();
		auto r_type = other->GetDataType();

		get_switch (GetDataType())
		{
		default:
		case DataType::Invalid:
			report::compile::error("{} has an {} data type", GetName(), magic_enum::enum_name(switch_value));
			break;

		case DataType::Class:
		case DataType::Struct:
		case DataType::Attribute:
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
			//report::fault::debug("Type {} has a missing syntax tree", GetName());
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
				for (SyntaxRecord& inherit : derives->children()) {

					Environment* env = NULL_OP(NULL_Q(GetParent())->GetEnvironment());

					ITypeInfo* type = SearchTypePath(env, inherit).info;

					Access access = Access::None;

					if (auto decl = inherit.FindChild(parse_strings::declare_specifier); decl) {
						auto buff = GetSpecifiersFromStrings(*decl);
						access = buff.access;
					}

					if (access == Access::None) {
						access = Access::Private;
					}

					if (!type)  //I'd actually rather report.
						inherit.error("Could not generate type from {}", inherit.GetTag());

					SetDerivesTo(type->GetHierarchyTree(), access);
				}


			}
		}
		FinalizeAndSort();

		PrintInheritance();

	}


	IFunction* TypeBase::FindConstructor(OverloadArgument& key, Overload& out)
	{
		std::vector<OverloadInfo*> functions = FindFunctions(parse_strings::constructor);

		if (functions.empty() == false)
		{
			if (auto index = CheckOverload2(key, { functions.begin(), functions.end() }, out); index != -1)
			{
				if (auto info = static_cast<OverloadInfo*>(out.param); info->IsOverloadUsuable() == true) {

					if (auto function = dynamic_cast<IFunction*>(out.param))
					{
						auto self = As<ITypeInfo>();


						MergeTemplate merger{ self->GetTemplatePart(), out };
						//TODO: in the future system, the merger will no longer be necessary. Instead, that will be handled by the parentage system.

						return function->CheckFunction(merger);
					}
				}
			}
		}


		return nullptr;
	}
}