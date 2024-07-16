#pragma once

#include "Lexicon/QualifiedType.h"

namespace LEX
{
	enum struct SignatureEnum
	{
		Result,
		Target,
		Argument,
	};


	struct SignatureBase
	{
		//This doesn't use a vtable, just increases the size slightly to ensure that it's not using features
		// it doesn't have. Due to the structural nature of this object it is unlikely that I'll use a vtable.
		const size_t size = sizeof(SignatureBase);
		QualifiedType result;
		QualifiedType target;

		std::vector<QualifiedType> parameters;

		//Ideally, you'd place access functions that would check the offset of the queried type before accessing,
		// incase the pointer to the other signature isn't that large.


		template <SignatureEnum T, typename E, typename... Next>
		bool Fill()
		{
			//In order to handle specific reference types, or specific qualifiers, there will be a wrapper type to handle it.
			// I'll just unpack it when the time comes. This would mostly be used for the target of a function.
			// under these situations, it will use the suggested  qualifiers rather than the the true ones.

			//If an the last entry is the callInfo, ignore it and move on. If it's callinfo but not last, do not compile.


			if constexpr (std::is_same_v<E, Void> && T == SignatureEnum::Argument) {
				//Do not compile actually.
				return false;
			}


			//Need to save this for later.
			//if constexpr (Tar && !std::is_reference_v<E>) {
			//	return false;
			//}

			constexpr auto next_size = sizeof...(Next);

			using _Pointless = std::remove_pointer_t<E>;
			using _Refless = std::remove_const_t<std::remove_reference_t<E>>;
			using _Naked = std::remove_pointer_t<_Pointless>;//Might be better to get the underlying value of the thing.

			QualifiedType& entry = T == SignatureEnum::Result ?
				result : T == SignatureEnum::Target ?
				target : parameters.emplace_back();

			logger::info("increase? {}", parameters.size());

			entry.policy = GetVariableType_<_Refless>();


			if constexpr (std::is_const_v<_Naked>) {
				entry.flags |= Qualifier::Const;
			}

			//For right now it really doesn't matter.
			if constexpr (std::is_reference_v<E>) {
				entry.flags |= Qualifier::RefL;
			}



			if constexpr (next_size)
			{
				constexpr SignatureEnum Enum = T == SignatureEnum::Result ?
					SignatureEnum::Target : T == SignatureEnum::Target ?
					SignatureEnum::Argument : T;


				return Fill<Enum, Next...>();
			}
			else
			{
				return true;
			}
		}

	};


}