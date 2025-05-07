#pragma once

#include "Lexicon/Engine/Declaration.h"

#include "Lexicon/Engine/Expression.h"

#include "Lexicon/Engine/TypeBase.h"

#include "Lexicon/Engine/parse_strings.h"

namespace LEX
{
	/*

	auto _run = [&](SyntaxRecord& entry)
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
				return IdentityManager::instance->GetInherentBase(InherentType::kVoid);

			case "string"_h:
				return IdentityManager::instance->GetBaseByOffset("STRING", 0);

			default://typename
				type_name = &entry;
				search_name = name; break;
			}
		};

	//*/


	ITypeInfo* GetPolicyFromSpecifiers(SyntaxRecord& node, Element* elem)
	{
		ITypeInfo* result = nullptr;

		if (node.size() != 0)
		{
			Number::Settings settings{};

			SyntaxRecord* type_name = nullptr;
			std::string search_name;

			//For now this only really registers for numbers.

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
			
			for (auto& entry : node.children())
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
					if (settings.sign == Signage::Invalid)
						settings.sign = Signage::Unsigned;
					if (settings.limit == Limit::Invalid)
						settings.limit = Limit::Bound;
					break;

					//Double is going to have to use other things.
				case "double"_h:
					settings.type = NumeralType::Floating;
					if (settings.size == Size::Invalid)
						settings.size = Size::QWord;
					if (settings.sign == Signage::Invalid)
						settings.sign = Signage::Signed;
					if (settings.limit == Limit::Invalid)
						settings.limit = Limit::Infinite;
					break;

				case "float"_h:
					settings.type = NumeralType::Floating;
					if (settings.size == Size::Invalid)
						settings.size = Size::DWord;
					if (settings.sign == Signage::Invalid)
						settings.sign = Signage::Signed;
					if (settings.limit == Limit::Invalid)
						settings.limit = Limit::Infinite;
					break;

				case "void"_h:
					return IdentityManager::instance->GetInherentType(InherentType::kVoid);

				case "voidable"_h:
					return IdentityManager::instance->GetInherentType(InherentType::kVoidable);

				case "object"_h:
					return IdentityManager::instance->GetTypeByOffset("CORE", 0);


				case "string"_h:
					return IdentityManager::instance->GetTypeByOffset("STRING", 0);

				default://typename
					type_name = &entry;
					break;
				}
			}

			if (type_name) {
				result = elem->SearchTypePath(*type_name).info;
			}
			else {
				settings.limit = settings.limit == Limit::Invalid ? Limit::Overflow : settings.limit;
				settings.size = settings.size == Size::Invalid ? Size::DWord : settings.size;
				settings.type = settings.type == NumeralType::Invalid ? NumeralType::Integral : settings.type;
				settings.sign = settings.sign == Signage::Invalid ? Signage::Signed : settings.sign;

				auto offset = settings.GetOffset();
				
				//result = LEX::IdentityManager::GetTypeByID(offset + 1);
				result = LEX::IdentityManager::instance->GetTypeByOffset("NUMBER", offset);
				assert(result);
				
			}
		}
		return result;
	}


	Declaration::Declaration(SyntaxRecord& header, Element* source, Reference genericRef, Reference defaultRef)
	{
		if (header.GetTag() != parse_strings::header) {
			report::fault::critical("header not found.");
		}


		//std::array<SyntaxRecord&, 3> header_nodes{ *header.FindChild("type_qual") , *header.FindChild("type_spec"), *header.FindChild("decl_spec") };
		SyntaxRecord& type_qual = *header.FindChild(parse_strings::type_qualifier);
		SyntaxRecord& decl_spec = *header.FindChild(parse_strings::declare_specifier);
		SyntaxRecord& type_spec = *header.FindChild(parse_strings::type_specifier);

		//So here's the concept, if you see 2 restricteds in the same place, they aren't compatible. This simplifies this sort of thing.
		// rest
		//SyntaxType::Restricted;


		HeaderFlag flags{};
		HeaderFlag excludes{};

		KeywordType type = KeywordType::TypeQual;

		//Need some fucking rules about this.
		for (auto& node : header.children())
		{

			for (auto& child : node.children())
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

				auto it = headerGuide[type].find(name);

				if (headerGuide[type].end() == it) {
					//This didn't crash for some fucking reason????
					report::fault::critical("cannot find setting flags for keyword {}", name);
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
		
		policy = GetPolicyFromSpecifiers(type_spec, source);
		*this = GetQualifiersFromStrings(type_qual);
		*this = GetSpecifiersFromStrings(decl_spec);



		switch (reference)
		{
		case Reference::Generic:
			reference = genericRef;
			break;

		case Reference::Temp:
			reference = defaultRef;
			break;
		}
	}
}