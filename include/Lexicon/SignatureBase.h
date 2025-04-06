#pragma once

#include "Lexicon/Impl/ref_wrapper.h"
#include "Lexicon/AnnotatedType.h"

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
		//I'm turning my back on this declaration and likely to be giving signature base a vtable.

		//This doesn't use a vtable, just increases the size slightly to ensure that it's not using features
		// it doesn't have. Due to the structural nature of this object it is unlikely that I'll use a vtable.
		AnnotatedType result;
		AnnotatedType target;

		std::vector<AnnotatedType> parameters;

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
			
			constexpr auto k_ref_type = detail::reference_type_v<E, T == SignatureEnum::Result>;

			using _Refless = std::remove_const_t<std::remove_reference_t<E>>;
			using _Pointless = std::remove_pointer_t<E>;
			using _Naked = std::remove_pointer_t<_Pointless>;//Might be better to get the underlying value of the thing.
			
			AnnotatedType& entry = T == SignatureEnum::Result ?
				result : T == SignatureEnum::Target ?
				target : parameters.emplace_back();


			entry.policy = GetVariableType<_Refless>();
			//entry.policy = GetVariableType<_Naked>();

			//static_assert(false, "SignatureBase currently has some problems with the data it sets up. No idea why. needs addressing.");



			if constexpr (std::is_const_v<_Naked>) {
				entry.constness = Constness::Const;
			}
			else {
				entry.constness = Constness::Modable;
			}

			if constexpr (T != SignatureEnum::Target)
			{
				if constexpr (k_ref_type == detail::kMaybeRef) {
					entry.reference = Reference::Maybe;
				}
				else if constexpr (k_ref_type == detail::kLocalRef) {
					entry.reference = Reference::Local;
				}
				else if constexpr (k_ref_type == detail::kScopeRef) {
					entry.reference = Reference::Scoped;
				}
				else if constexpr (k_ref_type == detail::kGlobalRef) {
					entry.reference = Reference::Global;
				}
				else {
					if (T != SignatureEnum::Result) {
						entry.reference = Reference::Auto;
					}
				}
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

	protected:
		SignatureBase() = default;
	};



	namespace Version
	{
		namespace _1
		{

#define PULL_FROM_SIG(mc_name)mc_name##_impl() const { return base()->mc_name; }
#define GET_FROM_SIG(mc_name)mc_name() const{ CHECK_INTERFACE_VERSION({}); return  mc_name##_impl(); }
#define GET_COLL_SIG(mc_name)mc_name() const{ CHECK_INTERFACE_VERSION({}); auto coll = mc_name##_impl(); return  { std::begin(coll), std::end(coll) }; }

			
			struct INTERFACE_VERSION(ISignature, public SignatureBase)
			{
			protected:
				//I'm like 50 percent sure this should be inheriting protected to prevent misuse if the sizes are mismanaged

				//ISignature() = default;
				//ISignature(const ISignature&) = default;
				//ISignature(ISignature&&) = default;
				SignatureBase* base() { return this; }
				const SignatureBase* base() const { return this; }

				virtual AnnotatedType PULL_FROM_SIG(result);
				virtual AnnotatedType PULL_FROM_SIG(target);
				virtual std::span<const AnnotatedType> PULL_FROM_SIG(parameters);

			public:

				AnnotatedType GET_FROM_SIG(result);
				AnnotatedType GET_FROM_SIG(target);
				std::vector<AnnotatedType> GET_COLL_SIG(parameters);
			};

		}

#undef GET_FROM_SIG
#undef PULL_FROM_SIG
#undef GET_COLL_SIG

		CURRENT_VERSION(ISignature, 1);

	}

	//This is to be used without reference in interfaces, having it plugged in anywhere where there's a signature base.
	struct IMPL_VERSION(ISignature){};
	REQUIRED_SIZE(ISignature, sizeof(SignatureBase) + 0x8);
}