#pragma once

#include "IGenericArgument.h"
#include "ISpecial.h"
#include "TypeID.h"


#include "DataType.h"

#include "Conversion.h"

namespace LEX
{
	struct TypeID;
	class Variable;
	struct ICallableUnit;//No longer needed
	struct IFunction;
	struct AbstractTypePolicy;
	class RuntimeVariable;
	struct PolicyBase;
	enum struct DataType : uint8_t;

	struct InheritData;
	struct OverloadCode;

	//Using this will allow manual conversions to other types that ordinarily cannot convert, even without a explicitly declared conversion.
	//using Conversion = std::function<RuntimeVariable(RuntimeVariable)>;
	

	//use this instead of the other.
	enum struct ConversionFlag
	{
		None = 0 << 0,
		Explicit = 1 << 0,
		Initialize = 1 << 1,
		IgnoreAccess = 1 << 2,
	};



	enum struct ConversionType
	{
		Implicit,
		Explicit,

	};


	struct ITypePolicy : public ISpecial
	{
		constexpr static uint32_t NonGenericIndex = full_value<uint32_t>;




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
		virtual AbstractTypePolicy* GetTypePolicy(IGenericArgument* args) = 0;

		AbstractTypePolicy* FetchTypePolicy(IGenericArgument* args)
		{
			return this ? GetTypePolicy(args) : nullptr;
		}

		virtual bool IsAbstract() const { return false; }

		virtual TypeID GetTypeID() const = 0;

		virtual DataType GetDataType() const = 0;


		DataType FetchDataType() const
		{
			return this ? GetDataType() : DataType::Invalid;
		}



		TypeID FetchTypeID() const
		{
			logger::info("is this ? {}", !!this);
			return this ? GetTypeID() : TypeID{ 0 };
		}

		bool IsRelated(const ITypePolicy* rhs, const ITypePolicy* scope) const
		{
			return IsConvertibleTo(rhs, scope) > ConvertResult::Failure || rhs->IsConvertibleTo(this, scope) > ConvertResult::Failure;
		}

		
		virtual ConvertResult IsConvertibleTo(const ITypePolicy* rhs, const ITypePolicy* scope, Conversion* out = nullptr, ConversionType type = ConversionType::Implicit) const
		{
			//The function to tell if this is convertiable to the rhs. the out function is the required conversion
			// function. Unsure about what type to use right now, so it's void and defaulted to nothig.

			//We use a callableunit now, things like numeric with underlying conversions can just handle the conversions, 
			// similarly strings can do the same

			if (this == rhs)
				return ConvertResult::Exact;

			return ConvertResult::Ineligible;

		}

		ConvertResult IsConvertibleTo(const ITypePolicy* rhs, const ITypePolicy* scope, Conversion& out, ConversionType type = ConversionType::Implicit) const
		{
			//TODO: this is going to be hidden once specialized, so rename this and make the main version a pivot
			return IsConvertibleTo(rhs, scope, &out, type);
		}
		//Make some safe functions for these.

	INTERNAL:		
		virtual PolicyBase* GetTypeBase() = 0;
		virtual const PolicyBase* GetTypeBase() const = 0;
	public:
		virtual _String GetName() const = 0;
		

	INTERNAL:
		//this internal stuff is basically only really used for construction, as such, it's obscured from
		// non-implementation versions.
		//This should be const.
		//I want to remove this. it should be handled by the new user I think.
		//virtual std::vector<InheritData> GetInheritFrom(uint32_t hashMin, uint32_t idxInc) = 0;



		//I'm just forking this shit over, no real reason to do it 
		virtual const InheritData* GetInheritData(const ITypePolicy* type) const = 0;

		virtual std::vector<InheritData> GetInheritData() const = 0;

	};

}