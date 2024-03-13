#pragma once

#include "BasicQualifier.h"
#include "RuntimeQualifier.h"
#include "DeclareSpecifier.h"

//*src
#include "Environment.h"
#include "Number.h"
#include "IdentityManager.h"
namespace LEX
{
	struct ITypePolicy;



	enum KeywordType
	{
		None,
		DeclSpec,
		TypeQual,
		TypeSpec,
	};




	struct HeaderSetting
	{
		KeywordType type = KeywordType::None;
		uint8_t comp = 0xFF;

		HeaderSetting() = default;
		HeaderSetting(KeywordType t, uint8_t c = 0xFF)
		{
			type = t;
			comp = c;
		}



		operator bool() const
		{
			return type != KeywordType::None;
		}
	};
	struct HeaderEntry : public HeaderSetting
	{
		std::string name;
	};


	inline static HeaderSetting Exclusive(KeywordType t)
	{
		return HeaderSetting(t, 0x0);
	}

	inline static std::unordered_map<std::string_view, HeaderSetting> headerMap
	{
		{ "typename", Exclusive(KeywordType::TypeSpec) },
		{"signed", KeywordType::TypeSpec},
		{"unsigned", KeywordType::TypeSpec},
		{"short", KeywordType::TypeSpec},
		{"long", KeywordType::TypeSpec},
		{"bool", KeywordType::TypeSpec},
		{"string", KeywordType::TypeSpec},
		{"int", KeywordType::TypeSpec},
		{"float", KeywordType::TypeSpec},
		{"double", KeywordType::TypeSpec},
		{"float", KeywordType::TypeSpec},
		{"void", KeywordType::TypeSpec},

		{"readonly~", KeywordType::DeclSpec},
		{"static", KeywordType::DeclSpec},
		{"const~", KeywordType::DeclSpec},
		{"mutable~", KeywordType::DeclSpec},


		{"ref", KeywordType::TypeQual},
		{"const", KeywordType::TypeQual},
		{"mutable", KeywordType::TypeQual},
	};

	inline HeaderEntry GetHeaderEntry(std::string name = "", bool post = false)
	{
		if (name.empty() == true) {
			name = "typename";
		}
		else if (post) {//This idea does not work.
			name += "~";
		}

		//TODO: Reimplement this, I'm too lazy and tired to do it now.		
		HeaderSetting setting = headerMap[name];

		if (!setting)
			return {};

		HeaderEntry entry;

		entry.name = name;
		entry.comp = setting.comp;
		entry.type = setting.type;

		return entry;
	}



	/*
	The idea is that there are 3 types of settings, restricted exclusive, and NA.

	NA is tab, other than it's categoric requirements it has none. Then there is restrictive which disallows other restrictives.
	Then there is exclusive which disallows everything else.

	Then there's the tag of the keyword, which if isn't "" states that all other things in the same category have to be within that.

	For example things solely able to be used on integer would be integer, while things that could be used on both floats and integers would be????
	I don't think this auto shit works.
	Wait, what I can do is use a flag integer on 2 different header settings, and if you or on them, and it equals zero (and both weren't already zero) they're incompatible.
	//*/

	//TODO: I'd like to have GetPolicyFromSpecifiers dealt with via strings, and/or able to be dealt with via a record.
	inline ITypePolicy* GetPolicyFromSpecifiers(Record& node, Environment* env)
	{
		ITypePolicy* result = nullptr;

		if (node.size() != 0)
		{
			Number::Settings settings{};

			std::string search_name;

			//For now this only really registers for numbers.

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
					settings.size = Size::QWord;
					settings.sign = Signage::Signed;
					settings.limit = Limit::Infinite;
					break;

				case "float_"_h:
					settings.type = NumeralType::Floating;
					settings.size = Size::DWord;
					settings.sign = Signage::Signed;
					settings.limit = Limit::Infinite;
					break;
				
				case "void"_h:
					return IdentityManager::GetTypeByID(0);

				case "string"_h:
					//fall.
					__fallthrough;

				default://typename
					search_name = name; break;
				}
			}

