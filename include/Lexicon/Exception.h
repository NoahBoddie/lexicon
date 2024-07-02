#pragma once

namespace LEX
{
	//TODO: Work throw location into the exceptions "source_location a_loc = source_location::current()"


	//TODO: Move Basic Error to somewhere that's not here.
	struct Error : public std::exception
	{
		//this should only be caught if one is looking to catch fatal

		std::string _tempWhat;

		//FIXITE: Replace with a string so proper formatting can be done.
		Error(std::string what) : _tempWhat{ what } {}
		Error(const char* what) : _tempWhat(what) {}
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
		ParsingError(std::string what) : Error{ what } {}
		ParsingError(const char* what) : Error{ what } {}
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
		EnvironmentError(std::string what) : CompilingError{ what } { logger::debug("{}", what); }
		EnvironmentError(const char* what) : CompilingError{ what } { logger::debug("{}", what); }
	};

}