

#include "Lexicon/Engine/ParseModule.h"
#include "Lexicon/Engine/Parser.h"

namespace LEX::Impl
{
	class TokenHandler
	{
		enum ChangeFlag
		{
			//I super don't care about these right now.
			// But the idea was that keywords and such scale
			None = 0,
			Operators = 1 << 0,
			Puncuation = 1 << 1,
			Keyword = 1 << 2,
			Constant = 1 << 3,
		};

		inline static ChangeFlag _flags = ChangeFlag::Constant;

		//Op codes are codes that are the strings of various operators. Their sorting requires that the longer operators are first, while the smaller later.
		// This ensures that a smaller operator doesn't consume a bigger one.
		inline static std::vector<std::string_view> _opCodes
		{ 
			"=>",
			"+", 
			"-", 
			"*", 
			"/", 
			"%",
			"?",
			"=", 
			"<", 
			">",
			"!", 
			"|",
			"||",
			"&", 
			"&&", 
			".",
			"^",
			"^^",
			"++", 
			"--", 
			"<<", 
			">>", 
			"==", 
			"+=",
			"!=",
			">=",
			"<=",
			"|=",
			"&=",
		};
		//Extra op keys
		//OR or AND and, NOT, not, XOR/xor (not sure how to handle that), is (and is not should be something to write, knowing is came before can help with that)
		// as, also is ":" an operator? Should ? be an operator too?
		// The lower case are the logical and the uppercase are the bitwise
		// === is another one, and maybe !==
		//Also to tell if something is not some form of assign, if it doesn't start with '=' or '!' and ends with '='

		//TODO: use (?:...) more, this is a non-capturing group, and most of the time this is what I actually want.

		inline static std::vector<std::string_view> _puncCodes{ ",", ";", "(",")", "{", "}", "[", "]", ":", "::", "...", "/:", "#", "[:", ":]"};//?
		

		//TODO: Consider some contextual keywords, keywords not always made keywords, but only under certain situations. 
		// Basically they'd compare to being identifiers unless asked for specifically.

		//This shoult be handled externally I think, or should be able to be added to.
		inline static std::vector<std::string_view> _keyCodes
		{ 
			"if", 
			"else", 
			"return", 
			"default", 
			"struct", 
			"class", 
			"intrinsic", 
			"external",//I REALLY want to rename this.
			"interface",
			"bind",
			"this",
			"static",
			"signed",
			"unsigned",
			"short",
			"long",
			"bool",
			"int",
			"object",
			"float",
			"string",
			"double",
			"const",
			"mutable",
			"void",
			"voidable",
			"ref",
			"local",
			"global",
			"scoped",
			//"maybe",
			"readonly",
			"public",
			"private",
			"protected",
			"internal",
			"requires",
			"include",
			"import",
			"as",
			"typeof",
			"generic",
		};

		inline static std::vector<std::string_view> _ConKeyCodes
		{
			"maybe",
		};

		inline static std::string _compRegex;
		inline static std::string _constRegex;
		//
		//The para's should come predone
		constexpr static std::string_view format = R"((?:format[\s\S]*end_format))";//((\d|\.)+f?) /// (((\d|\.)(\d|\.)+)f?)
		constexpr static std::string_view digits = R"((?:(?:0[x|X][0-9|A-F|a-f]*)|(?:\.*\d(\d|\.)*f?)))";//(\.*\d(\d|\.)*f?) //((\d|\.)+f?) /// (((\d|\.)(\d|\.)+)f?)
		constexpr static std::string_view identifiers = R"((?:[_A-Za-z][\w]*))";
		constexpr static std::string_view quotes = R"((?:(?:'''[^']{0,}'{0,3})|(?:"""[^"]{0,}"{0,3})|(?:'[^'\n]{0,}'?)|(?:"[^"\n]{0,}"?)))";
		constexpr static std::string_view comments = R"((?:\/\/[^\n]{0,})|(?:\/\*([^*]|\*(?!\/)){0,}(?:\*\/)?))";
		constexpr static std::string_view objects = R"((?::{[^}\n]{0,}}?))";//Currently objects aren't used, but I'm thinking about maybe not having these as a literal.
		constexpr static std::string_view remains = R"((?:\w|\S))";//Last to be sub, appended to the compiled regex.
		
		constexpr static std::string_view preprocessor = R"((?:\n))";

		static void _CheckCompiled();

		static void _CheckConstant();
	public:
		
		static std::vector<std::string_view> GetKeywords();
		static std::vector<std::string_view> GetOperators();
		static std::vector<std::string_view> GetPuncutation();

		static std::string GetRegex(bool prep);

	};
}