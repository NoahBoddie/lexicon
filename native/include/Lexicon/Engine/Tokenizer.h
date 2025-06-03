#pragma once

namespace LEX
{
	/*
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
	//*/

	//This is now called tokenizer.h
	struct Tokenizer
	{
		constexpr static std::string_view remains = R"((?:\w|\S))";//Used to consume any other unidentified token

		//Just so I don't have to visit constantly
		Tokenizer* parent = nullptr;
		std::set<std::string_view> keywords;	//Keywords and conditionals aren't compiled into this due to them already needing to be identifiers
		std::set<std::string_view> conditionals;
		std::set<std::string_view> operators;
		std::set<std::string_view> punctuation;
		std::string mainPattern;
		mutable std::string compiledPattern;

		void AddKeyword(const std::string_view& pattern) {
			keywords.emplace(pattern);
		}

		void AddConditional(const std::string_view& pattern) {
			conditionals.emplace(pattern);
		}

		void AddOperator(const std::string_view& pattern) {
			compiledPattern.clear();
			operators.emplace(pattern);
		}

		void AddPunctuation(const std::string_view& pattern) {
			compiledPattern.clear();
			punctuation.emplace(pattern);
		}

		void AddPattern(const std::string_view& pattern) {
			compiledPattern.clear();
			mainPattern += pattern;
			mainPattern += "|";
		}

		bool IsOperator(const std::string_view& token) {
			if (!this) return false;
			return operators.contains(token) || parent->IsOperator(token);
		}


		bool IsKeyword(const std::string_view& token) {
			if (!this) return false;
			return keywords.contains(token) || parent->IsKeyword(token);
		}


		bool IsConditional(const std::string_view& token) {
			if (!this) return false;
			return conditionals.contains(token) || parent->IsConditional(token);
		}

		bool IsPunctuation(const std::string_view& token) {
			if (!this) return false;
			return punctuation.contains(token) || parent->IsPunctuation(token);
		}


		void LoadSymbols(std::vector<std::string_view>& list) const
		{
			if (!this)
				return;

			list.insert_range(list.end(), operators);
			list.insert_range(list.end(), punctuation);

			return parent->LoadSymbols(list);

		}
		void LoadPattern(std::string& pattern) const
		{
			if (!this)
				return;
			
			pattern += mainPattern;
			
			return parent->LoadPattern(pattern);
		}

		std::string CompilePattern() const
		{
			if (compiledPattern.empty())
			{

				auto sort_code = [](auto& a, auto& b) { return a.size() > b.size(); };

				std::vector<std::string_view> ordered;

				ordered.reserve(operators.size() + punctuation.size());
				
				LoadSymbols(ordered);
				std::sort(ordered.begin(), ordered.end(), sort_code);


				std::string result; 

				LoadPattern(result);

				for (auto& code : ordered) {

					if (code.size() == 0)
						continue;

					std::string submit;

					if (std::isalpha(code[0]) != 0) {
						submit = code;
					}
					else {
						submit = std::format(R"(\Q{}\E)", code);
					}


					result += std::format(R"((?:{})|)", submit);
				}

				result += remains;//Should end with "|"

				compiledPattern = result;
			}

			return compiledPattern;
		}

	};
	
	inline Tokenizer basicTokens{};
	inline Tokenizer scriptTokens{};
	inline Tokenizer prepTokens{};
	
