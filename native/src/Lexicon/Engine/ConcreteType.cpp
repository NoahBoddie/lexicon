#pragma once

#include "Lexicon/Engine/ConcreteType.h"

#include "Lexicon/Engine/Expression.h"

#include "Lexicon/Interfaces/ObjectPolicyManager.h"
#include "Lexicon/Engine/ConcreteFunction.h"

#include "Lexicon/Interfaces/IdentityManager.h"

#include "Lexicon/Engine/Declaration.h"

#include "Lexicon/Engine/parse_strings.h"
#include "Lexicon/Engine/ConcreteGlobal.h"
#include "Lexicon/Engine/GlobalVariable.h"

#include "Lexicon/Engine/Project.h"
#include "Lexicon/Interfaces/ProjectClient.h"

namespace LEX
{

	//TODO: REcordToInt exists in multiple places. A lot even. Please, centralize them.
	TypeOffset _RecordToInt(SyntaxRecord& ast)
	{//helps with generic or concrete divide
		std::string tag = ast.GetTag();

		if (std::strncmp(tag.c_str(), "0x", 2) == 0 || std::strncmp(tag.c_str(), "0X", 2) == 0)
		{
			return std::stoi(tag, nullptr, 16);
		}
		else
		{
			return std::stoi(tag, nullptr, 10);
		}
	}
	
	ConcreteType::ConcreteType() : ConcreteTypeBase{} {}

	ConcreteType::ConcreteType(uint32_t i) : ConcreteTypeBase{ i } {}

	ConcreteType::ConcreteType(std::string_view name, TypeOffset offset) : ConcreteTypeBase{ name, offset } {}
	/*
	ConcreteType::ConcreteType()
	{
		IdentityManager::instance->ObtainID(this);
	}

	ConcreteType::ConcreteType(uint32_t i)
	{
		//SetTypeID(0); return;

		IdentityManager::instance->ClaimID(this, i);
	}

	ConcreteType::ConcreteType(std::string_view name, TypeOffset offset)
	{
		//SetTypeID(0); return;
		IdentityManager::instance->ClaimID(this, name, offset);
	}
	//*/



	TypeInfo* ConcreteType::GetExtends()
	{
		//I think I could have this in 2 forms. One where you output to an array and one where you just get at a point.
		// I kinda don't really super want this though cause I'd have to define it twice, so maybe a const vector or something?
		// I think an out vector might be the best solution. Idk.

		return _extends->GetTypePolicy((ITemplateBody*)nullptr);
	}

	Variable ConcreteType::GetDefault()
	{
		if (policy)
			return policy->CreateObject(GetTypeID());

		return _default;
	}
	void ConcreteType::SetDefault(Variable& var)
	{
		_default = var;
	}


	void ConcreteType::LoadFromRecord(SyntaxRecord& ast)
	{
		_name = ast.GetTag();

		SyntaxRecord* settings = ast.FindChild(parse_strings::settings);

		if (!settings) {
			report::compile::critical("setting not found in type policy record");
		}

		switch (Hash(settings->FindChild(parse_strings::data_type)->GetFront().GetTag()))
		{
		case "class"_h:			_dataType = DataType::Class; break;//Makes a given object act like a class in assignment.
		case "struct"_h:		_dataType = DataType::Struct; break;
		case "interface"_h:		_dataType = DataType::Interface; break;
		//case "enum"_h:		_dataType = DataType::Interface; break;
		default:				report::compile::critical("improper data type found."); break;
		}

		//Do something with generic. It's useless right now


		if (auto test = settings->FindChild(parse_strings::attach); test)
		{
			SyntaxRecord& attach = test->GetFront();
			//I actually will allow interfaces to be external. It prevents them from being instantiable though.

			
			switch (Hash(attach.GetTag())) 
			{
				//case "data"_h://Not allowed yet
			case "external"_h:
				{
					//Should clash with intrinsic.
					MarkLinkExtern(); 

					break;
					if (attach.size() == 0) {
						report::compile::critical("external type requires some type.");
					}

					//unique_type.size()

					//save this shit til after linkage.
					//ObjectPolicy* ObjectPolicyManager::GetObjectPolicyFromName(obj_type.GetTag());

					SyntaxRecord& cat_name = attach.GetFront();

					category = cat_name.GetTag();

					//category name is completely optional.
					offset = cat_name.size() ? _RecordToInt(cat_name.GetFront()) : 0;

					logger::info("searching {}, our location {}", category, offset);

					//this shouldn't be done yet, but I really just want to send this shit.
					policy = ObjectPolicyManager::instance->GetObjectPolicyFromName(category);
				}
				break;

			case "intrinsic"_h:
				//if (_dataType == DataType::Interface) {
				//	report::compile::critical("interfaces cannot be intrinsic/external");
				//}
				break;

			default:
				report::compile::critical("PLACEHOLDER don't know how to handle unique type.");
				break;
			}

		}


		
	}

