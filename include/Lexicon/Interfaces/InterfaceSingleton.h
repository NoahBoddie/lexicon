#pragma once

#include "Interface.h"
#include "InterfaceManager.h"

namespace LEX
{
	template <typename T>
	struct InterfaceSingleton
	{
		using _Self = InterfaceSingleton<T>;

		InterfaceSingleton()
		{
			static_assert(std::is_base_of_v<_Self, T>, "Requires T to derive from InterfaceSingleton's specialization of T.");
			static_assert(std::is_base_of_v<Interface, T>, "Requires T to derive from Interface.");
		}




		struct _unprotect : public T {
			_unprotect() : T() {}
		};

		static T* RequestSingleton()
		{
			static T* _interface = nullptr;
			if (_interface)
			{
				Update result = InterfaceManager::RequestInterface<T>(_interface, T::version);

				//Make this part a static function plz
				switch (result)
				{
				case Update::Invalid:
					report::fault::critical("fix name mismatch");
					break;//mention the name is probably wrong

				case Update::Library:
					report::fault::critical("update library");
					break;//Library is out of date, developer fault.

				case Update::Engine:
					report::fault::critical("update engine");
					break;//Engine is out of date, user fault.

				case Update::Match:
					break;//We're all gucci
				}
			}

			return _interface;
		}


		static T& GetSingleton()
		{

#ifdef LEX_SOURCE
			static _unprotect singleton{};

			return singleton;

#else			//If not the source, we go through the interface manager via request, instead of instantiating our own one.
			return *RequestSingleton();
#endif

		}
	private:
		struct _instance//class should be private.
		{
		private:
			T* get() const
			{
#ifdef LEX_SOURCE
				return &GetSingleton();

#else			
				return RequestSingleton();
#endif
			}
		public:

			T& operator* () const
			{
				return *get();

			}


			T* operator-> ()const
			{
				return get();
			}

			operator bool() const
			{
				//source never has to ask.
#ifdef LEX_SOURCE
				return true;

#else			
				return get();
#endif
			}
		};
	public:
		//inline static T& instance = GetSingleton();
		inline static _instance instance = _instance{};
		


#ifdef LEX_SOURCE
		//We only need to 

		struct _ctor {
			_ctor()
			{
				auto& singleton = GetSingleton();
				InterfaceManager::RegisterInterface(singleton);
			}
		};


		inline static _ctor _init{};

#endif
	};

}