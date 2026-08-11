#pragma once

#include "Lexicon/IFormula.h"
#include "Lexicon/VariableType.h"
#include "Lexicon/SignatureBase.h"
#include "Lexicon/FormulaHandler.h"
#include "Lexicon/Impl/ref_wrapper.h"

#include "Lexicon/Interfaces/IScript.h"
#include "Lexicon/Interfaces/FormulaManager.h"
#include "Lexicon/Interfaces/ProjectManager.h"

#ifdef LEX_SOURCE
#include "Lexicon/Engine/SyntaxRecord.h"
#endif

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





	//Note that if it's a const pointer (like this), it will check if the to type is a pointer, otherwise it will just be a const.

	struct FormulaParam
	{
		FormulaParam() = default;

		FormulaParam(const std::string_view& path) : value{ path } {}
		FormulaParam(const std::string& path) : value{ std::string_view{path} } {}

		FormulaParam(const char* path) : FormulaParam{ std::string_view{path} } {}

		FormulaParam(TypeInfo* type) : value{ type } {}

		TypeInfo* GetType(IScript* script)
		{
			if (script)
			{
				switch (value.index())
				{
				case variant_index<decltype(value), std::string_view>():
					value = script->GetTypeFromPath(std::get<std::string_view>(value))->As<TypeInfo>();
					[[fallthrough]];
				case variant_index<decltype(value), TypeInfo*>():
					return std::get<TypeInfo*>(value);
				}
			}

			return nullptr;
		}

		std::variant<TypeInfo*, std::string_view> value;
	};


	namespace detail
	{
		template <typename T1, typename T2>
		concept same_as_extracted = std::same_as<std::remove_cvref_t<T1>, std::remove_cvref_t<T2>>;


		//If it's a runtime type, it requires 2 strings, one for type, and the other for the name.
		template <typename T>
		using param_view_t = std::conditional_t<std::is_same_v<std::remove_cvref_t<T>, runtime_type>,
			std::pair<FormulaParam, std::string_view>, std::string_view>;

		template<typename T, typename To = Variable>
		using remove_runtype_t = std::conditional_t<std::is_same_v<std::remove_cvref_t<T>, runtime_type>, inherit_qualifier_t<To, T>, T>;


		template<typename Handler, typename R, typename T, typename... Args>//This gets owned by the function btw.
		struct FormulaBaseImpl
		{
			static constexpr bool uses_runtime = ((std::is_same_v<std::remove_cvref_t<Args>, runtime_type>) || ...);

		private:

			static auto get_view(const auto& v) -> std::string_view {
				if constexpr (std::is_same_v<std::remove_cvref_t<decltype(v)>, std::string_view>) {
					return v;
				}
				else {
					return v.second;
				}
			}


			static bool PrepSignature(SignatureBase& base, FormulaParam ret_type, FormulaParam tar_type, param_view_t<Args>... parameters, std::optional<IScript*>& from = std::nullopt)
			{
				bool processed = base.Fill<SignatureEnum::Result, R, T, Args...>();

				//Here have one that checks for R to have the runtime_type
				//Here have a T check
				//And here attempt to use a fold expression to fish out if there's one.

				if (processed) {

					IScript* script = from && from.value() ? from.value() : ProjectManager::instance->GetShared()->GetCommons();

					if constexpr (std::is_same_v<std::remove_cvref_t<R>, runtime_type>) {
						processed = base.SignatureBase::result.policy = ret_type.GetType(script);
						if (!processed) return false;
					}

					if constexpr (std::is_same_v<std::remove_cvref_t<T>, runtime_type>) {
						processed = base.SignatureBase::target.policy = tar_type.GetType(script);
						if (!processed) return false;
					}
					//use uses_runtime
					if constexpr (sizeof...(Args) && ((std::is_same_v<std::remove_cvref_t<Args>, runtime_type>) || ...))
					{
						size_t index = 0;

						auto& params = base.SignatureBase::parameters;

						//processed = ((params[i++].policy = std::is_same_v<std::remove_cvref_t<Args>, runtime_type> ?
						//	script->GetTypeFromPath(get_view(parameters, true)) : params[i - 1].policy) && ...);

						processed = ([&]<typename T> [[msvc::forceinline]] -> bool
						{
							auto i = index++;
							if constexpr (std::is_same_v<std::remove_cvref_t<Args>, runtime_type>) {
								return params[i].policy = parameters.first.GetType(script);
							}
							else {
								return true;
							}

						}.operator() < Args > () && ...);
					}

					return processed;
				}

			}

			static Handler CompileFormula(ISignature& base, param_view_t<Args>... parameters, std::string_view routine,
				std::optional<IScript*> from = std::nullopt, const std::source_location& loc = std::source_location::current())
			{


				Handler self;

				std::vector<std::string_view> params{ get_view(parameters)... };

				auto result = FormulaManager::instance->RequestFormula(base, params, routine, self, from, loc);

				if (result) {
					report::failure("Formula '{}' failed to resolve. Error value {}", loc, routine, result);
				}

				return std::move(self);
			}
#ifdef LEX_SOURCE
			static Handler CompileFormula(ISignature& base, param_view_t<Args>... parameters, std::string_view name, SyntaxRecord& ast,
				std::optional<IScript*> from = std::nullopt, const std::source_location& loc = std::source_location::current())
			{
				Handler self;

				std::vector<std::string_view> params{ get_view(parameters)... };

				auto result = FormulaManager::instance->RequestFormulaFromRecord(base, params, name, ast, self, from, loc);

				if (result) {
					report::failure("Formula '{}' failed to resolve. Error value {}", loc, name, result);
				}

				return std::move(self);
			}
#endif
		protected:

			static Handler CreateImpl(FormulaParam ret_type, FormulaParam tar_type, param_view_t<Args>... parameters,
				std::string_view routine, std::optional<IScript*> from = std::nullopt,
				const std::source_location& loc = std::source_location::current())
			{
				ISignature base;

				if (PrepSignature(base, ret_type, tar_type, parameters..., from) == false) {
					report::failure("signature not processed", loc);
					return {};
				}

				return CompileFormula(base, parameters..., routine, from, loc);
			}


#ifdef LEX_SOURCE

			static Handler CreateImpl(FormulaParam ret_type, FormulaParam tar_type, param_view_t<Args>... parameters,
				std::string_view name, SyntaxRecord& ast, std::optional<IScript*> from = std::nullopt,
				const std::source_location& loc = std::source_location::current())
			{
				ISignature base;

				if (PrepSignature(base, ret_type, tar_type, parameters..., from) == false) {
					report::failure("signature not processed", loc);
					return {};
				}

				return CompileFormula(base, parameters..., name, ast, from, loc);
			}

#endif


		};


		template<typename Handler, typename R, typename T, typename... Args>//This gets owned by the function btw.
		struct FormulaBase : private FormulaBaseImpl<Handler, R, T, Args...>
		{
			using Base = FormulaBaseImpl<Handler, R, T, Args...>;
			using Self = FormulaBase<Handler, R, T, Args...>;

			static Handler Create(param_view_t<Args>... parameters,
				std::string_view routine, std::optional<IScript*> from = std::nullopt,
				const std::source_location& loc = std::source_location::current()
			)
			{
				return Base::CreateImpl({}, {}, parameters..., routine, from, loc);
			}
#ifdef LEX_SOURCE
			static Handler Create(param_view_t<Args>... parameters,
				std::string_view name, SyntaxRecord& ast, std::optional<IScript*> from = std::nullopt,
				const std::source_location& loc = std::source_location::current()
			)
			{
				return Base::CreateImpl({}, {}, parameters..., name, ast, from, loc);
			}
#endif
		};

		template<typename Handler, stl::same_as_extracted<runtime_type> R, typename T, typename... Args>//This gets owned by the function btw.
		struct FormulaBase<Handler, R, T, Args...> : private FormulaBaseImpl<Handler, R, T, Args...>
		{
			static constexpr bool uses_runtime = true;

			using Base = FormulaBaseImpl<Handler, R, T, Args...>;
			using Self = FormulaBase<Handler, R, T, Args...>;

			static Handler Create(FormulaParam return_type, param_view_t<Args>... parameters,
				std::string_view routine, std::optional<IScript*> from = std::nullopt,
				const std::source_location& loc = std::source_location::current()
			)
			{
				return Base::CreateImpl(return_type, {}, parameters..., routine, from, loc);
			}

#ifdef LEX_SOURCE
			static Handler Create(FormulaParam return_type, param_view_t<Args>... parameters,
				std::string_view name, SyntaxRecord& ast, std::optional<IScript*> from = std::nullopt,
				const std::source_location& loc = std::source_location::current()
			)
			{
				return Base::CreateImpl(return_type, {}, parameters..., name, ast, from, loc);
			}
#endif
		};

		template<typename Handler, typename R, stl::same_as_extracted<runtime_type> T, typename... Args>//This gets owned by the function btw.
		struct FormulaBase<Handler, R, T, Args...> : private FormulaBaseImpl<Handler, R, T, Args...>
		{
			static constexpr bool uses_runtime = true;

			using Base = FormulaBaseImpl<Handler, R, T, Args...>;
			using Self = FormulaBase<Handler, R, T, Args...>;

			static Handler Create(FormulaParam target_type, param_view_t<Args>... parameters,
				std::string_view routine, std::optional<IScript*> from = std::nullopt,
				const std::source_location& loc = std::source_location::current()
			)
			{
				return Base::CreateImpl({}, target_type, parameters..., routine, from, loc);
			}

#ifdef LEX_SOURCE
			static Handler Create(FormulaParam target_type, param_view_t<Args>... parameters,
				std::string_view name, SyntaxRecord& ast, std::optional<IScript*> from = std::nullopt,
				const std::source_location& loc = std::source_location::current()
			)
			{
				return Base::CreateImpl({}, target_type, parameters..., name, ast, from, loc);
			}
#endif
		};

		template<typename Handler, stl::same_as_extracted<runtime_type> R, stl::same_as_extracted<runtime_type> T, typename... Args>//This gets owned by the function btw.
		struct FormulaBase<Handler, R, T, Args...> : private FormulaBaseImpl<Handler, R, T, Args...>
		{
			static constexpr bool uses_runtime = true;

			using Base = FormulaBaseImpl<Handler, R, T, Args...>;
			using Self = FormulaBase<Handler, R, T, Args...>;
			static Handler Create(FormulaParam return_type, FormulaParam target_type, param_view_t<Args>... parameters,
				std::string_view routine, std::optional<IScript*> from = std::nullopt,
				const std::source_location& loc = std::source_location::current()
			)
			{
				return Base::CreateImpl(return_type, target_type, parameters..., routine, from, loc);
			}

#ifdef LEX_SOURCE
			static Handler Create(FormulaParam return_type, FormulaParam target_type, param_view_t<Args>... parameters,
				std::string_view name, SyntaxRecord& ast, std::optional<IScript*> from = std::nullopt,
				const std::source_location& loc = std::source_location::current()
			)
			{
				return Base::CreateImpl(return_type, target_type, parameters..., name, ast, from, loc);
			}
#endif
		};


	}





	//make summary later.
	template <detail::function_has_var_type T>
	struct Formula<T> : private detail::FormulaBase<FormulaHandler, T, StaticTargetTag>
	{
		//TODO: Formula immediate return doesn't use runtime types, I'd like it to however.

		//This version of formula is completely removed from play
	private:
		using Base = detail::FormulaBase<FormulaHandler, T, StaticTargetTag>;

		Formula() = delete;
		Formula(const Formula&) = delete;
		Formula(Formula&&) = delete;
		Formula& operator=(const Formula&) = delete;
		Formula& operator=(Formula&&) = delete;

		using Ty = std::conditional_t<std::is_void_v<T>, Void, T>;

	public:

		//This sorta needs to be able to make sure that the item can unvariable itself.


	private:

		static T CallImpl(FormulaHandler& handler, std::optional<Ty>& def)
		{
			if (!handler) {
				if constexpr (std::is_void_v<T>) {
					return;
				}
				else {
					if (def.has_value()) {
						return def.value();
					}
					else {
						throw Error("Default value not given for formula execution");
					}
				}
			}

			RuntimeVariable result = handler.formula()->Call();

			if constexpr (!std::is_same_v<void, T>) {
				return Unvariable<T>{}(result.Ptr());
			}
		}


		static T RunImpl(std::string_view routine, std::optional<IScript*> from, std::optional<Ty> def, const std::source_location& loc)
		{
			FormulaHandler handler = Base::Create(routine, from, loc);

			return CallImpl(handler, def);
		}

#ifdef LEX_SOURCE
		static T RunImpl(const std::string_view& name, SyntaxRecord& record ,std::optional<IScript*> from, std::optional<Ty> def, const std::source_location& loc)
		{
			FormulaHandler handler = Base::Create(name, record, from, loc);

			return CallImpl(handler, def);
		}
#endif


	public:
		//T needs to be able to be inited by default
		static T RunDefault(std::string_view routine, const std::source_location& loc = std::source_location::current())
		{
			return RunImpl(routine, std::nullopt, Ty{}, loc);
		}


		static T Run(std::string_view routine, const std::source_location& loc = std::source_location::current())
		{
			return RunImpl(routine, std::nullopt, std::nullopt, loc);
		}

		static T Run(std::string_view routine, Ty def, const std::source_location& loc = std::source_location::current())
		{
			return RunImpl(routine, std::nullopt, def, loc);
		}

		static T Run(std::string_view routine, IScript* from, const std::source_location& loc = std::source_location::current())
		{
			return RunImpl(routine, from, std::nullopt, loc);
		}

		static T Run(std::string_view routine, IScript* from, Ty def, const std::source_location& loc = std::source_location::current())
		{
			return RunImpl(routine, from, def, loc);
		}


#ifdef LEX_SOURCE
		static T RunDefault(std::string_view name, SyntaxRecord& routine, const std::source_location& loc = std::source_location::current())
		{
			return RunImpl(name, routine, std::nullopt, Ty{}, loc);
		}


		static T Run(std::string_view name, SyntaxRecord& routine, const std::source_location& loc = std::source_location::current())
		{
			return RunImpl(name, routine, std::nullopt, std::nullopt, loc);
		}

		static T Run(std::string_view name, SyntaxRecord& routine, Ty def, const std::source_location& loc = std::source_location::current())
		{
			return RunImpl(name, routine, std::nullopt, def, loc);
		}

		static T Run(std::string_view name, SyntaxRecord& routine, IScript* from, const std::source_location& loc = std::source_location::current())
		{
			return RunImpl(name, routine, from, std::nullopt, loc);
		}

		static T Run(std::string_view name, SyntaxRecord& routine, IScript* from, Ty def, const std::source_location& loc = std::source_location::current())
		{
			return RunImpl(name, routine, from, def, loc);
		}
#endif
	};



	template <typename R, typename... Args>
	struct Formula<R(Args...)> :
		public detail::FormulaBase<Formula<detail::remove_runtype_t<R, Voidable>(detail::remove_runtype_t<Args>...)>, R, StaticTargetTag, Args...>,
		public FormulaHandler
	{
		using Ret = detail::remove_runtype_t<R, Voidable>;
		using Self = Formula<Ret(detail::remove_runtype_t<Args>...)>;
		using Base = detail::FormulaBase<Self, R, StaticTargetTag, Args...>;

		//operator Self() { return *reinterpret_cast<Self*>(this); }
		//operator const Self() const { return *reinterpret_cast<const Self*>(this); }

		constexpr Formula() noexcept = default;
		inline Formula& operator=(const Self& self) { FormulaHandler::operator=(self); return *this; }
		inline Formula& operator=(Self&& self) { FormulaHandler::operator=(std::move(self)); return *this; }
		Formula(const Self& self) : FormulaHandler{ self } {}
		Formula(Self&& self) : FormulaHandler{ std::move(self) } {}


		using Ry = std::conditional_t<std::is_void_v<R>, Void, detail::remove_runtype_t<R, Voidable>>;


		//The idea of this is you cast an IFormula into this forcibly, and this type will then manage all of the function calls for the type.
		// Basically, think of this as a std::function. it should then translate all the rest of the bullshit around it.

		//Basically, this is a wrapper for a given IFormula.


		detail::remove_runtype_t<R, Voidable> operator()(detail::remove_runtype_t<Args>... args, std::optional<Ry> def = std::nullopt)
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


			RuntimeVariable result = formula()->Call(std::forward<detail::remove_runtype_t<Args>>(args)...);

			if constexpr (!std::is_same_v<void, R>) {
				return Unvariable<Ret>{}(result.Ptr());
			}
		}
		/*
		static Self Create(change_to_t<Args, std::string_view>... parameters, std::string_view routine, std::optional<IScript*> from = std::nullopt,
			const std::source_location& loc = std::source_location::current())
		{
			Self self;

			ISignature base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, R, StaticTargetTag, Args...>();

			if (processed)
			{
				std::vector<std::string_view> params{ parameters... };

				auto result = FormulaManager::instance->RequestFormula(base, params, routine, self, from, loc);

				if (result) {
					report::failure("Formula '{}' failed to resolve. Error value {}", routine, result);
				}
			}
			else {
				report::failure("signature not processed");
			}
			return self;
		}
		//*/
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
	struct Formula<R(T::*)(Args...)> :
		public detail::FormulaBase<Formula<detail::remove_runtype_t<R, Voidable>(detail::remove_runtype_t<T>::*)(detail::remove_runtype_t<Args>...)>,
		R, detail::expected_var_type_t<T>, Args...>,
		public FormulaHandler
	{
		using Ret = detail::remove_runtype_t<R, Voidable>;
		using Target = detail::expected_var_type_t<detail::remove_runtype_t<T>>;

		using TrueSelf = Formula<R(T::*)(Args...)>;
		using Self = Formula<Ret(detail::remove_runtype_t<T>::*)(detail::remove_runtype_t<Args>...)>;

		using Base = detail::FormulaBase<Self, R, detail::expected_var_type_t<T>, Args...>;


		using TarType = std::conditional_t<std::is_pointer_v<Target>, Target, Target&>;

		using Ry = std::conditional_t<std::is_void_v<R>, Void, Ret>;

		//operator Self() { return *reinterpret_cast<Self*>(this); }
		//operator const Self() const { return *reinterpret_cast<const Self*>(this); }

		constexpr Formula() noexcept = default;

		inline Formula& operator=(const Self& self) { FormulaHandler::operator=(self); return *this; }
		inline Formula& operator=(Self&& self) { FormulaHandler::operator=(std::move(self)); return *this; }
		Formula(const Self& self) : FormulaHandler{ self } {}
		Formula(Self&& self) : FormulaHandler{ std::move(self) } {}

		//The concept of this is, if you have a type that's a pointer
		// it will use the pointer helper, which then needs to make the this helper to call on.
		// if you aren't using a pointer, it will give you the this helper directly

		struct HelperBase
		{
		protected:
			friend class Self;
			friend class TrueSelf;

			HelperBase(TarType tar, IFormula* form) : target{ tar }, call_unit{ form } {}

			//Prevent these from being transfered



			IFormula* call_unit = nullptr;
			TarType target;
		};


		struct ThisHelper : public HelperBase
		{
			using HelperBase::HelperBase;

			void* operator->() = delete;

			Ret Call(detail::remove_runtype_t<Args>... args, std::optional<Ry> def = std::nullopt)
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


				RuntimeVariable result = this->call_unit->Call(std::forward<TarType>(this->target), std::forward<detail::remove_runtype_t<Args>>(args)...);

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

		auto operator()(Target&& target) requires(!std::is_pointer_v<TarType>)
		{
			return GetHelper(target);
		}


		//This version should have a special operator where using -> will yield a helper class that will be able to be called in order to handle the function
		// So something like formula(target)->Call();  or formula(target)(); Or, I'll just allow the target to be one with the calls. Seems better that way.

		/*
		static Self Create(change_to_t<Args, std::string_view>... parameters, std::string_view routine, std::optional<IScript*> from = std::nullopt,
			const std::source_location& loc = std::source_location::current())
		{

			Self self;

			ISignature base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, R, detail::expected_var_type_t<T>, Args...>();

			if (processed)
			{

				std::vector<std::string_view> params{ parameters... };

				auto result = FormulaManager::instance->RequestFormula(base, params, routine, self, from, loc);


			}
			else {
				logger::warn("signature not processed");
			}
			return self;
		}
		//*/
	};

	struct DynamicFormula : public FormulaHandler
	{
		using FormulaHandler::FormulaHandler;
		using FormulaHandler::operator=;

		template <specialization_of<Formula> F>
		F& As() noexcept { return *reinterpret_cast<F*>(this); }
		
		template <specialization_of<Formula> F>
		const F& As() const noexcept { return *reinterpret_cast<F*>(this); }
	};
}