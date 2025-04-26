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
		/// <param name="value">If true assigns data from proxy to the target, if false vice versa</param>
		virtual void Assign(Variable& proxy, void* target, bool value) const = 0;

	};

	template <typename T>
	struct ReferenceHandler : public IReferenceHandler
	{
		static ReferenceHandler<T>* GetSingleton()
		{
			static ReferenceHandler<T> singleton;
			return std::addressof(singleton);
		}


		void Assign(Variable& proxy, void* target, bool value) const override
		{
			T& dest = *reinterpret_cast<T*>(target);

			if (value) {
				proxy = MakeVariable(dest);
			}
			else {
				//Here I would like some way to define equivalency so setting isn't required.
				dest = Unvariable<T>{}(std::addressof(proxy));
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

		//TODO:Instead of owning a shared pointer of this, I'm thinking why not have a shared pointer of a native reference be how we access it?
		// This would take away the size constraints, and it largely makes the most sense

		void Update(bool value)
		{
			handler->Assign(Ref(), target, value);

		}

		void UpdateVal()
		{
			Update(true);
		}


		void UpdateRef()
		{
			Update(false);
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
		//Used to be VariableComponent only
		template<Assignable<Variable> T> requires(!std::derived_from<Unvariable<T>, detail::not_implemented>)
		explicit NativeReference(T& tar) :
			target{ std::addressof(tar) },
			handler{ ReferenceHandler<T>::GetSingleton() }
		{
			//Is there an actual reason to do this right here?
			Update(true);
		}


		~NativeReference()
		{
			Update(false);
		}

	};
	REQ_OR_LESS_SIZE(NativeReference, sizeof(Variable));

	template<typename T>
	NativeReference extern_ref(T& tar)
	{
		return NativeReference{ tar };
	}
}
