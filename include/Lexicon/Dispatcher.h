#pragma once

#include "Lexicon/Variable.h"
#include "Lexicon/Unvariable.h"
#include "Lexicon/Revariable.h"
#include "Lexicon/RuntimeVariable.h"

#include "Lexicon/Impl/ref_wrapper.h"

//*src
#include "Lexicon/IFunction.h"

namespace LEX
{

	struct ProcedureHandler;
	struct ProcedureData;

	//TODO: Dispatcher can 
	struct Dispatcher
	{
		friend ProcedureHandler;

		//This isn't really that important, the main use of it is to just keep track of all dispatchers. Instead though,
		// I might make a thing where you can switch out registered external functions similar to that of changing states.
		inline static std::vector<std::unique_ptr<Dispatcher>> _dispatchList{ 1 };

		static bool TryRegister(Dispatcher* dispatch, IFunction* func);

		virtual void Dispatch(RuntimeVariable& result, Variable* target, std::vector<Variable*>& args, ProcedureData& data) = 0;


	protected:
		template <typename T>
		static void* PullAddress(T& value)
		{
			if constexpr (std::derived_from<T, detail::wrapper_settings>)
			{
				//maybe I can make a function that does this bit manually
				return value.reference();
			}
			else
			{
				return std::addressof(value);
			}
		}

	protected:
		//A constant code that will serve as varification. Actually unsure what this does but I'll keep it in case I remember
		static constexpr uint64_t func_code = 1;

		bool registered = false;

	};




	template <class R, class T, class... Args>
	struct BasicDispatcher : public Dispatcher  //<R(Args...)>
	{
		using Self = BasicDispatcher<R, T, Args...>;
		using Signature = std::tuple<R, T, Args...>;
		using Function = R (*)(T, Args...);
	private:

		
	public:
		static bool Create(Function func, IFunction* dispatchee) 
		{
			std::unique_ptr<Dispatcher> test_dispatch = std::make_unique<Self>(func);

			bool result = TryRegister(test_dispatch.get(), dispatchee);

			if (result)  //If it's successful, we don't need to delete the dispatcher.
				test_dispatch.release();

			return result;
		}

		Function _callback = nullptr;


		template <typename GuideType, typename TupleType, std::size_t... Indices>
		auto tie_as_tuple(TupleType& t, std::index_sequence<Indices...>) {
			//std::forward_as_tuple<std::tuple_element_t<Indices, TupleType>...>
			return std::forward_as_tuple(std::forward<std::tuple_element_t<Indices, GuideType>>(std::get<Indices>(t))...);
		}

		template <typename... GuideTypes, typename TupleType>
		auto tie_as_tuple(TupleType& t) {
			return tie_as_tuple<std::tuple<GuideTypes...>>(t, std::make_index_sequence<std::tuple_size_v<TupleType>>{});
		}

		template<typename T>
		decltype(auto) HandleDispatch(T& tuple)
		{
			//If I could make this already forwarded I'd enjoy that
			if constexpr (std::is_same_v<R, void>) {
				std::apply(_callback, std::forward<decltype(tuple)>(tuple));
				return detail::not_implemented{};
			}
			else {
				return std::apply(_callback, std::forward<decltype(tuple)>(tuple));
			}
		}

		template<typename Ty>
		decltype(auto) LaunderDispatch(Ty& tuple)
		{
			auto ref_tuple = tie_as_tuple<T, Args...>(tuple);
			return HandleDispatch(ref_tuple);
		}





		//Need a copy of this called value export.
		template <class T1, size_t I = 1>
		static inline void ValueImport(T1& tuple, std::vector<Variable*>& args)
		{
			if constexpr (I < std::tuple_size_v<T1>) {
				using _Elem = std::tuple_element_t<I, T1>;

				std::get<I>(tuple) = Unvariable<_Elem>{}(args[I - 1]);

				//if constexpr (std::is_same_v<EntryType>) {
				//The actual version will test for the parameters.
				//}

				ValueImport<T1, I + 1>(tuple, args);
			}
		}
		template<typename Ty>
		auto ImportTarget(Variable* target)
		{
			if constexpr (std::same_as<StaticTargetTag, Ty>)
			{
				return StaticTargetTag{};
			}
			else {
				return Unvariable<Ty>{}(target);
			}
		}
		

		
		template<size_t T, size_t ... Ts>
		static constexpr size_t first = T;
		
