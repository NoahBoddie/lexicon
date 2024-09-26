#pragma once


#include "Lexicon/IFormula.h"
#include "Lexicon/VariableType.h"
#include "Lexicon/FormulaHandler.h"

#include "Lexicon/Interfaces/FormulaManager.h"

namespace LEX
{


	template <typename T>
	struct Formula;


	//make summary later.
	template <detail::function_has_var_type T>
	struct Formula<T>
	{
		//This version of formula is completely removed from play
	private:
		Formula() = delete;
		Formula(const Formula&) = delete;
		Formula(Formula&&) = delete;
		Formula& operator=(const Formula&) = delete;
		Formula& operator=(Formula&&) = delete;

	public:

		//This sorta needs to be able to make sure that the item can unvariable itself.

		static T Run(std::string_view routine, std::string_view from = ""/*, default here*/)
		{

			FormulaHandler self;

			SignatureBase base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, T, StaticTargetTag>();

			if (processed)
			{

				std::vector<std::string_view> params{};

				auto result = FormulaManager::instance->RequestFormula(base, params, routine, self, from);

				//Send out a message here.
			}
			
			if (self) {
				
				RuntimeVariable result = self.formula()->Call();
				
				if constexpr (!std::is_same_v<void, T>) {
					return Unvariable<T>{}(result.Ptr());
				}
			}

			//Send an exception. probably.
			if constexpr (std::is_same_v<void, T>) {
				return;
			}
			else {
				return {};
			}

		}

	};

	template <typename From, typename To>
	struct change_to { typedef To type; };

	template <typename From, typename To>
	using change_to_t = change_to<From, To>::type;



	template <typename R, typename... Args>
	struct Formula<R(Args...)> : public FormulaHandler
	{
		using Self = Formula<R(Args...)>;


		//The idea of this is you cast an IFormula into this forcibly, and this type will then manage all of the function calls for the type.
		// Basically, think of this as a std::function. it should then translate all the rest of the bullshit around it.

		//Basically, this is a wrapper for a given IFormula.


		R operator()(Args... args/*, Variable var*/)
		{
			//What should I do if someone tries to call this and doesn't have the right stuff?

			RuntimeVariable result = formula()->Call(std::forward<Args>(args)...);

			if constexpr (!std::is_same_v<void, R>) {
				return Unvariable<R>{}(result.Ptr());
			}
		}

		static Self Create(change_to_t<Args, std::string_view>... parameters, std::string_view routine, std::string_view from = "")
		{

			Self self;

			SignatureBase base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, R, StaticTargetTag, Args...>();

			if (processed)
			{

				std::vector<std::string_view> params{ parameters... };

				auto result = FormulaManager::instance->RequestFormula(base, params, routine, self, from);

				
			}
			else {
				logger::warn("signature not processed");
			}
			return self;
		}

	};

}