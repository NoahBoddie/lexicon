#include "Lexicon/Engine/Parser.h"
#include "Lexicon/Engine/ParserTest.h"
//#include "CompileModule.h"
namespace LEX::Impl
{
	Parser::Parser(TokenStream& t, ParseModule* mdl) : tokenizer{ t } , _modules{ ParseHandler::BuildModules() }
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
		//logger::info("C {}", peek().GetTag());
		ProcessChain link = contextChain->InheritChain(mdl, contextChain);//(mdl, contextChain, this);

		_LinkContext(link);
		//logger::info("C {}", peek().GetTag());
		record = mdl->HandleToken(this, rec_nest);



		_UnlinkContext();
	}

	bool Parser::_QueryModule(Record* rec_nest, ParseModule* mdl, ParseFlag flag)
	{
		//This is where the above module checks should go. Uses execute module
		// also note, try should probably not check the current context, as it's likely the one who fired it.
		// this allows more control where there are situations where the only ways a thing happens is if the parser is in control.

		//Mesures first if the module can handle the current situation, but then measures if the currently loaded context will allow such a thing.

		bool module_check = mdl->CanHandle(this, rec_nest, flag);
		bool context_check = module_check ? contextChain->current->ContextAllowed(mdl, contextChain) : false;
		
		return module_check && context_check;
		
	}

	bool Parser::_TryModule(Record& out, Record* rec_nest, ParseModule* mdl, ParseFlag flag)
	{
		//This is where the above module checks should go. Uses execute module
		// also note, try should probably not check the current context, as it's likely the one who fired it. 
		// this allows more control where there are situations where the only ways a thing happens is if the parser is in control.

		//Mesures first if the module can handle the current situation, but then measures if the currently loaded context will allow such a thing.

		bool handle = _QueryModule(rec_nest, mdl, flag);
		
		if (handle)
		{
			//RGL_LOG(info, "try success? {} {}", module_check, context_check);
			logger::info("try module: {}, token: {}: target: {}, success: {}", mdl->GetContext(), peek().GetTag(), !!rec_nest, handle);
		}

		if (handle) {
			_ExecuteModule(out, rec_nest, mdl);
		}

		return handle;

	}


	bool Parser::_SearchModule(Record& out, Record* rec_nest, bool atomic)
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
			bool success = _TryModule(out, rec_nest, mod.get(), atomic ? ParseFlag::Atomic : ParseFlag::None);

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
	void Parser::croak(std::string msg, Token* token)
	{
		return GetInput()->croak(msg, token);
	}



	bool Parser::IsType(TokenType type, std::string_view str) {
		RecordData tok = tokenizer.peek();
		return tok && tok.GetEnum<Token>().type == type && (str == "" || tok.GetTag() == str);
	}

	bool Parser::WasType(TokenType type, std::string_view str) {
		RecordData tok = tokenizer.prev();
		return tok && tok.GetEnum<Token>().type == type && (str == "" || tok.GetTag() == str);
	}


	RecordData Parser::ConsumeType(TokenType type, std::string_view str)
	{
		//TODO:Fix Format #1
		//TokenToString
		if (IsType(type, str) == false) {
			auto token = peek();
			tokenizer.croak(std::format("Expecting {}, recieved {} from \"{}\"", magic_enum::enum_name(type), magic_enum::enum_name(token.TOKEN().type), token.GetTag()));
		}

		return tokenizer.next(); 
	}

	void Parser::SkipType(TokenType type, std::string_view str)
	{
		ConsumeType(type, str);
	}

	bool Parser::SkipIfType(TokenType type, std::string_view str)
	{
		bool result = IsType(type, str);
		
		if (result)
			tokenizer.next();

		return result;
	}

	std::vector<Record> Parser::Delimited(std::string_view start, std::string_view stop, std::function<void()> separator, std::function<ParseFunc> func) {
		//I would like a delimit that expects the seperator to be the second to last, and another that prevents it from being second to last.
		//Having a version of this with no parameters or just parser would be ideal, that way I can use member functions.

		//Checking for first might not really be that difficult.

		std::vector<Record> result;

		bool first = true;
		RGL_LOG(debug, "skipping start '{}'", start);
		SkipType(TokenType::Punctuation, start);
		while (tokenizer.eof() == false) {
			//bool start = first;

			if (IsType(TokenType::Punctuation, stop) == true) break;

			//RGL_LOG(debug, "skipping seperator '{}'", separator);

			if (first) first = false; 
			else if (separator) separator();

			RGL_LOG(debug, "delimit check b");

			if (IsType(TokenType::Punctuation, stop) == true) break;

			//Thinking of using the second arg for something.
			//Record entry = start && begin ? begin(this, nullptr) : func(this, nullptr);
			Record entry = func(this, nullptr);

			if (entry) {
				RGL_LOG(debug, "push back function");
				result.push_back(entry);
			}
			else {
				RGL_LOG(debug, "empty record discared");
			}
		}

		RGL_LOG(debug, "skipping end '{}'", stop);
		SkipType(TokenType::Punctuation, stop);

		return result;
	}




	//Make a version of this that can take a parser and record, and a version that can take nothing at all.
	std::vector<Record> Parser::Delimited(std::string_view start, std::string_view stop, std::string_view separator, std::function<ParseFunc> func) {
		return Parser::Delimited(start, stop, [&]() { SkipType(TokenType::Punctuation, separator); }, func);
	}

	

	void Parser::unexpected() {
		//want this to look prettier at some point.
		//TODO:Fix Format #2
		tokenizer.croak(std::format("Unexpected token: {} ({})", tokenizer.peek().GetTag(), magic_enum::enum_name(tokenizer.peek().TOKEN().type)));//"Unexpected token");//
	}


	ProcessChain Parser::GetChain()
	{
		return *contextChain;
	}

	Record Parser::_Parse(bool atomic) {
		bool success;

		int i = 0;

		Record result{};
		Record* nested = nullptr;
		do
		{
			
			success = _SearchModule(result, nested, atomic);

			//should only crash if atomic modules fails
			if (!success && !nested)
				unexpected();

			nested = &result;

			i++;
		} while (success && eof() == false);//eof to prevent searches once past the point.

		return result;
	}


	Record Parser::ParseAtomic() {
		Record result = _Parse(true);
		return result;
	}

	Record Parser::ParseExpression() {
		Record result = _Parse(false);
		return result;
	}

	Record Parser::EndExpression(Record& rec)
	{
		ParseModule::ExecuteModule<EndParser>(this, nullptr);
		return std::move(rec);
	}


	Record Parser::EndExpression(Record&& rec)
	{
		ParseModule::ExecuteModule<EndParser>(this, nullptr);
		return std::move(rec);
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
	// Would also like to make "create header" for this.
	Record Parser::CreateExpression(std::string str, Syntax expr, std::vector<Record> children)
	{
		return Record{ str, expr, children };
	}

	//This has no reason to be in here specifically.
	Record Parser::CreateExpression(RecordData data, Syntax expr, std::vector<Record> children)
	{
		auto tok = data.GetEnum<Token>();

		expr.line = tok.line;
		expr.column = tok.column;

		return Record{ data.GetTag(), expr, std::move(children) };
	}


	Record Parser__::CreateSyntax(std::string text, ParseModule* mdl, Column column, Line line)
	{
		//When parse is used, it should come with a type and a name. This type is what the top level type is. Only a few can be chosen,
		// and it will determine what can be compiled, and also how valid it is.

		InputStream inp_stream{ "<untitled>", "<loose>", text, ParseMode::kBasic, line, column};

		TokenStream tok_stream{ inp_stream };


		if (!mdl) {
			report::fault::critical("No parse module given bruv");
		}
		Parser par_stream{ tok_stream,	mdl };

		try
		{
			Record result = par_stream._ParseTopLevel();

			return result;
		}
		catch (const ParsingError& parse_error)
		{
			//This should still include the name if possible.
			//TODO: Actually report parsing error, and also return a malformed record that can detail what the fuck just happened.
			RGL_LOG(error, "{}", parse_error._tempWhat);
			return Parser::CreateExpression(parse_error._tempWhat, SyntaxType::Invalid, Parser::CreateExpression("nameless", SyntaxType::Invalid));
		}


	}
	
	Record Parser__::CreateSyntaxTree(std::string project, std::string name, std::string text, ParseModule* mdl, Column column, Line line)
	{
		//When parse is used, it should come with a type and a name. This type is what the top level type is. Only a few can be chosen,
		// and it will determine what can be compiled, and also how valid it is.

		std::unique_ptr<ParseModule> new_mod;

		if (!mdl) {
			new_mod = std::make_unique<ScriptParser>();
			mdl = new_mod.get();
		}


		InputStream inp_stream{ name, project, text, mdl->GetParseMode(), line, column };

		TokenStream tok_stream{ inp_stream };



		Parser par_stream{ tok_stream,	mdl };

		try
		{
			Record result = par_stream._ParseTopLevel();

			return result;
		}
		catch (const ParsingError& parse_error)
		{
			//This should still include the name if possible.
			//TODO: Actually report parsing error, and also return a malformed record that can detail what the fuck just happened.
			RGL_LOG(error, "{}", parse_error._tempWhat);
			return Parser::CreateExpression(parse_error._tempWhat, SyntaxType::Invalid, Parser::CreateExpression(name, SyntaxType::Invalid));
		}


	}
}