		//I HEAVILY suspect this will not work as I wish.
		//Tuple first, and then a make index sequence to unpack it. Function should be inlined.
		template<typename Ty, size_t... Indices>
		auto ValueImport2(std::vector<Variable*>& args, std::index_sequence<Indices...>)
		{
			constexpr auto I = first<Indices...>;

			return Unvariable<std::tuple_element_t<I, Ty>>{}(args[I]);
		}


		/*
		template <typename Param>
		Param CreateArguments(Variable* target, std::vector<Variable*>& args) {
			std::index_sequence<Indices...>
			//std::forward_as_tuple<std::tuple_element_t<Indices, TupleType>...>
			return std::forward_as_tuple(std::forward<std::tuple_element_t<Indices, GuideType>>(std::get<Indices>(t))...);
		}
		//*/

		//Value input will be a direct set into the tuple

		struct exporter : public RefCollection
		{
			template <class TParam, class TResult, typename TArgs, size_t I = 0>
			void ValueExport(RuntimeVariable& out, TResult& result, TArgs& tuple, Variable* target, std::vector<Variable*>& args)
			{
				using RetElem = detail::simplify_wrapper_t<TResult>;

				constexpr auto k_ret_type = detail::reference_type_v<std::tuple_element_t<0, TParam>, true>;

				constexpr auto k_ignore_ret = std::is_same_v<detail::not_implemented, TResult>;

				constexpr auto _I = I;//For visibility in debugging


				if constexpr (I < std::tuple_size_v<TArgs>) {
					using Elem = std::tuple_element_t<I + 1, TParam>;
					using ElemArg = std::tuple_element_t<I, TArgs>;

					constexpr auto k_param_type = detail::reference_type_v<Elem, false>;


					//For each step, there's a simple question, is this a referencable type? First we check it to be one of the types plainly 
					// (this is genuinely more important), 
					// then we check if it's a reference. If either is true, we dereference the variable and stick it into that. If not no activation is handled here.


					if constexpr (!std::is_same_v<StaticTargetTag, Elem>)//This should use args
					{
						
						if constexpr (k_param_type != detail::ref_type::kNoRef)
						{
							Variable* arg;

							if constexpr (I)
								arg = args[I - 1];
							else
								arg = target;

							if (arg)
							{
								auto& entry = std::get<I>(tuple);

								//Needs to check for const before doing this.
								//TODO: Make this another move, I don't need what we got here.
								//arg->Assign(static_cast<detail::simplify_wrapper_t<ElemArg>&>(entry));
								
								Revariable<detail::simplify_wrapper_t<ElemArg>> revar{};
								revar(entry, arg);
								

								if constexpr (!k_ignore_ret && k_ret_type != detail::ref_type::kNoRef)
								{
									auto* en = PullAddress(entry);

									//This bit isn't required if we are ignoring the return type like the above
									Collect(en, arg);
									TryToCollect(revar);
								}
							}

						}
					}

					ValueExport<TParam, TResult, TArgs, I + 1>(out, result, tuple, target, args);
				}
				else if constexpr (!k_ignore_ret && I == std::tuple_size_v<TArgs>)
				{
					

					if constexpr (1)
					{
							if constexpr (k_ret_type != detail::ref_type::kNoRef)
							{
								bool local_check = 1;
								bool global_check = 1;

								void* ptr = PullAddress(result);

								//Do checks right here.

								if (ptr) {//This check first accounts for maybe ref btw, important.
									
									//I could do this above but I figure, it will keep collecting anyhow right? might as well check once.
									if (TryLoadReference(ptr, out) == false) {
										out = extern_ref(*reinterpret_cast<detail::simplify_wrapper_t<TResult>*>(ptr));
									}
									return;
								}
							}

							if constexpr (k_ret_type == detail::ref_type::kNoRef || k_ret_type == detail::ref_type::kMaybeRef)
							{
								static_assert(k_ret_type != detail::ref_type::kLocalRef);
								out = Variable{ static_cast<RetElem&>(result), GetVariableType<RetElem>() };
							}
					}

				}
			}
		};


