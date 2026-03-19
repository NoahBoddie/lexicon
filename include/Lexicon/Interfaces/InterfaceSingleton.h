#pragma once

#include "Interface.h"
#include "InterfaceManager.h"

#ifdef LEX_SOURCE
#include "Lexicon/Engine/SingletonManager.h"
#endif

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

#ifndef LEX_SOURCE

	private:
		inline static T* _interface = nullptr;//should be atomic?
	public:


		static Update CheckSingleton(bool catch_error)
		{
			if (!_interface)
			{
				Update result;

				if (SafeInvoke<InterfaceManager::RequestError>(catch_error, [&]() { result = InterfaceManager::RequestInterface<T>(_interface, T::version); }))
				{
					result = Update::Missing;
				}

				//Make this part a static function plz
				switch (result)
				{
				case Update::Invalid:
					logger::critical("Invalid interface request, name not found");
					throw FatalError("invalid interface request");
					break;//mention the name is probably wrong

				case Update::Library:
					logger::critical("Requesting plugin is out of date with engine. Developer needs to update version.");
					throw FatalError("client is out of date");
					break;//Library is out of date, developer fault.

				case Update::Engine:
					logger::critical("Engine is out of date with requesting plugin version. Update Scripting engine.");
					throw FatalError("engine out of date");
					break;//Engine is out of date, user fault.

				case Update::Match:
					assert_if(!_interface) {
						logger::critical("Interface missing despite request success");
						throw FatalError("missing interface after successful query");
					}

					report::message::trace("interface {} success.", TypeName<T>::value);
					break;//We're all gucci
				case Update::Missing:
					break;
				default:
					logger::info("unknown issue {}", magic_enum::enum_name(result));
					throw FatalError("missing interface after successful query");
					break;
				}

				return result;
			}
			
			return Update::Match;
		}

		static T* RequestSingleton(bool catch_error)
		{
			CheckSingleton(catch_error);
			return _interface;
		}

		static T* RequestSingleton()
		{
			return RequestSingleton(true);
		}
#endif




		static T& GetSingleton()
		{

#ifdef LEX_SOURCE
			static _unprotect& singleton = make_singleton<_unprotect>();
			//static _unprotect singleton{};

			return singleton;

#else			//If not the source, we go through the interface manager via request, instead of instantiating our own one.
			return *RequestSingleton(false);
#endif

		}
	private:
		struct _instance//class should be private.
		{
		public:
			T* get() const
			{
#ifdef LEX_SOURCE
				return &GetSingleton();

#else			
				return RequestSingleton(false);
#endif
			}
		

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
				return CheckSingleton(true) == Update::Match;
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