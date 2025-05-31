#include "Lexicon/Engine/Parser.h"
#include "Lexicon/Engine/ParserTest.h"
//#include "CompileModule.h"
namespace LEX
{

	void ParsingStream::croak(std::string msg, Token* token)
	{
		//don't care so much about this.
		//TODO:Include name and project in croak please
		if (column() && line())
			throw ParsingError(std::format("{}(at line {} : column {})", msg, token ? token->line : line(), token ? token->column : column()));
		else
			throw ParsingError(std::format("{}", msg));

	}



	bool ParsingStream::IsType(TokenType type, std::string_view str) {
		RecordData tok = peek();

		if (!tok)
			return false;

		return (type == TokenType::Invalid || tok.GetEnum<Token>().type == type) && (str == "" || tok.GetTag() == str);
	}

	bool ParsingStream::WasType(TokenType type, std::string_view str) {
		RecordData tok = prev();
		return tok && tok.GetEnum<Token>().type == type && (str == "" || tok.GetTag() == str);
	}


	RecordData ParsingStream::ConsumeType(TokenType type, std::string_view str)
	{
		//TODO:Fix Format #1
		//TokenToString
		if (IsType(type, str) == false) {
			auto token = peek();
			croak(std::format("Expecting {}, recieved {} from \"{}\"", magic_enum::enum_name(type), magic_enum::enum_name(token.TOKEN().type), token.GetTag()));
		}

		return next(); 
	}

	void ParsingStream::SkipType(TokenType type, std::string_view str)
	{
		ConsumeType(type, str);
	}

	bool ParsingStream::SkipIfType(TokenType type, std::string_view str)
	{
		bool result = IsType(type, str);
		
		if (result)
			next();

		return result;
	}

	std::vector<Record> ParsingStream::Delimited(std::string_view start, std::string_view stop, std::function<void()> separator, std::function<ParseFunc> func) {
		//I would like a delimit that expects the seperator to be the second to last, and another that prevents it from being second to last.
		//Having a version of this with no parameters or just parser would be ideal, that way I can use member functions.

		//Checking for first might not really be that difficult.

		std::vector<Record> result;

		bool first = true;
		
		//if it's empty it will consume anything.
		if (start.empty() == false)
			SkipType(TokenType::Invalid, start);
		
		while (eof() == false) {
			//bool start = first;

			if (IsType(TokenType::Invalid, stop) == true) break;

			//RGL_LOG(debug, "skipping seperator '{}'", separator);

			if (first) first = false; 
			else if (separator) separator();

			if (IsType(TokenType::Invalid, stop) == true) break;

			//Thinking of using the second arg for something.
			//Record entry = start && begin ? begin(this, nullptr) : func(this, nullptr);
			Record entry = func(this, nullptr);

			if (entry) {
				result.push_back(entry);
			}
			else {
				report::parse::trace("empty record discared");
			}
		}

		SkipType(TokenType::Invalid, stop);

		return result;
	}




	//Make a version of this that can take a parser and record, and a version that can take nothing at all.
	std::vector<Record> ParsingStream::Delimited(std::string_view start, std::string_view stop, std::string_view separator, std::function<ParseFunc> func) {
		return ParsingStream::Delimited(start, stop, [&]() { SkipType(TokenType::Punctuation, separator); }, func);
	}

	

	void ParsingStream::unexpected() {
		//want this to look prettier at some point.
		//TODO:Fix Format #2
		croak(std::format("Unexpected token: {} ({})", peek().GetTag(), magic_enum::enum_name(peek().TOKEN().type)));//"Unexpected token");//
	}




	Record ParsingStream::EndExpression(Record& rec)
	{
		ParseModule::ExecuteModule<EndParser>(this, nullptr);
		return std::move(rec);
	}


	Record ParsingStream::EndExpression(Record&& rec)
	{
		ParseModule::ExecuteModule<EndParser>(this, nullptr);
		return std::move(rec);
	}

	//Would like to seperate these from parser
	// Would also like to make "create header" for this.
	Record ParsingStream::CreateExpression(std::string str, Syntax expr, std::vector<Record> children)
	{
		return Record{ str, expr, children };
	}

	//This has no reason to be in here specifically.
	Record ParsingStream::CreateExpression(RecordData data, Syntax expr, std::vector<Record> children)
	{
		auto tok = data.GetEnum<Token>();

		expr.line = tok.line;
		expr.column = tok.column;

		return Record{ data.GetTag(), expr, std::move(children) };
	}




	bool Parser__::CreateSyntaxTree(RecordBase& out, std::string_view text, std::string_view name, ParseModule* mdl, Column column, Line line)
	{
		//When parse is used, it should come with a type and a name. This type is what the top level type is. Only a few can be chosen,
		// and it will determine what can be compiled, and also how valid it is.

		std::unique_ptr<ParseModule> new_mod;

		if (!mdl) {
			new_mod = std::make_unique<ScriptParser>();
			mdl = new_mod.get();
		}

		if (name.empty() == true)
		{
			name = "<untitled>";
		}


		ParsingStream stream{ name, text, mdl, line, column };

		try
		{
			out = stream.ParseTopLevel();

			//return APIResult_::Success;
			//If someone wants 
			return !stream._memory.failure;
		}
		catch (const ParsingError& error)
		{
			//This should still include the name if possible.
			//TODO: Actually report parsing error, and also return a malformed record that can detail what the fuck just happened.
			report::parse::failure("HEY this is supposed to be a failure, and to print a message. For now it does nothing.");
			
			//return error.result;
			return false;
		}

	}


	bool Parser__::CreateSyntax(RecordBase& out, std::string_view text, ParseModule* mdl, Column column, Line line)
	{
		//When parse is used, it should come with a type and a name. This type is what the top level type is. Only a few can be chosen,
		// and it will determine what can be compiled, and also how valid it is.

		std::unique_ptr<ParseModule> new_mod;

		if (!mdl) {
			report::fault::critical("No parse module given bruv");
		}
		
		return CreateSyntaxTree(out, text, "<untitled>", mdl, column, line);
	}

}



