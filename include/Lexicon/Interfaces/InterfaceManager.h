#pragma once

#include "Interface.h"

namespace LEX
{
	struct Interface;


#ifdef LEX_SOURCE
#define LEX_API extern "C" [[maybe_unused]] __declspec(dllexport)
#else
#define LEX_API inline
#endif


	

	
	//Pulls the version of the server binary and returns if the client's version is compatible with the server
	LEX_API Update PullVersion_Impl(uintptr_t client, uintptr_t& server) INTERFACE_FUNCTION;


	//Registers an interface under the given name
	LEX_API bool RegisterInterface_Impl(Interface& ifc, std::string_view name) INTERFACE_FUNCTION;
	
	//Attempts to pull an interface of a given version. If it's not a match, an interface will fail to be given
	LEX_API Update RequestInterface_Impl(Interface*& out, std::string_view name, uintptr_t version) INTERFACE_FUNCTION;
	

	LEX_API void ModInterfaceUseCount_Impl(bool inc) INTERFACE_FUNCTION;


#ifdef LEX_SOURCE
#ifndef NDEBUG
#define TEST_INTERFACE
#endif
#endif



	struct InterfaceManager
	{
		using VersionChecker = Update(*)(uintptr_t server, uintptr_t client);


		//bool expects = false;//A value to say we expect there to be a lexicon library by now
		struct RequestError : public std::exception
		{
			using exception::exception;
		};
		
	INTERNAL:
		inline static std::set<VersionChecker> checks;

	public:
		static void AddVersionCheck(VersionChecker check)
		{
			checks.emplace(check);
		}



		static uintptr_t GetVersion()
		{
			uintptr_t server;
#if !defined(LEX_SOURCE) || defined(TEST_INTERFACE)

			uintptr_t client = LEX_VERSION;
	
			if (SafeInvoke<RequestError>([&]()
				{
					ExternCall<PullVersion_Impl, RequestError>(FILE_FORMAT(LEX_BINARY_MODULE),
						"PullVersion_Impl", client, server);
				}))
			{
				server = 0;
			}
#else
			server = LEX_VERSION;
#endif
			return server;
		}

		//change name to validate version.
		static void ValidateVersion(bool missingOk = true)
		{
#if !defined(LEX_SOURCE) || defined(TEST_INTERFACE)
			static bool checked = false;
			if (!checked)
			{
				uintptr_t client = LEX_VERSION;
				uintptr_t server = 0;

				//TODO: Make this setup a real function. I use it a lot.
				Update result;

				if (SafeInvoke<RequestError>([&]()
					{
						result = ExternCall<PullVersion_Impl, RequestError>(FILE_FORMAT(LEX_BINARY_MODULE),
							"PullVersion_Impl", client, server);
					}))
				{
					result = Update::Missing;
				}

				if (result == Update::Match) {
					for (auto check : checks) {
						assert_if_not(check)
							result = check(server, client);

						if (result != Update::Match)
							break;
					}
				}

				if (result != Update::Match)
				{
					std::string name;
					std::string message;

					switch (result)
					{
						//TODO: I'd like to make this into a self formatting function if I can swing it.
					case Update::Library:
						message = std::format("{} is out of date. Needs to be compiled on most recent version of {}.", GetModuleName(), LEX_BINARY_MODULE);
						goto terminate;
					case Update::Engine:
						message = std::format("{} is out of date. Please get most recent version.", LEX_BINARY_MODULE);
						goto terminate;

					case Update::Missing:
						if (missingOk)
							break;

						message = std::format("{} is missing. Please load with {}, or update if out of date.", LEX_BINARY_MODULE, GetModuleName());
						goto terminate;

					case Update::Invalid:
						//Invalid can't really be used here
					terminate:
						name = GetModuleName();
						MessageBoxA(NULL, message.c_str(), name.c_str(), MB_OK);
						TerminateProcess(GetCurrentProcess(), EXIT_FAILURE);
						break;
					}
				}

				checked = true;
			}

#endif

		}

		static bool RegisterInterface(Interface& ifc, std::string_view name)
		{
#if !defined(LEX_SOURCE) || defined(TEST_INTERFACE)
			ValidateVersion();
			using Self = decltype(RegisterInterface_Impl);
			return ExternCall<RegisterInterface_Impl, RequestError>(FILE_FORMAT(LEX_BINARY_MODULE), "RegisterInterface_Impl", ifc, name);
#else
			return RegisterInterface_Impl(ifc, name);
#endif

		//If this is the source, it should just directly use the implemetation.
			
		}

		static Update RequestInterface(Interface*& out, std::string_view name, uintptr_t version)
		{
#if !defined(LEX_SOURCE) || defined(TEST_INTERFACE)
			ValidateVersion();
			using Self = decltype(RequestInterface_Impl);
			return ExternCall<RequestInterface_Impl, RequestError>(FILE_FORMAT(LEX_BINARY_MODULE), "RequestInterface_Impl", out, name, version);
#else
			
			return RequestInterface_Impl(out, name, version);
#endif

			
		}


		template<std::derived_from<Interface> T>
		static bool RegisterInterface(T& ifc)
		{
			return RegisterInterface(ifc, GetTypeName<std::remove_cvref_t<T>>());
		}





		template<std::derived_from<Interface> T>
		static Update RequestInterface(T*& out, uintptr_t version)
		{
			Interface* buffer = nullptr;

			auto result = RequestInterface(buffer, GetTypeName<std::remove_cvref_t<T>>(), version);

			if (buffer)//Have to reinterpret cast because the dynamic cast will NOT work for interfaces. Getting the shit is good enough.
				out = reinterpret_cast<T*>(buffer);


			return result;
		}


		static void ModInterfaceUseCount(bool inc)
		{
#if !defined(LEX_SOURCE) || defined(TEST_INTERFACE)
			assert_if(SafeInvoke<RequestError>([&]()
				{
					ExternCall<ModInterfaceUseCount_Impl, RequestError>(FILE_FORMAT(LEX_BINARY_MODULE),
						"ModInterfaceUseCount_Impl", inc);
				}))
			{
				//Do something idk
			}
#else
			ModInterfaceUseCount_Impl(inc);
#endif
		}
	};

}