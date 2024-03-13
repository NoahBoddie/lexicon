#pragma once

#include "IGenericArgument.h"
#include "Interface.h"
#include "TypeID.h"

namespace LEX
{
	struct TypeID;
	class Variable;
	struct ICallableUnit;//No longer needed
	struct IFunction;
	struct AbstractTypePolicy;
	class RuntimeVariable;


	//Using this will allow manual conversions to other types that ordinarily cannot convert, even without a explicitly declared conversion.
	//using Conversion = std::function<RuntimeVariable(RuntimeVariable)>;
	struct Conversion
	{
		enum Enum
		{
			Type,
			Policy,
			Function,
		};

		union
		{
			uint64_t raw = 0;
			ITypePolicy* convertPolicy;
			IFunction* convertFunction;
		};

		Enum type = Enum::Type;
		
		operator bool() const
		{
			return raw;
		}
	};


	enum struct ConversionType
	{
		Implicit,
		Explicit,
	};


	struct ITypePolicy : public Interface
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
		//TODO:Make second interface for "AbstractTypePolicy" which GetTypePolicy returns, and what stores all the type policy info.
		virtual AbstractTypePolicy* GetTypePolicy(IGenericArgument* args) = 0;

		AbstractTypePolicy* FetchTypePolicy(IGenericArgument* args)
		{
			return this ? GetTypePolicy(args) : nullptr;
		}

		virtual bool IsAbstract() const { return false; }

		virtual TypeID GetTypeID() const = 0;


		bool IsRelated(const ITypePolicy* rhs) const
		{
			return IsConvertibleTo(rhs) || rhs->IsConvertibleTo(this);
		}

		virtual bool IsConvertibleTo(const ITypePolicy* rhs, Conversion* out = nullptr, ConversionType type = ConversionType::Implicit) const
		{
			//The function to tell if this is convertiable to the rhs. the out function is the required conversion
			// function. Unsure about what type to use right now, so it's void and defaulted to nothig.

			//We use a callableunit now, things like numeric with underlying conversions can just handle the conversions, 
			// similarly strings can do the same

			return this == rhs;

		}

		bool IsConvertibleTo(const ITypePolicy* rhs, Conversion& out, ConversionType type = ConversionType::Implicit) const
		{
			//TODO: this is going to be hidden once specialized, so rename this and make the main version a pivot
			return IsConvertibleTo(rhs, &out, type);
		}


		
		

		bool IsGenericArg()
		{
			return GetGenericIndex() != NonGenericIndex;
		}


		virtual uint32_t GetGenericIndex()
		{
			//full value represents that it isn't generic.
			return full_value<uint32_t>;
		}
	};

}