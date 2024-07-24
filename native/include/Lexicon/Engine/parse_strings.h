#pragma once

namespace LEX
{
	struct parse_strings
	{
		constexpr static auto this_word = "<:this:>";
		constexpr static auto rhs = "<:right:>";
		constexpr static auto lhs = "<:left:>";
		constexpr static auto path = "<:path:>";
		constexpr static auto unscoped = "<:unscoped:>";
		constexpr static auto scope = "::";
		constexpr static auto header = "<:header:>";
		constexpr static auto declare_specifier = "<:decl_spec:>";
		constexpr static auto type_qualifier = "<:type_qual:>";
		constexpr static auto type_specifier = "<:type_spec:>";
		constexpr static auto def_expression = "<:def:>";//change name to definition
		constexpr static auto extends = "<:extend:>";
		constexpr static auto untitled = "<:NA:>";
		constexpr static auto body = "<:bod:>";
		constexpr static auto code = "<:cod:>";
		constexpr static auto parameters = "<:params:>";
		constexpr static auto args = "<:args:>";

		constexpr static auto statement_block = "<:statement:>";
		constexpr static auto expression_block = "<:express:>";
		constexpr static auto alternate_block = "<:alt:>";

		constexpr static auto true_value = "true";
		constexpr static auto false_value = "false";
		constexpr static auto maybe_value = "maybe";

		constexpr static auto generic = "<:gen:>";
		constexpr static auto settings = "<:settings:>";
		constexpr static auto data_type = "<:datatype:>";
		constexpr static auto attach = "<:attach:>";
		constexpr static auto derives = "<:derives:>";
		constexpr static auto format_start = "format";
		constexpr static auto format_end = "end_format";

	};

}