#pragma once

#include "Lexicon/Engine/ProcessContext.h"
#include "Lexicon/Engine/TokenStream.h"
#include "Lexicon/Engine/Expression.h"
#include "Lexicon/Engine/ExpressionType.h"
#include "Lexicon/Engine/ParseHandler.h"

#include "Lexicon/Engine/ParseModule.h"

namespace LEX::Impl
{

	
	//TODO:Change name from "Parser" (which will be the static parsing class) to "ParseStream" which is more reflective of what it does.
	// As this isn't actually the thing doing any of the parsing anyhow.
	class Parser : public IProcess
	{
		//Friends with just the parse module
		friend class Parser__;
		friend class ParseModule;

		Parser(TokenStream& t, ParseModule* mdl);

		//parser should come with a context, this shit aint 
		Parser(TokenStream&& t, ParseModule* mdl);
		


		void _LinkContext(ProcessChain& chain);

		void _UnlinkContext();

		void _ExecuteModule(Record& record, Record* rec_nest, ParseModule* mdl);

		bool _QueryModule(Record* rec_nest, ParseModule* mdl, ParseFlag flag);

		bool _TryModule(Record& out, Record* rec_nest, ParseModule* mdl, ParseFlag flag);


		bool _SearchModule(Record& out, Record* rec_nest, bool atomic);


		Record _Parse(bool atomic);


		Record _ParseTopLevel();


	public:
		


		//make an explicit skip, so I can ensure that next doesn't throw stuff away.
		RecordData next();
		RecordData peek();

		RecordData prev();

		bool eof();
		void croak(std::string msg, Token* token = nullptr);



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
		std::vector<Record> Delimited(std::string start, std::string stop, std::string separator, std::function<Record(Parser*)> func)
		{
			return Delimited(start, stop, separator, [=](Parser* a1, auto) { return func(a1); });
		}



		//A shorthand so new lambdas don't need to get made contantly to send arg-less calls.
		std::vector<Record> Delimited(std::string start, std::string stop, std::function<void()> separator, std::function<Record()> func)
		{
			//std::function<ParseFunc> _b = nullptr;

			//_b = begin ? [=](auto, auto) { return begin(); } : _b;

			return Delimited(start, stop, separator, [=](auto, auto) { return func(); });
		}
		


		//template <typename TClass>
		std::vector<Record> Delimited(std::string start, std::string stop, std::function<void()> separator, std::function<Record(Parser*)> func)
		{
			return Delimited(start, stop, separator, [=](Parser* a1, Record*) { return func(a1); });
		}


		//std::vector<Record>

		void unexpected();


		ProcessChain GetChain();


		//I think the above works like this. 
		// Search looks for a module.
		// Try will try to use that module
		// execute will execute, regardless of conditions so the last thing needs to check itself.



		


		//This is the only function that's actually required now.
		Record ParseExpression();

		Record ParseAtomic();
		

		Record EndExpression(Record& rec);
		Record EndExpression(Record&& rec);

		

		TokenStream* GetTokenizer();

		InputStream* GetInput();

		
		std::string name();

		//Would like to seperate these from parser(steam) and move it to Parser__ (to be named Parser)
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


	private:
		TokenStream& tokenizer;
		ProcessChain _init = CreateChain();//{ nullptr, nullptr, this };
		std::vector<std::unique_ptr<ParseModule>> _modules;


	public:
		//The idea is adding to this allows things to be registered regardless of order, or skip the line.
		// or really, anything. It's upto interpretation. But I wanted to use it for some preprocessors.
		//std::vector<Record> persistent;
		
		
		//Would like this to be private too honestly, and const when given.
		ProcessChain* contextChain = &_init;


		void FlagFailure()
		{
			_failure = true;
		}

		bool _failure = false;

		//Note, a statement like this would need parser memory
		//1 < 2 > (4);

		

		//TODO:An init function for Parser will need to take a parsemodule instead, which sets the base for init, as 
		// well as the entry parse module, who's carried out without consideration.
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


