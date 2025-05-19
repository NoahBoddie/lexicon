#pragma once


#include "Lexicon/IFormula.h"
#include "Lexicon/VariableType.h"
#include "Lexicon/FormulaHandler.h"

#include "Lexicon/Impl/ref_wrapper.h"

#include "Lexicon/Interfaces/FormulaManager.h"

namespace LEX
{
	struct IScript;



	template <typename T>// requires(detail::reference_type_v<T, true> == detail::kNoRef)
	struct Formula;
	
	//I want a good way to prevent these from having a reference return type.

	/*
	template <typename T>
	concept is_ref = std::is_reference_v<T>;

	template <is_ref T>
	struct Formula<T> {};
	//*/
	
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

		using Ty = std::conditional_t<std::is_void_v<T>, Void, T>;

	public:

		//This sorta needs to be able to make sure that the item can unvariable itself.


	private:
		static T RunImpl(std::string_view routine, std::optional<IScript*> from, std::optional<Ty> def)
		{

			FormulaHandler self;

			ISignature base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, T, StaticTargetTag>();

			uint64_t temp_cmp_res = 1;

			if (processed)
			{

				auto result = FormulaManager::instance->RequestFormula(base, {}, routine, self, from);

				//Send out a message here.
				temp_cmp_res = result;

			}

			if (temp_cmp_res) {
				
				bool def_value = def.has_value();

				
				report::log("Error occured creating formula '{}'. Error {}.", std::source_location::current(), 
					IssueType::Apply, def_value ? IssueLevel::Failure : IssueLevel::Error,
					routine, temp_cmp_res);

				if (def_value) {
					if constexpr (std::is_void_v<T>) {
						return;
					}
					else {
						return def.value();
					}
				}
					

			}

			
			if (self) {
				//For now, this shouldn't catch.
				RuntimeVariable result = self.formula()->Call();
				
				if constexpr (!std::is_same_v<void, T>) {
					return Unvariable<T>{}(result.Ptr());
				}
			}

			//Send an exception. probably.
			if constexpr (std::is_void_v<T>) {
				return;
			}
			else {
				return {};
			}

		}

	public:
		static T Run(std::string_view routine)
		{
			return RunImpl(routine, std::nullopt, std::nullopt);
		}

		//T needs to be able to be inited by default
		static T RunDefault(std::string_view routine)
		{
			return RunImpl(routine, std::nullopt, Ty{});
		}



		static T Run(std::string_view routine, Ty def)
		{
			return RunImpl(routine, std::nullopt, def);
		}

		static T Run(std::string_view routine, IScript* from)
		{
			return RunImpl(routine, from, std::nullopt);
		}

