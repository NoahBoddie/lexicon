#pragma once

#include "Declaration.h"


#include "PolicyBase.h"

#include "parse_strings.h"

namespace LEX
{
	PolicyBase* GetPolicyFromSpecifiers(Record& node, Environment* env)
	{
		PolicyBase* result = nullptr;

		if (node.size() != 0)
		{
			Number::Settings settings{};

			Record* type_name = nullptr;
			std::string search_name;

			//For now this only really registers for numbers.

			auto _run = [&](Record& entry)
			{
				std::string& name = entry.GetTag();

				switch (Hash(name))
				{
				case "signed"_h:
					settings.sign = Signage::Signed;
					break;

				case "unsigned"_h:
					settings.sign = Signage::Unsigned;
					break;

				case "short"_h:
					settings.size = Size::Word;
					break;

				case "long"_h:
					settings.size = Size::QWord;
					break;

				case "int"_h:
					settings.type = NumeralType::Integral;
					break;

				case "bool"_h:
					settings.type = NumeralType::Integral;
					settings.size = Size::Bit;
					break;

					//Double is going to have to use other things.
				case "double"_h:
				case "float"_h://Right now, float IS double.

					settings.type = NumeralType::Floating;
					if (settings.size == Size::Invalid)
						settings.size = Size::QWord;
					if (settings.sign == Signage::Invalid)
						settings.sign = Signage::Signed;
					if (settings.limit == Limit::Invalid)
						settings.limit = Limit::Infinite;
					break;

				case "float_"_h:
					settings.type = NumeralType::Floating;
					if (settings.size == Size::Invalid)
						settings.size = Size::DWord;
					if (settings.sign == Signage::Invalid)
						settings.sign = Signage::Signed;
					if (settings.limit == Limit::Invalid)
						settings.limit = Limit::Infinite;

					break;

				case "void"_h:
					return IdentityManager::instance->GetBaseByID(0);

				case "string"_h:
					return IdentityManager::instance->GetBaseByOffset("STRING", 0);

				default://typename
					type_name = &entry;
					search_name = name; break;
				}
			};

			//This needs to work differently in the future, but this works for now.
			//switch (node.SYNTAX().type)
			//{
			//case SyntaxType::Header:
			//	for (auto& entry : node.GetChildren())
			//	{
			//		_run(entry);
			//	}
			//	break;
			//default:
			//	_run(node); break;
			//}
			
			for (auto& entry : node.GetChildren())
			{
				std::string& name = entry.GetTag();

				switch (Hash(name))
				{
				case "signed"_h:
					settings.sign = Signage::Signed;
					break;

				case "unsigned"_h:
					settings.sign = Signage::Unsigned;
					break;

				case "short"_h:
					settings.size = Size::Word;
					break;

				case "long"_h:
					settings.size = Size::QWord;
					break;

				case "int"_h:
					settings.type = NumeralType::Integral;
					break;

				case "bool"_h:
					settings.type = NumeralType::Integral;
					settings.size = Size::Bit;
					break;

					//Double is going to have to use other things.
				case "double"_h:
				case "float"_h://Right now, float IS double.

					settings.type = NumeralType::Floating;
					if (settings.size == Size::Invalid)
						settings.size = Size::QWord;
					if (settings.sign == Signage::Invalid)
						settings.sign = Signage::Signed;
					if (settings.limit == Limit::Invalid)
						settings.limit = Limit::Infinite;
					break;

				case "float_"_h:
					settings.type = NumeralType::Floating;
					if (settings.size == Size::Invalid)
						settings.size = Size::DWord;
					if (settings.sign == Signage::Invalid)
						settings.sign = Signage::Signed;
					if (settings.limit == Limit::Invalid)
						settings.limit = Limit::Infinite;

					break;

				case "void"_h:
					return IdentityManager::instance->GetBaseByID(0);

				case "string"_h:
					return IdentityManager::instance->GetBaseByOffset("STRING", 0);

				default://typename
					type_name = &entry;
					search_name = name; break;
				}
			}

			if (search_name.empty() == false) {
				logger::critical("searching {} (may be a scope operator", type_name->GetTag());
				result = env->SearchTypePath(*type_name);
			}
			else {
				settings.limit = settings.limit == Limit::Invalid ? Limit::Overflow : settings.limit;
				settings.size = settings.size == Size::Invalid ? Size::DWord : settings.size;
				settings.type = settings.type == NumeralType::Invalid ? NumeralType::Integral : settings.type;
				settings.sign = settings.sign == Signage::Invalid ? Signage::Signed : settings.sign;

				auto offset = settings.GetOffset();
				logger::critical(" faf???");
				//result = LEX::IdentityManager::GetTypeByID(offset + 1);
				result = LEX::IdentityManager::instance->GetBaseByOffset("NUMBER", offset);
			}
		}
		return result;
	}


	Declaration::Declaration(Record& header, Environment* env)
	{
		if (header.GetTag() != parse_strings::header) {
			report::fault::fatal("header not found.");
		}


		//std::array<Record&, 3> header_nodes{ *header.FindChild("type_qual") , *header.FindChild("type_spec"), *header.FindChild("decl_spec") };
		Record& type_qual = *header.FindChild(parse_strings::type_qualifier);
		Record& decl_spec = *header.FindChild(parse_strings::declare_specifier);
		Record& type_spec = *header.FindChild(parse_strings::type_specifier);

		//So here's the concept, if you see 2 restricteds in the same place, they aren't compatible. This simplifies this sort of thing.
		// rest
		//SyntaxType::Restricted;


		HeaderFlag flags{};
		HeaderFlag excludes{};

		KeywordType type = KeywordType::TypeQual;

		//Need some fucking rules about this.
		for (auto& node : header.GetChildren())
		{

			for (auto& child : node.GetChildren())
			{
				//std::string name = child.SYNTAX().type == SyntaxType::Declare ? 
				std::string name;

				switch (child.SYNTAX().type)
				{
				case SyntaxType::ProjectName:
				case SyntaxType::Scriptname:
				case SyntaxType::Typename:
				case SyntaxType::Scopename:
					name = "typename";
					break;

				default:
					name = child.GetTag();
					break;
				}

				//bool post = child.SYNTAX().type == SyntaxType::Declare;

				HeaderEntry entry;
				logger::info("{} ???", ExpressionToString(child.SYNTAX().type));

				auto it = headerGuide[type].find(name);

				if (headerGuide[type].end() == it) {
					//This didn't crash for some fucking reason????
					report::fault::fatal("cannot find setting flags for keyword {}", name);
				}

				entry = it->second;


				if (entry.includeFlags & excludes != HeaderFlag::None)
					return;

				if (entry.includeFlags & flags != HeaderFlag::None)
					return;

				flags |= entry.includeFlags;
				excludes |= entry.excludeFlags;
			}

			type = static_cast<decltype(type)>(type + 1);
		}

		this->flags = GetQualifiersFromStrings(type_qual);
		policy = GetPolicyFromSpecifiers(type_spec, env);
		declare = GetSpecifiersFromStrings(decl_spec);


	}
}