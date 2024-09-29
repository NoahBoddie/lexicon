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

	
	namespace detail
	{

		//These are useful enough that they could be moved.
		template <typename T, typename = void>
		struct expected_var_type;


		template <typename T> requires (!detail::function_has_var_type<T> && !detail::function_has_var_type<T*>)
		struct expected_var_type<T> { using type = void; };


		template <typename T> requires (detail::function_has_var_type<T>)
		struct expected_var_type<T> { using type = T; };
		//*/
		template <typename T> requires (detail::function_has_var_type<T*>)
		struct expected_var_type<T> { using type = T*; };

		template <typename T>
		using expected_var_type_t = expected_var_type<T>::type;
	}

	void Test()
	{

	}

	template <typename R, typename T, typename... Args>
	struct Formula<R(T::*)(Args...)> : public FormulaHandler
	{
		using Self = Formula<R(T::*)(Args...)>;

		using TarType = std::conditional_t<std::is_pointer_v<detail::expected_var_type_t<T>>,
			detail::expected_var_type_t<T>, detail::expected_var_type_t<T>&>;

		//The idea of this is you cast an IFormula into this forcibly, and this type will then manage all of the function calls for the type.
		// Basically, think of this as a std::function. it should then translate all the rest of the bullshit around it.

		//Basically, this is a wrapper for a given IFormula.

		R Call(TarType tar, Args&&... args/*, Variable var*/)
		{
			//What should I do if someone tries to call this and doesn't have the right stuff?

			RuntimeVariable result = formula()->Call(std::forward<TarType>(tar), std::forward<Args>(args)...);

			if constexpr (!std::is_same_v<void, R>) {
				return Unvariable<R>{}(result.Ptr());
			}
		}


		R operator()(TarType tar, Args... args/*, Variable var*/)
		{
			return Call(tar, std::forward<Args>(args)...);
		}

		template <typename T = int>
		R operator()(TarType&& tar, Args... args/*, Variable var*/, typename std::enable_if_t<!std::is_pointer_v<TarType>, T>* = 0)
		{
			return Call(tar, std::forward<Args>(args)...);
		}

		//This version should have a special operator where using -> will yield a helper class that will be able to be called in order to handle the function
		// So something like formula(target)->Call();  or formula(target)(); Or, I'll just allow the target to be one with the calls. Seems better that way.

		static Self Create(change_to_t<Args, std::string_view>... parameters, std::string_view routine, std::string_view from = "")
		{

			Self self;

			SignatureBase base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, R, detail::expected_var_type_t<T>, Args...>();

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



	struct Class
	{
		void Function(int) {}
	};

	void trest()
	{
		using func = void(int);
		using Func = decltype(Class::Function);
	}
}