		//*
		//If I can find a way to put the onus of this on the type perhaps? Would debugging be easier?
		template<size_t... Indices>
		inline void DispatchImpl(RuntimeVariable& out, Variable* target, std::vector<Variable*>& args, ProcedureData& data, std::index_sequence<Indices...>) 
		{
			//Concat this?
			using Param = std::tuple<detail::try_wrap_param_t<T>, detail::try_wrap_param_t<Args>...>;
			using TArgs = std::tuple<detail::try_wrap_param_t<Args>...>;


			constexpr size_t arg_size = std::tuple_size_v<std::tuple<Args...>>;

			if (auto list_size = args.size(); list_size != arg_size) {
				//Shit isn't the same fucking size I'm losing my mind.
				report::apply::error("Dispatch args and expected args do not match.");
			}

			Param tuple{ ImportTarget<detail::try_wrap_param_t<T>>(target), Unvariable<std::tuple_element_t<Indices, TArgs>>{}(args[Indices])...};

			decltype(auto) result = LaunderDispatch(tuple);

			exporter{}.ValueExport<Signature>(out, result, tuple, target, args);
		}
		

		void Dispatch(RuntimeVariable& out, Variable* target, std::vector<Variable*>& args, ProcedureData& data) override
		{
			return DispatchImpl(out, target, args, data, std::make_index_sequence<sizeof...(Args)>{});
		}
		/*/



		void Dispatch(RuntimeVariable& out, Variable* target, std::vector<Variable*>& args, ProcedureData& data) override
		{
			//Concat this?
			using Param = std::tuple<detail::try_wrap_param_t<T>, detail::try_wrap_param_t<Args>...>;
			using TArgs = std::tuple<detail::try_wrap_param_t<Args>...>;


			constexpr size_t arg_size = std::tuple_size_v<std::tuple<Args...>>;

			if (auto list_size = args.size(); list_size != arg_size) {
				//Shit isn't the same fucking size I'm losing my mind.
				report::apply::error("Dispatch args and expected args do not match.");
			}



			Param tuple{ ValueImport2<T>(target), ValueImport2<TArgs>(args, std::make_index_sequence<sizeof...(Args)>{}) };
			
			decltype(auto) result = LaunderDispatch(tuple);

			ValueExport<Signature>(out, result, tuple, target, args);

			//TODO: \/ This is supposed to be used on whatever executes this, rather whoever owns the RuntimeVariable
			//This is a resolving function, it should be done whenever a procedure is completed, to make sure that the return type is proper.
			
#ifdef DO_NOT_SHOW
			if (out.IsRefNegated())
			{
				logger::debug("triggered");
				void* ptr = out.Ptr();

				for (auto& rvar : args)
				{
					if (rvar.Ptr() == ptr)
					{
						if (rvar.IsReference() == false) {
							report::fault::critical("Non-reference argument referenced in return.");
						}

						out = rvar.AsRef();

						break;
					}
				}

			}


			for (auto& rvar : back_args) {
				rvar.TryUpdateRef();
			}
#endif
		}
		//*/


		void Dispatch_Old(RuntimeVariable& result, Variable* target, std::vector<Variable*>& args, ProcedureData& data)
		{
			//Unload that shit.
			//using Arg1 = std::tuple_element_t<0, std::tuple<Args...>>;

			constexpr size_t arg_size = std::tuple_size_v<std::tuple<Args...>>;

			if (auto list_size = args.size(); list_size != arg_size) {
				//Shit isn't the same fucking size I'm losing my mind.
				report::apply::error("Dispatch args and expected args do not match.");
			}

			//typename function_traits<std::remove_pointer_t<decltype(T)>>::arguments args;

			//I'll want to remove the references due to being unable to load them like this.
			std::tuple<T, std::remove_reference_t<Args>...> tuple;
			//static_assert(std::is_same_v<std::tuple<Target, int, int, int>,
			//	function_traits<std::remove_pointer_t<decltype(T)>>::arguments>, "GN  NE NIP");

			if constexpr (!std::is_same_v<T, StaticTargetTag>)
			{
				std::get<0>(tuple) = Unvariable<T>{}(target);
			}

			ValueImport(tuple, args);
			//Here we get the return type and switch up on it.

			//Confirm if the types are correct.

			if constexpr (std::is_same_v<R, void>) {
				std::apply(_callback, tuple);
			} else {
				R to_result = std::apply(_callback, tuple);

				//under more normal situations, vary this by whether it's a ref or not.

				result = Variable{ to_result, GetVariableType<R>() };
			}
		}

		constexpr BasicDispatcher(Function func) :
			_callback{ func }
		{
		}
	};

}