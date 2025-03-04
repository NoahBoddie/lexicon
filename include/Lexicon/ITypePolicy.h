#pragma once

#include "Lexicon/IGenericArgument.h"
#include "Lexicon/ISpecial.h"
#include "Lexicon/TypeID.h"


#include "Lexicon/DataType.h"

#include "Lexicon/Conversion.h"




namespace LEX
{
	//Please move this
	ENUM(TypeRuleset)
	{
		None			= 0,		
		ReturnOpt		= 1 << 0,	//Controls if when in a return the option of returning it is optional
		NoReturn		= 1 << 1,	//Controls if the type can be used in a return or not
		NoVariable		= 1 << 2,	//Specifies it cannot be used as a parameter or variable.
		NoGlobal		= 1 << 3,	//Specifies it can't be used as a global (runtime classes have this)
	};


	struct TypeID;
	class Variable;
	struct ICallableUnit;//No longer needed
	struct IFunction;
	struct AbstractTypePolicy;
	class RuntimeVariable;
	struct PolicyBase;
	struct HierarchyData;

	enum struct DataType : uint8_t;

	struct InheritData;
	struct OverloadCode;


	struct GenericBase;
	struct ITemplatePart;
	struct ITemplateBody;

	//Using this will allow manual conversions to other types that ordinarily cannot convert, even without a explicitly declared conversion.
	//using Conversion = std::function<RuntimeVariable(RuntimeVariable)>;
	

	//use this instead of the other.
	ENUM(ConversionFlag)
	{
		None = 0 << 0,
		Explicit = 1 << 0,
		Return = 1 << 1,
		Parameter = 1 << 2,
		Initialize = 1 << 3,
		IgnoreAccess = 1 << 4,
	};



	struct ITypePolicy : public ISpecial
	{
		constexpr static uint32_t NonGenericIndex = -1;




		//ITypePolicy();

		//ITypePolicy(uint32_t i);

		//ITypePolicy(std::string_view name, TypeOffset offset);


		//ITypePolicy(ISpecializable* tar) : _target{ tar } {}

		//An interface of a type policy, which may or may not be a policy. Generally, one should use all the functions that this provides in order
		// to use the type policy.


		//Unsure of how this gets one however.


		//Might require an ISpecializable target. Actually, move this shit.
		//ISpecializable* _target = nullptr;

		//This function basically expects a number of things to be submitted, but here, the full specialization of a calling function could be used in order to
		// get a partially specialized function.
		//I may still go with interface because I don't think I want something where I derive a specializaton from TypePolicy, it'd be a waste of space, literal repeat.
		// I KNOW, instead, I could make an implementation policy
		//TODO:Make second interface for "AbstractTypePolicy" which GetType returns, and what stores all the type policy info.
		virtual AbstractTypePolicy* GetTypePolicy(ITemplateBody* args) = 0;

		//This should be hidden.
		virtual ITypePolicy* CheckTypePolicy(GenericBase* base, ITemplatePart* args) { return this; };

		AbstractTypePolicy* FetchTypePolicy(ITemplateBody* args)
		{
			return this ? GetTypePolicy(args) : nullptr;
		}

		virtual bool IsResolved() const { return false; }

		virtual TypeID GetTypeID() const = 0;

		virtual DataType GetDataType() const = 0;


		DataType FetchDataType() const
		{
			return this ? GetDataType() : DataType::Invalid;
		}



		TypeID FetchTypeID() const
		{
			return this ? GetTypeID() : TypeID{ 0 };
		}

		bool IsRelated(const ITypePolicy* rhs, const ITypePolicy* scope) const
		{
			return IsConvertibleTo(rhs, scope) > ConvertResult::Failure || rhs->IsConvertibleTo(this, scope) > ConvertResult::Failure;
		}

		//Scope should be an environment that turns itself into an ITypePolicy.
		virtual ConvertResult IsConvertibleTo(const ITypePolicy* rhs, const ITypePolicy* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const
		{
			//The function to tell if this is convertiable to the rhs. the out function is the required conversion
			// function. Unsure about what type to use right now, so it's void and defaulted to nothig.

			//We use a callableunit now, things like numeric with underlying conversions can just handle the conversions, 
			// similarly strings can do the same

			if (this == rhs)
				return ConvertResult::Exact;

			return ConvertResult::Ineligible;

		}

		ConvertResult IsConvertibleTo(const ITypePolicy* rhs, const ITypePolicy* scope, Conversion& out, ConversionFlag flags = ConversionFlag::None) const
		{
			//TODO: this is going to be hidden once specialized, so rename this and make the main version a pivot
			return IsConvertibleTo(rhs, scope, &out, flags);
		}
		//Make some safe functions for these.


	public:
		virtual std::string_view GetName() const = 0;

	INTERNAL:		
		//At a later point this will die and be forgotten. I seek to have a type that can handle most of hierarchies needs,
		// without the explicit need of having a hierarchy data explicitly existing. Might make it a reference to send a message.
		virtual HierarchyData* GetHierarchyData() const = 0;

		//TODO: Allow TypeRuleset to be a pure virtual. I need to chase down where they should be used.
		//virtual TypeRuleset GetRuleset() const = 0;
		virtual TypeRuleset GetRuleset() const
		{
			return TypeRuleset::None;
		}


		bool CheckRuleset(TypeRuleset rule) const
		{
			return GetRuleset() & rule;
		}

		bool TryRuleset(TypeRuleset rule, bool def = false) const
		{
			return !this ? def : CheckRuleset(rule);
		}
	};

}