			if (search_name.empty() == false) {
				logger::critical(" faf a {}", search_name);
				result = env->TEMPSearchType(search_name);
			}
			else {
				settings.limit = settings.limit == Limit::Invalid ? Limit::Overflow : settings.limit;
				settings.size = settings.size == Size::Invalid ? Size::DWord : settings.size;
				settings.type = settings.type == NumeralType::Invalid ? NumeralType::Integral : settings.type;
				settings.sign = settings.sign == Signage::Invalid ? Signage::Signed : settings.sign;

				auto offset = settings.GetOffset();
				logger::critical(" faf???");
				result = LEX::IdentityManager::GetTypeByID(offset + 1);
			}
		}
		return result;
	}
	
	inline std::pair<BasicQualifier, RuntimeQualifier> GetQualifiersFromStrings(Record& node)
	{
		BasicQualifier basic{};
		RuntimeQualifier runtime{};

		for (auto& entry : node.GetChildren())
		{
			std::string& name = entry.GetTag();

			switch (Hash(name))
			{
			case "mutable"_h:
				basic |= BasicQualifier::Mutable;
				break;

			case "const"_h:
				basic |= BasicQualifier::Const;
				break;

			case "ref"_h:
				runtime |= RuntimeQualifier::Refr;
				break;

			}
		}

		return { basic, runtime };
	}

	inline DeclareSpecifier GetSpecifiersFromStrings(Record& node)
	{
		DeclareSpecifier decl{};

		for (auto& entry : node.GetChildren())
		{
			std::string& name = entry.GetTag();

			switch (Hash(name))
			{
			case "readonly"_h:
				decl |= DeclareSpecifier::Readonly;
			case "static"_h:
				decl |= DeclareSpecifier ::Static;
			case "const"_h:
				decl |= DeclareSpecifier::Const;
			//case "mutable~"_h://I don't think I actually intend to have mutables of this.

			}
		}

		return decl;
	}

	struct DeclareHeader
	{
		DeclareHeader() = default;
		//DeclareHeader(ITypePolicy* policy, BasicQualifier b, RuntimeQualifier r, DeclareSpecifier d){}
		DeclareHeader(Record& header, Environment* env)
		{
			if (header.GetTag() != "<header>") {
				report::fault::fatal("header not found.");
			}


			//std::array<Record&, 3> header_nodes{ *header.FindChild("type_qual") , *header.FindChild("type_spec"), *header.FindChild("decl_spec") };

			Record& type_qual = *header.FindChild("type_qual");
			Record& type_spec = *header.FindChild("type_spec");
			Record& decl_spec = *header.FindChild("decl_spec");

			//So here's the concept, if you see 2 restricteds in the same place, they aren't compatible. This simplifies this sort of thing.
			// rest
			//SyntaxType::Restricted;


			//Need some fucking rules about this.
			for (auto& node : header.GetChildren())
			{
				std::vector<HeaderEntry> entries{ node.size() };

				for (auto& child : node.GetChildren())
				{
					bool post = child.SYNTAX().type == SyntaxType::Declare;

					std::string name = child.SYNTAX().type == SyntaxType::Typename ? "" : child.GetTag();

					HeaderEntry entry = GetHeaderEntry(name, post);

					if (!entry) {
						report::compile::critical("Cannot create entry from header node.");
					}

					for (auto& a_entry : entries)
					{
						auto comp = a_entry.comp & entry.comp;

						if (!comp) {
							//Not compatible.
							report::compile::critical("Not compatible");
						}

						if (entry.name == a_entry.name) {
							//duplicate setting.
							report::compile::critical("Duplicate setting.");
						}
					}

					entries.push_back(entry);

				}
			}

			auto qualifier = GetQualifiersFromStrings(type_qual);

			basic = qualifier.first;
			runtime = qualifier.second;

			policy = GetPolicyFromSpecifiers(type_spec, env);

			declare = GetSpecifiersFromStrings(decl_spec);
		}


		//This is a declaration header. It stores declarations of functions, globals, members, methods etc.

		//How this will work is a function that checks for/filters (depending on what one wants) will be run through this struct.
		// For the check if a type exists when the query doesn't want one or vice versa, or when a qual/spec exists when not allowed it will return false.
		// Filter will merely clear out all unallowed things, and return the expunged as a new header.
	private:
		bool _filterByte = false;//This is used as a flag for filtering when policy is expected to be null but it isn't.
	public:
		//bool filtered = false;
		BasicQualifier basic{};
		RuntimeQualifier runtime{};
		//StoreSpecifier _3;//Declare Specs ARE store specs.
		DeclareSpecifier declare{};
		
		ITypePolicy* policy = nullptr;
		
		
		
		operator bool() const
		{
			//compare the rest of this shit. This is a struct, you should be able to do whatever the fuck you want to do, BUT, be careful.
			return basic || runtime || declare || policy || _filterByte;
		}

		//Make some combination functions for these.
		DeclareHeader Filter(bool type, BasicQualifier base = BasicQualifier::All, RuntimeQualifier run = RuntimeQualifier::All, DeclareSpecifier decl = DeclareSpecifier::All)
		{
			//static_assert(false, "This filter is not correct, it's supposed to remove stuff, and move it into a different header. this just moves the left overs into the new one.");

			DeclareHeader filter{};

			if (type)//If expecting a policy, 
				filter.policy = policy ? nullptr : policy;
			else//If not expecting a policy
				filter._filterByte = policy ? true : false;

			filter.basic = base & basic;
			filter.runtime = run & runtime;
			filter.declare = decl & declare;

			return filter;
		}


		bool Matches(bool type, BasicQualifier base = BasicQualifier::All, RuntimeQualifier run = RuntimeQualifier::All, DeclareSpecifier decl = DeclareSpecifier::All)
		{
			return true;
			return !Filter(type, base, run, decl);
		}



		//Also in light of this sort of thing, I want to use the ParamInput part of solution.
		// I'll call it ReturnOutput, it'll be the same thing, it just symbolizes what it is. This way I can keep the qualifiers.
	};
}