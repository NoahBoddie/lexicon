#pragma once

namespace LEX
{

	//TODO: Move Basic Error to somewhere that's not here.
	struct Error : public std::exception
	{
		std::string _tempWhat;

		//FIXITE: Replace with a string so proper formatting can be done.
		Error(std::string what) : _tempWhat{ what } {}
		Error(const char* what) : _tempWhat(what) {}
	};

	struct ParsingError : public Error
	{
		

		//TODO: Consider what the fuck catches one of these.
		//FIXITE: Replace with a string so proper formatting can be done.
		ParsingError(std::string what) : Error{ what } {}
		ParsingError(const char* what) : Error{ what } {}
	};


	struct CompilingError : public Error
	{


		
		CompilingError(std::string what) : Error{ what } {}
		CompilingError(const char* what) : Error{ what } {}
	};


}