	INITIALIZE("main_init")
	{

		
		/*

		

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

			//conditional
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
			constexpr static std::string_view objects = R"((?::{[^}\n]{0,}}?))";
			constexpr static std::string_view remains = R"((?:\w|\S))";//Last to be sub, appended to the compiled regex.

			constexpr static std::string_view preprocessor = R"((?:\n))";


			_constRegex += format;
			_constRegex += "|";
			_constRegex += comments;
			_constRegex += "|";
			_constRegex += identifiers;
			_constRegex += "|";
			_constRegex += digits;
			_constRegex += "|";
			_constRegex += quotes;
			_constRegex += "|";
			_constRegex += objects;
			_constRegex += "|";

		//*/


		constexpr static std::string_view format = R"((?:format[\s\S]*end_format))";
		constexpr static std::string_view digits = R"((?:(?:0[x|X][0-9|A-F|a-f]*)|(?:\.*\d(\d|\.)*f?)))";
		constexpr static std::string_view identifiers = R"((?:[_A-Za-z][\w]*))";
		constexpr static std::string_view quotes = R"((?:(?:'''[^']{0,}'{0,3})|(?:"""[^"]{0,}"{0,3})|(?:'[^'\n]{0,}'?)|(?:"[^"\n]{0,}"?)))";
		constexpr static std::string_view comments = R"((?:\/\/[^\n]{0,})|(?:\/\*([^*]|\*(?!\/)){0,}(?:\*\/)?))";
		constexpr static std::string_view objects = R"((?::{[^}\n]{0,}}?))";
		constexpr static std::string_view preprocessor = R"((?:\n))";

		basicTokens.AddPattern(format);
		basicTokens.AddPattern(digits);
		basicTokens.AddPattern(identifiers);
		basicTokens.AddPattern(quotes);
		basicTokens.AddPattern(comments);
		basicTokens.AddPattern(objects);

		basicTokens.AddPunctuation(",");
		basicTokens.AddPunctuation(";");
		basicTokens.AddPunctuation("(");
		basicTokens.AddPunctuation(")");
		basicTokens.AddPunctuation("{");
		basicTokens.AddPunctuation("}");
		basicTokens.AddPunctuation("[");
		basicTokens.AddPunctuation("]");
		basicTokens.AddPunctuation(":");
		basicTokens.AddPunctuation("::");
		basicTokens.AddPunctuation("...");
		//scriptTokens.AddPunctuation("/:");
		

		scriptTokens.parent = &basicTokens;

		//Want a better way to do this.
		scriptTokens.AddKeyword("if");
		scriptTokens.AddKeyword("else");
		scriptTokens.AddKeyword("return");
		scriptTokens.AddKeyword("default");
		scriptTokens.AddKeyword("struct");
		scriptTokens.AddKeyword("class");
		scriptTokens.AddKeyword("intrinsic");
		scriptTokens.AddKeyword("external");
		scriptTokens.AddKeyword("interface");
		scriptTokens.AddKeyword("bind");
		scriptTokens.AddKeyword("this");
		scriptTokens.AddKeyword("static");
		scriptTokens.AddKeyword("signed");
		scriptTokens.AddKeyword("unsigned");
		scriptTokens.AddKeyword("short");
		scriptTokens.AddKeyword("long");
		scriptTokens.AddKeyword("bool");
		scriptTokens.AddKeyword("int");
		scriptTokens.AddKeyword("object");
		scriptTokens.AddKeyword("float");
		scriptTokens.AddKeyword("string");
		scriptTokens.AddKeyword("double");
		scriptTokens.AddKeyword("const");
		scriptTokens.AddKeyword("mutable");
		scriptTokens.AddKeyword("void");
		scriptTokens.AddKeyword("voidable");
		scriptTokens.AddKeyword("ref");
		scriptTokens.AddKeyword("local");
		scriptTokens.AddKeyword("global");
		scriptTokens.AddKeyword("scoped");
		scriptTokens.AddKeyword("readonly");
		scriptTokens.AddKeyword("public");
		scriptTokens.AddKeyword("private");
		scriptTokens.AddKeyword("protected");
		scriptTokens.AddKeyword("internal");
		scriptTokens.AddKeyword("as");
		scriptTokens.AddKeyword("typeof");
		scriptTokens.AddKeyword("generic");

		scriptTokens.AddConditional("maybe");


		prepTokens.parent = &basicTokens;
		prepTokens.AddPattern(preprocessor);
		//These are not real keywords, they are just preprocessor things. They aren't reserved terms
		//prepTokens.AddKeyword("requires");//Is this an actual keyword?
		//prepTokens.AddKeyword("include");
		//prepTokens.AddKeyword("import");

		prepTokens.AddPunctuation("#");
		

	}


}
