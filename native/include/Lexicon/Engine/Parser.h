#pragma once

#include "Lexicon/Engine/ProcessContext.h"
#include "Lexicon/Engine/TokenStream.h"
#include "Lexicon/Engine/Expression.h"
#include "Lexicon/Engine/ExpressionType.h"
#include "Lexicon/Engine/ParseHandler.h"

#include "Lexicon/Engine/ParseModule.h"
#include "Lexicon/Engine/ModuleChain.h"
#include "Lexicon/Engine/Tokenizer.h"
#include "Lexicon/Engine/parse_strings.h"
namespace LEX
{
	class ParsingStream;

	
	//TODO:Change name from "Parser" (which will be the static parsing class) to "ParseStream" which is more reflective of what it does.
	// As this isn't actually the thing doing any of the parsing anyhow.

	using Iterator = std::string_view::const_iterator;


	typedef Iterator(TokenParser)(ParsingStream*, Iterator& it, Iterator);


	//I'd like a macro system as a part of a preprocessor system that couples with handling the preprocessor inputs and where they occur

	class ParsingStream
	{
	public:

		//Friends with just the parse module
		friend class Parser__;
		friend class ParseModule;


		struct Memory
		{

			Column column = 0;
			Line line = 0;
			Iterator current;
			RecordData peek;
			RecordData prev;
			bool failure = false;


		};

		


		ParsingStream(const std::string_view& name, const std::string_view& str, ParseModule* mdl, Line l = 1, Column c = 1) : 
			_name{ name }, 
			_stream { str },
			_modules{ ParseModule::BuildModules(mdl->GetCategory()) }
		{
			_memory.column = endColumn = c;
			_memory.line = endLine = l;
			
			_init.current = mdl;
			//Will croak without outputting line and column at this point.

			//TODO: Change this to have a single object that handles this
			//std::string kinda_basic = TokenHandler::GetRegex(false);

			//_regex = boost::regex{ kinda_basic };

			tokenizer = mdl->GetTokenizer();

			boost::regex regex{ tokenizer->CompilePattern() };

			defaultParser = [regex](ParsingStream* stream, Iterator& it, Iterator end) -> Iterator
			{
				if (it != end)
				{
					boost::match_results<Iterator> what;

					if (boost::regex_search(it, end, what, regex) == true) {

						auto& subject = what[0];


						if (subject.length() == 0)
							stream->croak(std::format("empty string found in parse results {}", std::string(it, end)));

						it = subject.second;

						return subject.first;
					}

					it = end;
				}

				return end;
			};



			_memory.current = _stream.cbegin();
			_end = _stream.cend();
			CalcColumnLine(_end, endColumn, endLine);
		}



		void CalcColumnLine(Iterator from, Iterator until, Column& a_column, Line& a_line)
		{
			//The lines and columns calculated are off. Need to fix it somehow.

			//Better to just use a while actually.
			auto test = std::count_if(from, until, [&](auto it)
				{//will count the number of line breaks and when one is detected it will reset the column
					bool res = it == '\n' || it == '\r';
					if (res) {
						a_column = 1;

						if (max_value<Line> == a_line)
							croak(std::format("Max number of line reached, more than {} lines.", max_value<Line>));

						a_line += 1;
					}
					else {
						if (max_value<Column> == a_column)
							croak(std::format("Column at line {} exceeds max character limit of {}.", a_column, max_value<Column>));

						a_column += 1;
					}
					return res;
				});
		}

		void CalcColumnLine(Iterator until, Column& a_column, Line& a_line)
		{
			return CalcColumnLine(current(), until, a_column, a_line);
		}

		void croak(std::string msg, Token* token = nullptr);


		//Proper name would be nexttoken or producetoken
		RecordData ReadNextImpl(const std::function<TokenParser>& func)
		{
			//This should be an impl function, the outer will keep calling this until it returns a "Valid" record.
			// Validity is in the eye of the beholder due to it throwing away what it finds. 
			//if (eof() == true)
			//	return {};

			auto curr = current();

			if (curr == _end)
				return {};


			auto res = func(this, curr, _end);

			if (res == _end)
				return {};


			if (curr == res) {
				report::parse::error("nothing moved, issue");
			}

			CalcColumnLine(res, _memory.column, _memory.line);

			auto data = RecordData{ std::string(res, curr), Token { column(), line() } };

			if (!data)
				croak("Empty string requested for use");

			current() = res;

			CalcColumnLine(curr, _memory.column, _memory.line);

			current() = curr;


			//If the creater was not a default, this will use whatever it's given as custom.
			if (&func != &defaultParser)
				data.TOKEN().type = TokenType::Custom;
			else {
				SetTokenType(data);
			}


			if (tokenizer && tokenizer->IsConditional(data.GetView()) == true) {
				data.TOKEN().isConditional = true;
			}


			return data;
		}

