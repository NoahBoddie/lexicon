#pragma once

#include "Lexicon/IssueCode.h"

namespace LEX
{
	struct APIResult_
	{

		enum QuickValue
		{
			Success,
			
		};
		//I'll flesh this out later but right now the look is all that matters
		constexpr APIResult_(QuickValue v) 
		{}

		constexpr APIResult_() noexcept = default;

		APIResult_(uint32_t issue1, uint32_t issue2, uint32_t message) : issueID{ issue1, issue2 }, messageID{ message } {}



		//This currently means that there is no code (but there is an error) and there is no message.
		IssueCode issueID{};
		//Should the given issue always be in english? maybe i just store both and let them choose which they want.
		uint64_t messageID{};
		

		[[nodiscard]] std::string Redeem(bool useTranslation)
		{
			//The point of redeem is to get the message as printed (mostly) to the logger. This can be useful if you want to print it to your
			// own logs or something.

			//After redeemed the given message id is killed. Similar things will happen when the API result dies.

			//Actually, there should be the ability to get the message without deleting it. So I can get it multiple times if need be.
			return {};
		}


		constexpr operator bool() const
		{
			return !issueID;
		}
	};


	//TODO: Work throw location into the exceptions "source_location a_loc = source_location::current()"


	//TODO: Move Basic Error to somewhere that's not here.
	struct Error : public std::exception
	{
		//this should only be caught if one is looking to catch fatal

		APIResult_ result{ (uint32_t)-1, (uint32_t)-1, (uint32_t)-1 };




		//FIXITE: Replace with a string so proper formatting can be done.
		Error(std::string what) {}
		Error(const char* what) {}
	};

	struct ScriptError : public Error
	{
		//All scripting errors should derive from this.


		ScriptError(std::string what) : Error{ what } {}
		ScriptError(const char* what) : Error{ what } {}
	};

	struct ParsingError : public Error
	{
		

		//TODO: Consider what the fuck catches one of these.
		//FIXITE: Replace with a string so proper formatting can be done.
		ParsingError(std::string what) : Error{ what } { logger::error("{}", what); }
		ParsingError(const char* what) : Error{ what } { logger::error("{}", what); }
	};
	using ParseError = ParsingError;


	struct CompilingError : public Error
	{


		
		CompilingError(std::string what) : Error{ what } {}
		CompilingError(const char* what) : Error{ what } {}
	};
	using CompileError = CompilingError;


	struct LinkError : public Error
	{



		LinkError(std::string what) : Error{ what } {}
		LinkError(const char* what) : Error{ what } {}
	};
	

	struct RuntimeError : public Error
	{
		RuntimeError(std::string what) : Error{ what } {}
		RuntimeError(const char* what) : Error{ what } {}
	};



	struct FatalError : public std::exception
	{
		//Fatal error cannot be caught by looking for error. Also want this to pivot to a catch all.
		std::string _tempWhat;

		//FIXITE: Replace with a string so proper formatting can be done.
		FatalError(std::string what) : _tempWhat{ what } {}
		FatalError(const char* what) : _tempWhat(what) {}


	};


	struct EnvironmentError : public CompilingError
	{
		EnvironmentError(std::string what) : CompilingError{ what } { logger::error("{}", what); }
		EnvironmentError(const char* what) : CompilingError{ what } { logger::error("{}", what); }
	};

}