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
	struct PolicyBase;

	class TempTest;
	
	//TODO: I'd like to have GetPolicyFromSpecifiers dealt with via strings, and/or able to be dealt with via a record.
	PolicyBase* GetPolicyFromSpecifiers(Record& node, Environment* env);

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
				flags |= Qualifier::Reference_;
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

		Declaration(Record& header, Environment* env);


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