/*

	//Make a version of this that can take a parser and record, and a version that can take nothing at all.
std::vector<Record> Parser::Delimited(std::string start, std::string stop, std::string separator, std::function<ParseFunc> func) {
	//I would like a delimit that expects the seperator to be the second to last, and another that prevents it from being second to last.
	//Having a version of this with no parameters or just parser would be ideal, that way I can use member functions.

	//Checking for first might not really be that difficult.

	std::vector<Record> result;

	bool first = true;
	RGL_LOG(debug, "skipping start '{}'", stop);
	SkipType(TokenType::Punctuation, start);
	while (tokenizer.eof() == false) {
		//bool start = first;

		if (IsType(TokenType::Punctuation, stop) == true) break;

		RGL_LOG(debug, "skipping seperator '{}'", separator);

		if (first) first = false; else SkipType(TokenType::Punctuation, separator);

		RGL_LOG(debug, "delimit check b");

		if (IsType(TokenType::Punctuation, stop) == true) break;

		//Thinking of using the second arg for something.
		//Record entry = start && begin ? begin(this, nullptr) : func(this, nullptr);
		Record entry = func(this, nullptr);

		if (entry) {
			RGL_LOG(debug, "push back function");
			result.push_back(entry);
		}
		else {
			RGL_LOG(debug, "empty record discared");
		}
	}

	RGL_LOG(debug, "skipping end '{}'", stop);
	SkipType(TokenType::Punctuation, stop);

	return result;
}


//*/