		static T Run(std::string_view routine, IScript* from, Ty def)
		{
			return RunImpl(routine, from, def);
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


		using Ry = std::conditional_t<std::is_void_v<R>, Void, R>;


		//The idea of this is you cast an IFormula into this forcibly, and this type will then manage all of the function calls for the type.
		// Basically, think of this as a std::function. it should then translate all the rest of the bullshit around it.

		//Basically, this is a wrapper for a given IFormula.


		R operator()(Args... args, std::optional<Ry> def = std::nullopt)
		{
			//What should I do if someone tries to call this and doesn't have the right stuff?
			if (formula() == nullptr)
			{
				bool def_value = def.has_value();


				report::log("Formula is null cannot call function.", std::source_location::current(), 
					IssueType::Apply, def_value ? IssueLevel::Failure : IssueLevel::Error);


				if (def_value) {
					if constexpr (std::is_void_v<R>) {
						return;
					}
					else {
						return def.value();
					}
				}
			}


			RuntimeVariable result = formula()->Call(std::forward<Args>(args)...);

			if constexpr (!std::is_same_v<void, R>) {
				return Unvariable<R>{}(result.Ptr());
			}
		}

		static Self Create(change_to_t<Args, std::string_view>... parameters, std::string_view routine, std::optional<IScript*> from = std::nullopt)
		{
			Self self;

			ISignature base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, R, StaticTargetTag, Args...>();

			if (processed)
			{
				std::vector<std::string_view> params{ parameters... };

				auto result = FormulaManager::instance->RequestFormula(base, params, routine, self, from);

				if (result) {
					report::failure("Formula '{}' failed to resolve. Error value {}", routine, result);
				}
			}
			else {
				report::failure("signature not processed");
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


	//TODO: This needs to be supported.
	//using TEVE = void(std::string::*)() const;

	template <typename R, typename T, typename... Args>
	struct Formula<R(T::*)(Args...)> : public FormulaHandler
	{
		using Self = Formula<R(T::*)(Args...)>;

		using Type = detail::expected_var_type_t<T>;

		using TarType = std::conditional_t<std::is_pointer_v<Type>,Type, Type&>;

		using Ry = std::conditional_t<std::is_void_v<R>, Void, R>;

		//The concept of this is, if you have a type that's a pointer
		// it will use the pointer helper, which then needs to make the this helper to call on.
		// if you aren't using a pointer, it will give you the this helper directly

		struct HelperBase
		{
		protected:
			friend class Self;

			HelperBase(TarType tar, IFormula* form) : target{ tar }, call_unit{ form } {}

			//Prevent these from being transfered


		
			IFormula* call_unit = nullptr;
			TarType target;
		};


		struct ThisHelper : public HelperBase
		{
			using HelperBase::HelperBase;
		
			void* operator->() = delete;

			R Call(Args&&... args, std::optional<Ry> def = std::nullopt)
			{
				//What should I do if someone tries to call this and doesn't have the right stuff?
				if (!this->call_unit)
				{
					bool def_value = def.has_value();


					report::log("Formula is null cannot call function.", std::source_location::current(),
						IssueType::Apply, def_value ? IssueLevel::Failure : IssueLevel::Error);


					if (def_value) {
						if constexpr (std::is_void_v<R>) {
							return;
						}
						else {
							return def.value();
						}
					}
				}


				RuntimeVariable result = this->call_unit->Call(std::forward<TarType>(this->target), std::forward<Args>(args)...);

				if constexpr (!std::is_same_v<void, R>) {
					return Unvariable<R>{}(result.Ptr());
				}
			}

		};

		struct PointerHelper : public HelperBase
		{
			using HelperBase::HelperBase;

			ThisHelper* operator->()
			{
				return reinterpret_cast<ThisHelper*>(this);
			}
		};




		//The idea of this is you cast an IFormula into this forcibly, and this type will then manage all of the function calls for the type.
		// Basically, think of this as a std::function. it should then translate all the rest of the bullshit around it.

		//Basically, this is a wrapper for a given IFormula.
		
		/*
		R Call(TarType tar, Args&&... args, std::optional<Ry> def = std::nullopt)
		{
			//What should I do if someone tries to call this and doesn't have the right stuff?

			if (formula() == nullptr)
			{
				bool def_value = def.has_value();


				report::log("Formula is null cannot call function.", std::source_location::current(), 
					IssueType::Apply, def_value ? IssueLevel::Failure : IssueLevel::Error);


				if (def_value) {
					if constexpr (std::is_void_v<R>) {
						return;
					}
					else {
						return def.value();
					}
				}
			}


			RuntimeVariable result = formula()->Call(std::forward<TarType>(tar), std::forward<Args>(args)...);

			if constexpr (!std::is_same_v<void, R>) {
				return Unvariable<R>{}(result.Ptr());
			}
		}


		R operator()(TarType tar, Args... args, std::optional<T> def = std::nullopt)
		{
			return Call(tar, std::forward<Args>(args)...);
		}

		//TODO: Use requires instead of enable_if.
		template <typename = void>
		R operator()(TarType&& tar, Args... args, std::optional<T> def = std::nullopt, typename std::enable_if_t<!std::is_pointer_v<TarType>, T>* = 0)
		{
			return Call(tar, std::forward<Args>(args)...);
		}
		//*/

	private:
		auto GetHelper(TarType target)
		{
			if constexpr (std::is_pointer_v<TarType>) {
				return PointerHelper{ target, formula() };
			}
			else {
				return ThisHelper{ target, formula() };
			}
		}
	
public:

		auto operator()(TarType target)
		{
			return GetHelper(target);
		}

		auto operator()(Type&& target) requires(!std::is_pointer_v<TarType>)
		{
			return GetHelper(target);
		}


		//This version should have a special operator where using -> will yield a helper class that will be able to be called in order to handle the function
		// So something like formula(target)->Call();  or formula(target)(); Or, I'll just allow the target to be one with the calls. Seems better that way.

		static Self Create(change_to_t<Args, std::string_view>... parameters, std::string_view routine, std::optional<IScript*> from = std::nullopt)
		{

			Self self;

			ISignature base{};

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