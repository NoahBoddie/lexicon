#pragma once

#include "Lexicon/Variable.h"
#include "Lexicon/Unvariable.h"

namespace LEX
{


	//Handles references to types that can be converted to references within RuntimeVariable
	struct INativeReference
	{

		INativeReference(void* tar) : _target{ tar }{}

		virtual ~INativeReference() = default;

		virtual void Update(bool value) = 0;


		void UpdateVal()
		{
			Update(true);
		}


		void UpdateRef()
		{
			Update(false);
		}

		Variable& Ref() { return _proxy; }
		const Variable& Ref() const { return _proxy; }

		//I'm thinking that this only updates at the end, or when someone specifies they want to update it some how.

		//Type must both be able to use unvariable and have be transferable into being a variable.
		//Used to be VariableComponent only
		

	protected:
		void* _target = nullptr;

		Variable _proxy{};
	};
	
	template <Assignable<Variable> T> requires(!std::derived_from<Unvariable<T>, detail::not_implemented>)
	struct NativeReference : public INativeReference
	{
		~NativeReference()
		{
			Update(false);
		}

		explicit NativeReference(T& tar) : INativeReference{ std::addressof(tar) }
		{
			//Is there an actual reason to do this right here?
			NativeReference::Update(true);
		}



		void Update(bool value) override
		{
			T& dest = *reinterpret_cast<T*>(_target);

			if (value) {
				_proxy = dest;
			}
			else {
				//Here I would like some way to define equivalency so setting isn't required.
				dest = Unvariable<T>{}(std::addressof(_proxy));
			}
		}


	};


	template<typename T>
	std::shared_ptr<INativeReference> extern_ref(T& tar)
	{
		return std::make_shared<NativeReference<T>>(tar);
	}
}
