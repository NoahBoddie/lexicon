#pragma once 


#include "Lexicon/Engine/ConcreteType.h"

namespace LEX
{
	//TODO: Due to the existence of VoidPolicy I will need a "IsPolicyValid" function.
	// or something like that to say it's an instantiatible type.

	//Also additionally, this shit should be a singleton.
	class NullType : public ConcreteType
	{
	public:

		constexpr TypeRuleset GetRuleset() const noexcept override
		{
			return TypeRuleset::NoGlobal | 
					TypeRuleset::NoVariable | 
					TypeRuleset::NoReturn | 
					TypeRuleset::NoInstant | 
					TypeRuleset::NoMembers;
		}

		//Should do nothing.


		static RuntimeVariable StrToDouble(const RuntimeVariable& a_this, Runtime*)
		{
			return (double)a_this->AsString().size();
		}

		ConvertResult GetConvertTo(const ITypeInfo* other, const ITypeInfo* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const override
		{
			//For now, this will be very specific. It won't even exist later. But for now, the idea is that this should be able to transfer into a string.
			//Later, I'm going to just make a thing that manages conversions akin to a dispatcher.

			ConvertResult result = __super::GetConvertTo(other, scope, out, flags);


			if (out && result <= ConversionEnum::Failure)
			{
				if (other->IsReferenceType() == true)
				{
					out->SetUserImpl(unconst(other));
					result = ConversionEnum::ImplDefined;
					//static_assert(false, "this needs a callable unit.");
				}

			}

			return result;
		}

		/*
		static RuntimeVariable ToNullptr(RuntimeVariable& other)
		{
			return other->IsValueZero() ? Variable{} : other.Ref();
		}

		static RuntimeVariable FromNullptr(RuntimeVariable other)
		{
			//Going to need something like convert to handle this. Will probably try to create it myself. Maybe it'll be something that takes
			// an entry for a type and spits out an ICallable that creates a version of that type.
			return other->IsValueZero() ? Variable{} : other.Ref();
		}
		//*/




		NullType() : ConcreteType{ InherentType::kNull }
		{
			_dataType = DataType::Class;
		}

	};
}