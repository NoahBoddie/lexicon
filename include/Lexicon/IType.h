#pragma once

#include "Lexicon/ISpecial.h"
#include "Lexicon/TypeID.h"


#include "Lexicon/DataType.h"

#include "Lexicon/Conversion.h"




namespace LEX
{

	struct TypeID;
	class Variable;
	struct ICallableUnit;//No longer needed
	struct IFunction;
	struct Type;
	class RuntimeVariable;
	struct TypeBase;
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



	struct IType : public ISpecial
	{
		constexpr static uint32_t NonGenericIndex = -1;




		//IType();

		//IType(uint32_t i);

		//IType(std::string_view name, TypeOffset offset);


		//IType(ISpecializable* tar) : _target{ tar } {}

		//An interface of a type policy, which may or may not be a policy. Generally, one should use all the functions that this provides in order
		// to use the type policy.


		//Unsure of how this gets one however.


		//Might require an ISpecializable target. Actually, move this shit.
		//ISpecializable* _target = nullptr;

		//This function basically expects a number of things to be submitted, but here, the full specialization of a calling function could be used in order to
		// get a partially specialized function.
		//I may still go with interface because I don't think I want something where I derive a specializaton from TypePolicy, it'd be a waste of space, literal repeat.
		// I KNOW, instead, I could make an implementation policy
		//TODO:Make second interface for "Type" which GetType returns, and what stores all the type policy info.
		virtual Type* GetTypePolicy(ITemplateBody* args) = 0;

		//This should be hidden.
		virtual BasicType* CheckTypePolicy(ITemplatePart* args) = 0;

		Type* FetchTypePolicy(ITemplateBody* args)
		{
			return this ? GetTypePolicy(args) : nullptr;
		}


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


		//Scope should be an environment that turns itself into an IType.
		virtual ConvertResult IsConvertibleTo(const BasicType* rhs, const BasicType* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const
		{
			//The function to tell if this is convertiable to the rhs. the out function is the required conversion
			// function. Unsure about what type to use right now, so it's void and defaulted to nothig.

			//We use a callableunit now, things like numeric with underlying conversions can just handle the conversions, 
			// similarly strings can do the same

			if (this == (IType*)rhs)
				return ConvertResult::Exact;

			return ConvertResult::Ineligible;

		}

		ConvertResult IsConvertibleTo(const BasicType* rhs, const BasicType* scope, Conversion& out, ConversionFlag flags = ConversionFlag::None) const
		{
			//TODO: this is going to be hidden once specialized, so rename this and make the main version a pivot
			return IsConvertibleTo(rhs, scope, &out, flags);
		}
		//Make some safe functions for these.


	public:
		virtual std::string_view GetName() const = 0;

		virtual std::span<BasicType*> GetTemplate() { return {}; }

	
	};
}