		RecordData ReadNext(const std::function<TokenParser>& func)
		{
			do
			{
				RecordData result = ReadNextImpl(func);

				if (result.TOKEN().type != TokenType::Comment)
					return result;
			} while (true);
		}




		constexpr Line line() const noexcept { return _memory.line; }
		constexpr Column column() const noexcept { return _memory.column; }

		RecordData CheckConditional(RecordData data)
		{
			//This will should check the currently queried parse module if it has a conditional with this name.
			if (data.TOKEN().isConditional && false)
				data.TOKEN().type = TokenType::Keyword;

			return data;
		}


		RecordData next()
		{
			//Using this while eof should likely result in a parsing error.

			RecordData token = _memory.peek;

			_memory.peek = {};//Should make a clear func for this.

			if (!token) {
				token = ReadNext(defaultParser);
			}

			_memory.prev = token;

			return CheckConditional(token);
		}

		RecordData peek() {
			//So it's something 
			if (!_memory.peek) {
				//auto it = _current;
				//_peek = _ReadNext(it);
				_memory.peek = ReadNext(defaultParser);
			}

			return CheckConditional(_memory.peek);
		}


		RecordData prev()
		{
			return _memory.prev;
		}


		bool eof() {
			return  !peek();
		}





		bool Search(const std::string& token, const std::vector<std::string_view>& strings)
		{
			//move to source.
			for (int i = 0; i < strings.size(); i++)
			{
				if (token == strings[i])
					return true;
			}

			return false;
		}


		void SetTokenType(RecordData& data)
		{

			constexpr char quote_char = '\"';
			constexpr char apost_char = '\'';

			std::string& token = data.GetTag();
			TokenType& type = data.TOKEN().type;
			//FOR ALL THINGS THAT SEARCH FRONT AND BACK, only the front should be exampled, however, the back should be a question for failure. Make that a different function
			// that should croak if need be. Such a thing should always return true.




			if (auto size = token.size(); size >= 2) {
				int res = std::strncmp(token.c_str(), "//", 2) == 0 ?
					1 : std::strncmp(token.c_str(), "/*", 2) == 0 ?
					2 : 0;
				//IE the second comment type
				const char& end = *(token.end() - 2);

				if (res == 2 && std::strncmp(&end, "*/", 2) != 0) {
					//Later, this is to be a generic function, able to print a generic name.
					croak("COMMENT unclosed at end of file.", &data.TOKEN());
				}
				if (res) {
					type = TokenType::Comment;
					return;
				}
			}
			
			if (token == "\n") {
				type = TokenType::Whitespace;
			}
			else if (token.starts_with(parse_strings::format_start)) {
				type = TokenType::Format;
			}
			else if (token == "true" || token == "false" || token == "maybe") {
				type = TokenType::Boolean;
			}
			else if (tokenizer->IsKeyword(token) == true) {
				type = TokenType::Keyword;
			}
			else if (token.find(":{") == 0 && token.rfind("}") == token.size() - 1) {
				type = TokenType::Object;
			}
			else if (token.front() == quote_char || token.front() == apost_char) {
				type = TokenType::String;
			}
			else if (std::isdigit(token.front()) || token.front() == '.' && token.size() > 1 && std::isdigit(token[1])) {
				type = TokenType::Number;
			}
			else if (tokenizer->IsOperator(token) == true) {
				type = TokenType::Operator;
			}
			else if (std::isalpha(token.front()) || token.front() == '_') {
				type = TokenType::Identifier;
			}
			else if (tokenizer->IsPunctuation(token) == true) {
				type = TokenType::Punctuation;
			}
			else {
				report::parse::error("Tokenizing: Unidentified token '{}' detected", data.GetTag());
			}
		}



		Record Parse(bool atomic) {
			bool success;

			int i = 0;

			Record result{};
			Record* nested = nullptr;
			do
			{
				success = SearchModule(result, nested, atomic);

				//should only crash if atomic modules fails
				if (!success && !nested)
					unexpected();

				nested = &result;

				i++;
			} while (success && (!eof() || nested));//eof to prevent searches once past the point.

			return result;
		}