	void ConcreteType::OnAttach()
	{
		HandleInheritance();
	}

	void ConcreteType::CompileExpression_DEPRECATED(SyntaxRecord& ast)
	{
		//Keeping this because I may have need of it in the future.
		for (auto& node : ast.children())
		{
			switch (ast.SYNTAX().type)
			{
			case SyntaxType::Function:
				AddFunction(Component::Create<ConcreteFunction>(node));
				break;


			case SyntaxType::Variable:
				//This is incorrect, this is in policy.
				AddVariable(Component::Create<ConcreteGlobal>(node));
				break;


			default:
				report::compile::critical("unexpected syntax was invalid.");
			}
		}




		//tba
		//This expects a type of
	}



    LinkResult ConcreteType::OnLink(LinkFlag flags)
    {
		SyntaxRecord& ast = *GetSyntaxTree();
		
		switch (flags)
		{
		case LinkFlag::Declaration:
		{
			HandleInheritance();
			break;
		}


		case LinkFlag::External:
		{
			SyntaxRecord& attach = ast.FindChild(parse_strings::settings)->FindChild(parse_strings::attach)->GetFront();

			if (attach.size() == 0) {
				report::compile::critical("external type requires some type.");
			}

			//unique_type.size()

			//save this shit til after linkage.
			//ObjectPolicy* ObjectPolicyManager::GetObjectPolicyFromName(obj_type.GetTag());

			SyntaxRecord& cat_name = attach.GetFront();

			category = cat_name.GetTag();


			if (cat_name.size())
			{
				//this should more be if it's not number.
				if (auto& args = cat_name.GetFront(); args.GetView() == "args")
				{
					auto& children = args.children();

					std::vector<std::string_view> string_args{ children.size() };

					std::transform(children.begin(), children.end(), string_args.begin(), [](SyntaxRecord& it) { return it.GetView(); });

					offset = GetProject()->client()->GetOffsetFromArgs(cat_name.GetView(), string_args.data(), string_args.size());
					logger::info("offset from args = {}", offset);
				}
				else
				{
					offset = _RecordToInt(args);
				}


			}
			else
			{
				offset = 0;
			}
			//category name is completely optional.
			//offset = cat_name.size() ? _RecordToInt(cat_name.GetFront()) : 0;

			logger::info("searching {}, our location {}", category, offset);

			//this shouldn't be done yet, but I really just want to send this shit.
			policy = ObjectPolicyManager::instance->GetObjectPolicyFromName(category);

			break;
		}
		
		default:
			return LinkResult::Failure;
		}


        return LinkResult::Success;
    }

    LinkFlag ConcreteType::GetLinkFlags()
    {
		//return LinkFlag::None;
		
		//Needs to handle linking once when declaration happens 
		auto result = LinkFlag::Declaration;

		if (IsLinkExtern() == true)
			result |= LinkFlag::External;

		return  result;
    }



}