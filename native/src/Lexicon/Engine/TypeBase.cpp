#include "Lexicon/Engine/TypeBase.h"

#include "Lexicon/Engine/ConcreteType.h"
#include "Lexicon/Interfaces/IdentityManager.h"

#include "Lexicon/Engine/Declaration.h"
#include "Lexicon/Engine/parse_strings.h"
#include "Lexicon/Interfaces/ObjectPolicyManager.h"

#include "Lexicon/MergeTemplate.h"
#include "Lexicon/Interfaces/AttributeManager.h"
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


	void TypeBase::CheckDeriveFrom(IHierarchyTree* tree, SyntaxRecord& record)
	{
		auto other = tree->GetHierarchyType();

		if (!other) {
			return;
		}

		auto l_type = GetDataType();
		auto r_type = other->GetDataType();

		bool other_is_interface = r_type == DataType::Interface;

		get_switch (GetDataType())
		{
		default:
		case DataType::Invalid:
			record.critical<IssueType::Fault>("{} has an {} data type", GetName(), magic_enum::enum_name(switch_value));
			break;

		case DataType::Class:
		case DataType::Struct:
		case DataType::Attribute:
			if (r_type == DataType::Interface) {
				OnInherit(other, record);
				return;
			}
			[[fallthrough]];
		case DataType::Interface:
			if (l_type == r_type){
				OnInherit(other, record);
				return;
			}
			break;
		}

		record.error<IssueType::Compile>("{} as a {} cannot derive from {} a {}.",
			GetName(), magic_enum::enum_name(l_type),
			other->GetName(), magic_enum::enum_name(r_type));
	}

	void TypeBase::OnInherit(ITypeInfo* other, SyntaxRecord& record)
	{
		auto base = other->GetAs<TypeBase>();

		if (base && base->GetDataType() == DataType::Attribute) {
			if (attrBuilder && attrBuilder != base->attrBuilder) {
				record.error<IssueType::Compile>("Type '{}' already has a linked builder", GetName());
			}

			attrBuilder = base->attrBuilder;
		}
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

			if (GetDataType() == DataType::Attribute) {
				if (auto attach = settings->FindChild(parse_strings::attribute_data); attach)
				{

					if (attach->size() == 0) {
						attach->critical("external type requires some type.");
					}

					bool required = false;

					SyntaxRecord& attach_data = attach->GetFront();
					switch (Hash(attach_data.GetTag()))
					{
					case "intrinsic"_h:
						required = true;
						__fallthrough;
					case "external"_h:
						if  constexpr (1)
						{
							//Handle error, I can't fucking be bothered.
							std::string name = GetFullName();
							
							//I wish to make the return of this optional. True for success, false for failure, nullopt for defered
							std::optional<bool> success = AttributeManager::instance->RequestNativeData(name, this);

							assert_if(!success.value_or(false) && required) {
								attach->critical<IssueType::Compile>("intrinsic native data not found '{}'.", name);
							}
							
							report::trace("request for native data success: {}", success.has_value() ? std::to_string(success.value()) : "defered");
						}
						break;
					}

				}
			}



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

					SetDerivesTo(type->GetHierarchyTree(), inherit, access);
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