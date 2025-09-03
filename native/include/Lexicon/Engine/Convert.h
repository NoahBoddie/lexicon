#pragma once

#include "Lexicon/ICallableUnit.h"
#include "Lexicon/RuntimeVariable.h"

namespace LEX
{
	using ConvertFunc = RuntimeVariable(*)(RuntimeVariable);

	template<ConvertFunc Func>
	struct Convert final : public ICallableUnit
	{
		using Self = Convert<Func>;

		static Self& GetSingleton()
		{
			static Self singleton{};

			return singleton;
		}


		inline static Self* instance = &GetSingleton();

		virtual RuntimeVariable Execute(std::span<RuntimeVariable> args, Runtime*, RuntimeVariable*)
		{

			//Convert should be real simple.
			//here, it's supposed to return the conversion.
			return Func(args[0]);
		}


		Convert(ConvertFunc func) {}

	protected:
		constexpr Convert() = default;

		Convert(const Convert&) = delete;
		Convert(Convert&&) = delete;
		Convert& operator=(const Convert&) = delete;
		Convert& operator=(Convert&&) = delete;

	};
}