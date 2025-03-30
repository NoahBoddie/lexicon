#pragma once

//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"
#include "Lexicon/DeclareSpecifier.h"
#include "Lexicon/Engine/HeaderSettings.h"
#include "Lexicon/QualifiedType.h"

//*src
#include "Lexicon/Engine/Environment.h"
#include "Lexicon/Number.h"
#include "Lexicon/Interfaces/IdentityManager.h"


namespace LEX
{
	struct TypeBase;

	class TempTest;
	


	//A group of flags to check matching for when it comes to declarations
	ENUM(DeclareMatches, int8_t)
	{

		None = 0,
		//Type = 1 << 0,//This was taken away due to not being relevant, declarations always have types
		Constness = 1 << 1,
		Reference = 1 << 2,

		All = -1,
	};

	bool Matches(DeclareMatches match_flags, QualifierFlag qual_flags = QualifierFlag::All, SpecifierFlag spec_flags = SpecifierFlag::All);




	//TODO: I'd like to have GetPolicyFromSpecifiers dealt with via strings, and/or able to be dealt with via a record.
	TypeBase* GetPolicyFromSpecifiers(SyntaxRecord& node, Element* elem);









	inline Qualifier GetQualifiersFromStrings(SyntaxRecord& node)
	{
		Qualifier qualifiers{};

		for (auto& entry : node.children())
		{
			std::string& name = entry.GetTag();

			switch (Hash(name))
			{
			case "mutable"_h:
				qualifiers.constness = Constness::Mutable;
				break;

			case "const"_h:
				qualifiers.constness = Constness::Const;
				break;

			case "ref"_h:
				//report::break_warn("Sorry dumbo, don't know how to handle refs yet");
				qualifiers.reference = Reference::Generic;
				break;

			}
		}

		return qualifiers;
	}

	inline Specifier GetSpecifiersFromStrings(SyntaxRecord& node)
	{
		Specifier specifiers{};

		for (auto& entry : node.children())
		{
			std::string& name = entry.GetTag();

			switch (Hash(name))
			{
			//case "readonly"_h:
			//	specifiers = SpecifierFlag::Readonly;
			//	break;

			case "static"_h:
				specifiers.flags = SpecifierFlag::Static;
				break;

			case "const"_h:
				specifiers.flags = SpecifierFlag::Const;
				break;
				//case "mutable~"_h://I don't think I actually intend to have mutables of this.
			case "public"_h:
				specifiers.access = Access::Public;
				break;

			case "private"_h:
				specifiers.access = Access::Private;
				break;

			case "protected"_h:
				specifiers.access = Access::Protected;
				break;

			case "internal"_h:
				specifiers.access |= Access::Internal;
				break;

			case "external"_h:
				specifiers.flags |= SpecifierFlag::External;
				break;

			default:
				report::error("unknown specifier '{}' detected", name);
			}
		}

		return specifiers;
	}


	//TODO: Merge declaration with Specifier. No reason to be seperate
	struct Declaration : public QualifiedType, public Specifier
	{
	public:
		Declaration() = default;

		//Do this via function
		Declaration(SyntaxRecord& header, Element* source, Reference genericRef, Reference defaultRef = Reference::Temp);

		using QualifiedType::operator=;
		Declaration& operator=(const Specifier& other) { __super::operator=(other); return *this; }
		



		//This is a declaration header. It stores declarations of functions, globals, members, methods etc.

		//How this will work is a function that checks for/filters (depending on what one wants) will be run through this struct.
		// For the check if a type exists when the query doesn't want one or vice versa, or when a qual/spec exists when not allowed it will return false.
		// Filter will merely clear out all unallowed things, and return the expunged as a new header.
	private:
		bool _filterByte = false;//This is used as a flag for filtering when policy is expected to be null but it isn't.
	public:
		
		bool FilterHasValue() const
		{
			//compare the rest of this shit. This is a struct, you should be able to do whatever the fuck you want to do, BUT, be careful.
			return (Qualifier::operator bool() || Specifier::operator bool() || policy || _filterByte);
		}

		Declaration Filter(DeclareMatches match_flags, QualifierFlag qual = QualifierFlag::All, SpecifierFlag spec = SpecifierFlag::All)
		{
			Declaration filter{};

			//TODO: This is cycle switch. Please replicate it in RGL
			for (int i = 1; i; i <<= 1)
			{
				if (match_flags & i)
				{
					switch ((DeclareMatches)i)
					{
					case DeclareMatches::Reference:
						if (!reference)
							filter.reference = reference;
						break;

					case DeclareMatches::Constness:
						if (!constness)
							filter.constness = constness;
						break;
					}
				}
				
			}

			
			filter._filterByte = !policy;
			filter.QualifierFlags() = QualifierFlags() & ~qual;
			filter.SpecifierFlags() = SpecifierFlags() & ~spec;

			return filter;
		}


		operator bool() const
		{
			return FilterHasValue();
		}

		bool Matches(DeclareMatches match_flags, QualifierFlag qual = QualifierFlag::All, SpecifierFlag spec = SpecifierFlag::All)
		{
			return !Filter(match_flags, qual, spec).FilterHasValue();
		}

		bool Matches(DeclareMatches match_flags, SpecifierFlag spec)
		{
			return Matches(match_flags, QualifierFlag::All, spec);
		}


		bool Matches(QualifierFlag qual)
		{
			return Matches(DeclareMatches::All, qual, SpecifierFlag::All);
		}

		bool Matches(SpecifierFlag spec)
		{
			return Matches(DeclareMatches::All, QualifierFlag::All, spec);
		}




		//Also in light of this sort of thing, I want to use the ParamInput part of solution.
		// I'll call it ReturnOutput, it'll be the same thing, it just symbolizes what it is. This way I can keep the qualifiers.
	};
}