		Record ParseAtomic() {
			Record result = Parse(true);
			return result;
		}

		Record ParseExpression() {
			Record result = Parse(false);
			return result;
		}

		void LinkContext(ModuleChain& chain)
		{
			//I want to give more power to these functions, so much that they manage the process of construction too
			_chain = &chain;
		}

		void ExecuteModule(Record& record, Record* rec_nest, ParseModule* mdl)
		{
			//This version of the function should have no checks, it's expected that it should work, then any error fatal to the process.
			//logger::info("C {}", peek().GetTag());
			//ModuleChain link = _chain->InheritChain(mdl, _chain);//(mdl, _chain, this);
			ModuleChain link{ mdl, _chain };

			//I think I want to bake the reset into it's blood
			//_LinkContext(link);
			//logger::info("C {}", peek().GetTag());
			record = mdl->HandleToken(this, rec_nest);



			//_UnlinkContext();
		}

		bool QueryModule(Record* rec_nest, ParseModule* mdl, ParseFlag flag)
		{
			//This is where the above module checks should go. Uses execute module
			// also note, try should probably not check the current context, as it's likely the one who fired it.
			// this allows more control where there are situations where the only ways a thing happens is if the parser is in control.

			//Mesures first if the module can handle the current situation, but then measures if the currently loaded context will allow such a thing.


			if (eof() && mdl->CanParseEOF() == false) {
				return false;
			}

			bool module_check = mdl->CanHandle(this, rec_nest, flag);
			bool context_check = module_check ? _chain->current->ContextAllowed(mdl, _chain) : false;

			return module_check && context_check;

		}

		bool TryModule(Record& out, Record* rec_nest, ParseModule* mdl, ParseFlag flag)
		{
			//This is where the above module checks should go. Uses execute module
			// also note, try should probably not check the current context, as it's likely the one who fired it. 
			// this allows more control where there are situations where the only ways a thing happens is if the parser is in control.

			//Mesures first if the module can handle the current situation, but then measures if the currently loaded context will allow such a thing.

			bool handle = QueryModule(rec_nest, mdl, flag);


			if (handle) {
				ExecuteModule(out, rec_nest, mdl);
			}

			return handle;

		}


		bool SearchModule(Record& out, Record* rec_nest, bool atomic)
		{
			//In truth, should access the ParseHandler


			for (std::unique_ptr<ParseModule>& mod : _modules)
			{
				if (!mod)
					continue;
				//The first doesn't count, if not nested atomic will not be considered.
				if (rec_nest && atomic && mod->IsAtomic() == false)
					continue;

				//If not atomic, and the parseModule isn't atomic
				bool success = TryModule(out, rec_nest, mod.get(), atomic ? ParseFlag::Atomic : ParseFlag::None);

				if (success)
					return true;
			}

			return false;

		}



		Record ParseTopLevel()
		{
			//I think this function could actually be allowed to be free.
			if (std::addressof(_init) != _chain)
				croak("Cannot parse top level while parsing.");

			if (!_chain)//Should be warning?
				croak("ParseStream is expired, cannot parse top level.");


			if (!_chain->current)
				croak("No top level ParseModule detected.");

			ParseModule* mdl = dynamic_cast<ParseModule*>(_chain->current);

			//If this can't work, that's one weird occurance. Probably not going to account for that, I'd rather a crash.

			Record result = mdl->HandleToken(this, nullptr);

			if (!eof())
				croak("Top level file finished not EOF");

			//Severing the chain means that this string has been parsed to it's full ability, and (likely) parsed properly.
			// make this a function btw.
			_chain = nullptr;

			return result;
		}




		bool IsType(TokenType type, std::string_view str = "");

		RecordData ConsumeType(TokenType type, std::string_view str = "");

		void SkipType(TokenType type, std::string_view str);

		bool SkipIfType(TokenType type, std::string_view str);

		//Would like something called required type which fulfills the role of next and istype

		bool WasType(TokenType type, std::string_view str = "");



		std::vector<Record> Delimited(std::string_view start, std::string_view stop, std::string_view separator, std::function<ParseFunc> func);


		std::vector<Record> Delimited(std::string_view start, std::string_view stop, std::function<void()> separator, std::function<ParseFunc> func);


