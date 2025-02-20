#pragma once

#include "Lexicon/Variable.h"
#include "Lexicon/Unvariable.h"
#include "Lexicon/RuntimeVariable.h"


//*src
#include "Lexicon/IFunction.h"

namespace LEX
{

	struct ProcedureHandler;
	struct ProcedureData;

	struct Dispatcher
	{
		friend ProcedureHandler;

		//This isn't really that important, the main use of it is to just keep track of all dispatchers. Instead though,
		// I might make a thing where you can switch out registered external functions similar to that of changing states.
		inline static std::vector<std::unique_ptr<Dispatcher>> _dispatchList{ 1 };

		static bool TryRegister(Dispatcher* dispatch, IFunction* func);

		virtual void Dispatch(RuntimeVariable& result, Variable* target, std::vector<Variable*>& args, ProcedureData& data) = 0;

	protected:
		//A constant code that will serve as varification. Actually unsure what this does but I'll keep it in case I remember
		static constexpr uint64_t func_code = 1;

		bool registered = false;

	};

	template <class R, class T, class... Args>
	struct BasicDispatcher : public Dispatcher  //<R(Args...)>
	{
		using Self = BasicDispatcher<R, T, Args...>;
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

		void Dispatch(RuntimeVariable& result, Variable* target, std::vector<Variable*>& args, ProcedureData& data) override
		{
			//Unload that shit.
			//using Arg1 = std::tuple_element_t<0, std::tuple<Args...>>;

			constexpr size_t arg_size = std::tuple_size_v<std::tuple<Args...>>;

			if (auto list_size = args.size(); list_size != arg_size) {
				//Shit isn't the same fucking size I'm losing my mind.
				throw nullptr;
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