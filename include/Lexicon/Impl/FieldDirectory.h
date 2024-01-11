#pragma once

namespace LEX
{
	struct FieldDirectory
	{
		//should be a constexpr struct that functions much like how a string_literal should.
		//The idea is that it will keep it's own version of a string literal (kinda)
		// where it will take a string and delimit it, seperating it between a project, a script, and an iden.

		//There should be another type, where basically it isn't constexpr and that isn't a template that I can use.

		std::string_view project;
		std::string_view script;
		std::string_view identifier;//Can be multiple entries, a function should exist to break down.
		//std::string idens;//I'd like this to come as an array if I can.


		std::string GetFullName()
		{
			return "NOT IMPLEMENTED";
		}
	};
}