		//A shorthand so new lambdas don't need to get made contantly to send arg-less calls.
		std::vector<Record> Delimited(std::string_view start, std::string_view stop, std::string_view separator, std::function<Record()> func)
		{
			//std::function<ParseFunc> _b = nullptr;

			//_b = begin ? [=](auto, auto) { return begin(); } : _b;

			return Delimited(start, stop, separator, [=](auto, auto) { return func(); });
		}

		//template <typename TClass>
		std::vector<Record> Delimited(std::string start, std::string stop, std::string separator, std::function<Record(ParsingStream*)> func)
		{
			return Delimited(start, stop, separator, [=](ParsingStream* a1, auto) { return func(a1); });
		}



		//A shorthand so new lambdas don't need to get made contantly to send arg-less calls.
		std::vector<Record> Delimited(std::string start, std::string stop, std::function<void()> separator, std::function<Record()> func)
		{
			//std::function<ParseFunc> _b = nullptr;

			//_b = begin ? [=](auto, auto) { return begin(); } : _b;

			return Delimited(start, stop, separator, [=](auto, auto) { return func(); });
		}



		//template <typename TClass>
		std::vector<Record> Delimited(std::string start, std::string stop, std::function<void()> separator, std::function<Record(ParsingStream*)> func)
		{
			return Delimited(start, stop, separator, [=](ParsingStream* a1, Record*) { return func(a1); });
		}


		//std::vector<Record>

		void unexpected();


		ModuleChain GetChain();


		//I think the above works like this. 
		// Search looks for a module.
		// Try will try to use that module
		// execute will execute, regardless of conditions so the last thing needs to check itself.







		Record EndExpression(Record& rec);
		Record EndExpression(Record&& rec);



		//Would like to seperate these from parser(steam) and move it to Parser__ (to be named ParsingStream)
		//Would also like to remove the expression from this, there's no reason for it to be.
		static Record CreateExpression(std::string str, Syntax expr = SyntaxType::None, std::vector<Record> children = {});

		//This has no reason to be in here specifically.
		static Record CreateExpression(RecordData data, Syntax expr, std::vector<Record> children = {});



		template<std::same_as<Record>... Rs>requires (sizeof...(Rs) != 0)
			static Record CreateExpression(RecordData data, Syntax expr, Rs... records)
		{
			return CreateExpression(data, expr, { records... });
		}

		ParseModule* GetBuiltModule(const std::type_info& ref) const
		{
			const std::type_info* other = &ref;

			for (auto& mod : _modules)
			{
				auto ptr = mod.get();

				auto info = &typeid(*ptr);

				if (info == other)
				{
					return ptr;
				}
			}

			return nullptr;
		}
		template <std::derived_from<ParseModule> Module>
		ParseModule* GetBuiltModule() const
		{
			return GetBuiltModule(typeid(Module));
		}

		void FlagFailure()
		{
			_memory.failure = true;
		}

		std::string_view name() const
		{
			return _name;
		}

		ModuleChain* chain()
		{
			return _chain;
		}


	protected:
		const Iterator& current() const
		{
			return _memory.current;
		}

		Iterator& current()
		{
			return _memory.current;
		}

		Tokenizer* tokenizer = nullptr;
		std::function<TokenParser> defaultParser;

		std::string_view _name;

		std::string_view _stream;

		Memory _memory;

		Iterator _end;

		Column endColumn;
		Line endLine;

		ModuleChain _init { nullptr, nullptr, this };
		ModuleChain* _chain = &_init;
		std::vector<std::unique_ptr<ParseModule>> _modules;


	};



















	//TODO:The initial use of parse (somehow depending) should be establishing that the top most record is a script.


	struct Parser__
	{
		//Syntax created from this is likely at runtime, and genarally will lack any kind of header behaviour. This is primarily to be used for loose
		// formulas. The module unlike below has to be specified.
		// That, or I'll make project locations and such sorta suggestions.

		template <std::derived_from<ParseModule> Module>
		static bool CreateSyntax(RecordBase& out, std::string_view text, Column column = 1, Line line = 1)
		{
			std::unique_ptr<ParseModule> mdl;

			mdl = std::make_unique<Module>();

			return CreateSyntax(out, text, mdl.get(), column, line);
		}


		static bool CreateSyntax(RecordBase& out, std::string_view text, ParseModule* mdl, Column column = 1, Line line = 1);

		static bool CreateSyntaxTree(RecordBase& out, std::string_view text, std::string_view name = "", ParseModule* mdl = nullptr, Column column = 1, Line line = 1);
	};
	
}


