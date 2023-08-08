#include "Lexicon/Impl/Parser.h"
#include "Lexicon/Impl/ParserTest.h"
//#include "CompileModule.h"
namespace LEX::Impl
{
	Parser::Parser(TokenStream& t, ParseModule* mdl) : tokenizer{ t }
	{
		_init.current = mdl;
	}

	//parser should come with a context, this shit aint 
	Parser::Parser(TokenStream&& t, ParseModule* mdl) : tokenizer{ t }
	{
		_init.current = mdl;
	}



	void Parser::_LinkContext(ProcessChain& chain)
	{
		//I want to give more power to these functions, so much that they manage the process of construction too
		contextChain = &chain;
	}

	void Parser::_UnlinkContext()
	{
		//Add checks please
		contextChain = contextChain->previous;
	}

	void Parser::_ExecuteModule(Record& record, Record* rec_nest, ParseModule* mdl)
	{
		//This version of the function should have no checks, it's expected that it should work, then any error fatal to the process.

		ProcessChain link = contextChain->InheritChain(mdl, contextChain);//(mdl, contextChain, this);

		_LinkContext(link);

		record = mdl->HandleToken(this, rec_nest);



		_UnlinkContext();
	}


	bool Parser::_TryModule(Record& out, Record* rec_nest, ParseModule* mdl)
	{
		//This is where the above module checks should go. Uses execute module
		// also note, try should probably not check the current context, as it's likely the one who fired it. 
		// this allows more control where there are situations where the only ways a thing happens is if the parser is in control.

		//Mesures first if the module can handle the current situation, but then measures if the currently loaded context will allow such a thing.

		bool module_check = mdl->CanHandle(this, rec_nest);
		bool context_check = module_check ? contextChain->current->ContextAllowed(mdl, contextChain) : false;
		bool handle = module_check && context_check;

		RGL_LOG(info, "try success? {} {}", module_check, context_check);
		RGL_LOG(info, "try module: {}, token: {}, success: {}", mdl->GetContext(), peek().GetTag(), handle);


		if (handle) {
			_ExecuteModule(out, rec_nest, mdl);
		}

		return handle;

	}


	bool Parser::_SearchModule(Record& out, Record* rec_nest)
	{
		//In truth, should access the ParseHandler

		std::vector<ParseModule*> module_list = ParseHandler::GetModuleList();
		for (ParseModule* mod : module_list)
		{
			bool success = _TryModule(out, rec_nest, mod);

			if (success)
				return true;
		}

		return false;

	}



	Record Parser::_ParseTopLevel()
	{
		//I think this function could actually be allowed to be free.
		if (std::addressof(_init) != contextChain)
			croak("Cannot parse top level while parsing.");

		if (!contextChain)//Should be warning?
			croak("ParseStream is expired, cannot parse top level.");


		if (!contextChain->current)
			croak("No top level ParseModule detected.");

		ParseModule* mdl = dynamic_cast<ParseModule*>(contextChain->current);

		//If this can't work, that's one weird occurance. Probably not going to account for that, I'd rather a crash.

		Record result = mdl->HandleToken(this, nullptr);

		if (!eof())
			croak("Top level file finished not EOF");

		//Severing the chain means that this string has been parsed to it's full ability, and (likely) parsed properly.
		// make this a function btw.
		contextChain = nullptr;

		return result;
	}



	RecordData Parser::next()
	{
		return tokenizer.next();
	}
	RecordData Parser::peek()
	{
		return tokenizer.peek();
	}

	RecordData Parser::prev()
	{
		return tokenizer.prev();
	}

	bool Parser::eof()
	{
		return tokenizer.eof();
	}
	void Parser::croak(std::string msg)
	{
		return GetInput()->croak(msg);
	}



	bool Parser::IsType(TokenType type, std::string str) {
		RGL_LOG(info, "hit");
		RecordData tok = tokenizer.peek();
		return tok && tok.GetEnum<Token>().type == type && (str == "" || tok.GetTag() == str);
	}

	void Parser::SkipType(TokenType type, std::string str)
	{
		//TODO:Fix Format #1
		//TokenToString
		if (IsType(type, str) == true) tokenizer.next();
		else tokenizer.croak("expecting type, got wrong type");//std::format("Expecting {}: \"{}\"", TokenToString(type, false), str));
	}



	std::vector<Record> Parser::Delimited(std::string start, std::string stop, std::string separator, std::function<ParseFunc> func) {
		//A single char might be a better delimiter
		std::vector<Record> result;

		bool first = true;
		SkipType(TokenType::Punctuation, start);
		while (!tokenizer.eof() == false) {
			if (IsType(TokenType::Punctuation, stop) == true) break;
			if (first) first = false; else SkipType(TokenType::Punctuation, separator);
			if (IsType(TokenType::Punctuation, stop) == true) break;

			result.push_back(func(this, nullptr));
		}
		SkipType(TokenType::Punctuation, stop);

		return result;
	}

	void Parser::unexpected() {
		//want this to look prettier at some point.
		//TODO:Fix Format #2
		tokenizer.croak(std::format("Unexpected token: {} ({})", tokenizer.peek().GetTag(), TokenToString(tokenizer.peek().TOKEN().type)));//"Unexpected token");//
	}


	ProcessChain Parser::GetChain()
	{
		return *contextChain;
	}

	Record Parser::ParseExpression() {
		bool success;

		int i = 0;

		Record result{};
		Record* nested = nullptr;
		do
		{
			success = _SearchModule(result, nested);

			//should only crash if atomic modules fails
			if (!success && !nested)
				unexpected();

			nested = &result;

			i++;



		} while (success && eof() == false);//eof to prevent searches once past the point.

		return result;
	}

	TokenStream* Parser::GetTokenizer()
	{
		return &tokenizer;
	}

	InputStream* Parser::GetInput()
	{
		return GetTokenizer()->GetInput();
	}


	std::string Parser::name()// const
	{
		return GetInput()->name();
	}

	std::string Parser::project()// const
	{
		return GetInput()->project();
	}

	//Would like to seperate these from parser
	Record Parser::CreateExpression(std::string str, Expression expr, std::vector<Record> children)
	{
		return Record{ str, expr, children };
	}

	//This has no reason to be in here specifically.
	Record Parser::CreateExpression(RecordData data, Expression expr, std::vector<Record> children)
	{
		auto tok = data.GetEnum<Token>();

		expr.line = tok.line;
		expr.column = tok.column;

		return Record{ data.GetTag(), expr, children };
	}


	Record Parser__::CreateSyntaxTree(std::string project, std::string name, std::string text, Column column, Line line, ParseModule* mdl)
	{
		//When parse is used, it should come with a type and a name. This type is what the top level type is. Only a few can be chosen,
		// and it will determine what can be compiled, and also how valid it is.

		InputStream inp_stream{ name, project, text, line, column };

		TokenStream tok_stream{ inp_stream };

		

		if (!mdl)
			mdl = ScriptParser::GetSingleton();

		Parser par_stream{ tok_stream,	mdl };

		try
		{
			Record result = par_stream._ParseTopLevel();

			return result;
		}
		catch (const ParsingError& parse_error)
		{
			//TODO: Actually report parsing error, and also return a malformed record that can detail what the fuck just happened.
			RGL_LOG(error, "{}", parse_error._tempWhat);
			return Parser::CreateExpression(parse_error._tempWhat, ExpressionType::Invalid);
		}


	}
}