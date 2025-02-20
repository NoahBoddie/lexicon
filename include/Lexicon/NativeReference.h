#pragma once

#include "Lexicon/Variable.h"
#include "Lexicon/Unvariable.h"

namespace LEX
{

	struct IReferenceHandler
	{
		/// <summary>
		/// 
		/// </summary>
		/// <param name="proxy"></param>
		/// <param name="dest"></param>
		/// <param name="input">If true assigns data from proxy to the target, if false vice versa</param>
		virtual void Assign(Variable& proxy, void* target, bool input) const = 0;

	};

	template <typename T>
	struct ReferenceHandler : public IReferenceHandler
	{
		static ReferenceHandler<T>* GetSingleton()
		{
			static ReferenceHandler<T> singleton;
			return std::addressof(singleton);
		}


		void Assign(Variable& proxy, void* target, bool input) const override
		{
			T& dest = *reinterpret_cast<T*>(target);

			if (input) {
				dest = Unvariable<T>{}(std::addressof(proxy));
			}
			else {
				proxy = MakeVariable(dest);
			}
		}


	};




	//Native reference is a type that handles
	struct NativeReference
	{
		//Might ref wrap this
		IReferenceHandler* handler = nullptr;

		void* target;

		//This should never change, unless the new target does too.
		std::shared_ptr<Variable> proxy = std::make_shared<Variable>();

		void Update(bool in)
		{
			handler->Assign(Ref(), target, in);

		}


		Variable* Ptr() { return proxy.get(); }
		const Variable* Ptr() const { return proxy.get(); }

		Variable& Ref() { return *proxy; }
		const Variable& Ref() const { return *proxy; }

		void Test()
		{
			auto test = Unvariable<int>{}(&Ref());
		}

		//I'm thinking that this only updates at the end, or when someone specifies they want to update it some how.

		//Type must both be able to use unvariable and have be transferable into being a variable.
		template<Assignable<VariableComponent> T> requires(!std::derived_from<Unvariable<T>, detail::not_implemented>)
		explicit NativeReference(T& tar) :
			target{ std::addressof(tar) },
			handler{ ReferenceHandler<T>::GetSingleton() }
		{
			Update(false);
		}


		~NativeReference()
		{
			Update(true);
		}

	};
	REQ_OR_LESS_SIZE(NativeReference, sizeof(Variable));

	template<typename T>
	NativeReference extern_ref(T& tar)
	{
		return NativeReference{ tar };
	}
}
