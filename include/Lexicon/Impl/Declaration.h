#pragma once

//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"
#include "DeclareSpecifier.h"
#include "HeaderSettings.h"


//*src
#include "Environment.h"
#include "Number.h"
#include "IdentityManager.h"


namespace LEX
{
	struct ITypePolicy;



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
					return IdentityManager::instance->GetTypeByID(0);

				case "string"_h:
					return IdentityManager::instance->GetTypeByOffset("STRING", 0);

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
				//result = LEX::IdentityManager::GetTypeByID(offset + 1);
				result = LEX::IdentityManager::instance->GetTypeByOffset("NUMBER", offset);
			}
		}
		return result;
	}

	inline Qualifier GetQualifiersFromStrings(Record& node)
	{
		Qualifier flags{};

		for (auto& entry : node.GetChildren())
		{
			std::string& name = entry.GetTag();

			switch (Hash(name))
			{
			case "mutable"_h:
				flags |= Qualifier::Mutable;
				break;

			case "const"_h:
				flags |= Qualifier::Const;
				break;

			case "ref"_h:
				flags |= Qualifier::Refr;
				break;

			}
		}

		return flags;
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
				decl |= DeclareSpecifier::Static;
			case "const"_h:
				decl |= DeclareSpecifier::Const;
				//case "mutable~"_h://I don't think I actually intend to have mutables of this.

			}
		}

		return decl;
	}

	struct Declaration : public QualifiedType
	{
		//Declaration(ITypePolicy* policy, BasicQualifier b, RuntimeQualifier r, DeclareSpecifier d){}
	public:
		Declaration() = default;

		Declaration(Record& header, Environment* env)
		{
			if (header.GetTag() != "<header>") {
				report::fault::fatal("header not found.");
			}


			//std::array<Record&, 3> header_nodes{ *header.FindChild("type_qual") , *header.FindChild("type_spec"), *header.FindChild("decl_spec") };
			Record& type_qual = *header.FindChild("type_qual");
			Record& decl_spec = *header.FindChild("decl_spec");
			Record& type_spec = *header.FindChild("type_spec");

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
					std::string name = child.SYNTAX().type == SyntaxType::Field ? "typename" : child.GetTag();

					//bool post = child.SYNTAX().type == SyntaxType::Declare;

					HeaderEntry entry;


					auto it = headerGuide[type].find(name);

					if (headerGuide[type].end() == it) {
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

			this->flags = GetQualifiersFromStrings(type_qual);
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

		//QualifiedType type{};

		//Qualifier flags{};

		//StoreSpecifier _3;//Declare Specs ARE store specs.
		DeclareSpecifier declare{};

		//ITypePolicy* policy = nullptr;



		operator bool() const
		{
			//compare the rest of this shit. This is a struct, you should be able to do whatever the fuck you want to do, BUT, be careful.
			return flags || declare || policy || _filterByte;
		}

		//Make some combination functions for these.
		Declaration Filter(bool type, Qualifier qual = Qualifier::All, DeclareSpecifier decl = DeclareSpecifier::All)
		{
			//static_assert(false, "This filter is not correct, it's supposed to remove stuff, and move it into a different header. this just moves the left overs into the new one.");

			Declaration filter{};

			if (type)//If expecting a policy, 
				filter.policy = policy ? nullptr : policy;
			else//If not expecting a policy
				filter._filterByte = policy ? true : false;

			filter.flags = flags & qual;
			filter.declare = decl & declare;

			return filter;
		}


		bool Matches(bool type, Qualifier qual = Qualifier::All, DeclareSpecifier decl = DeclareSpecifier::All)
		{
			return true;
			return !Filter(type, qual, decl);
		}



		//Also in light of this sort of thing, I want to use the ParamInput part of solution.
		// I'll call it ReturnOutput, it'll be the same thing, it just symbolizes what it is. This way I can keep the qualifiers.
	};
}