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


		constexpr auto k_external = "interface";

		Record& obj_type = settings->GetChild(2);

		switch (Hash(obj_type.GetTag()))
		{
			//case "data"_h://Not allowed yet
		case "external"_h:
		{
			//Should clash with intrinsic.

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
			if (_dataType == DataType::Interface) {
				report::compile::critical("interfaces cannot be intrinsic/external");
			}
			break;

		default:
			report::compile::critical("PLACEHOLDER don't know how to handle unique type."); break;
		}

	}

}