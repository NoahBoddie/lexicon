#pragma once

#include "ConcretePolicy.h"

#include "ObjectPolicyManager.h"

namespace LEX
{

	//TODO: REcordToInt exists in multiple places. A lot even. Please, centralize them.
	TypeOffset _RecordToInt(Record& ast)
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


	void ConcretePolicy::LoadFromRecord(Record& ast) 
	{
		_name = ast.GetTag();

		Record* settings = ast.FindChild("settings");

		if (!settings) {
			report::compile::critical("setting not found in type policy record");
		}

		switch (Hash(settings->GetChild(0).GetTag()))
		{
		case "class"_h:			_dataType = DataType::Class; break;//Makes a given object act like a class in assignment.
		case "struct"_h:		_dataType = DataType::Struct; break;
		case "interface"_h:		_dataType = DataType::Interface; break;
		default:				report::compile::critical("improper data type found."); break;
		}

		//Do something with generic. It's useless right now


		Record& obj_type = settings->GetChild(2);

		//I actually will allow interfaces to be external. It prevents them from being instantiable though.

		switch (Hash(obj_type.GetTag()))
		{
			//case "data"_h://Not allowed yet
		case "external"_h:
		{
			//Should clash with intrinsic.
			_linkExternal = true;

			if (obj_type.size() == 0) {
				report::compile::critical("external type requires some type.");
			}


			//unique_type.size()

			//save this shit til after linkage.
			//ObjectPolicy* ObjectPolicyManager::GetObjectPolicyFromName(obj_type.GetTag());

			Record& cat_name = obj_type.GetFront();

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
			report::compile::critical("PLACEHOLDER don't know how to handle unique type."); break;
		}

	}



    LinkResult ConcretePolicy::OnLink(LinkFlag flags)
    {
        Record& ast = *GetSyntaxTree();
		
		switch (flags)
		{
		case LinkFlag::Declaration:
		{
			//Handling inheritance here
			break;
		}


		case LinkFlag::External:
		{

			Record& obj_type = ast.FindChild("settings")->GetChild(2);

			if (obj_type.size() == 0) {
				report::compile::critical("external type requires some type.");
			}
			else if (obj_type.GetTag() != "external") {

			}
			Record& cat_name = obj_type.GetFront();

			category = cat_name.GetTag();

			//category name is completely optional.
			offset = cat_name.size() ? _RecordToInt(cat_name.GetFront()) : 0;

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

    LinkFlag ConcretePolicy::GetLinkFlags()
    {
		return LinkFlag::None;
		
		//Needs to handle linking once when declaration happens 
		auto result = LinkFlag::Declaration;

		if (_linkExternal)
			result |= LinkFlag::External;
		return  result;
    }
}