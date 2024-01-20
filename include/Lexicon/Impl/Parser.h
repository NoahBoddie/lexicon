#pragma once

#include "Lexicon/Impl/ProcessContext.h"
#include "Lexicon/Impl/TokenStream.h"
#include "Lexicon/Impl/Expression.h"
#include "Lexicon/Impl/ExpressionType.h"
#include "Lexicon/Impl/ParseHandler.h"


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


		bool _TryModule(Record& out, Record* rec_nest, ParseModule* mdl, bool atomic = false);


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



		bool IsType(TokenType type, std::string str = "");

		RecordData ConsumeType(TokenType type, std::string str = "");

		void SkipType(TokenType type, std::string str);
		
		bool SkipIfType(TokenType type, std::string str);

		//Would like something called required type which fulfills the role of next and istype

		std::vector<Record> Delimited(std::string start, std::string stop, std::string separator, std::function<ParseFunc> func);

		//A shorthand so new lambdas don't need to get made contantly to send arg-less calls.
		std::vector<Record> Delimited(std::string start, std::string stop, std::string separator, std::function<Record()> func)
		{
			return Delimited(start, stop, separator, [=](auto, auto) { return func(); });
		}

		//template <typename TClass>
		std::vector<Record> Delimited(std::string start, std::string stop, std::string separator, std::function<Record(Parser*)> func)
		{
			return Delimited(start, stop, separator, [=](Parser* a1, auto) { return func(a1); });
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
		//{
			//TODO: I need ParseAtomic back
			// it would basically serve as a flag, but the idea would be I wouldn't start processing operators, I'd only process the next thing as much to make a singular
			// series of tokens to make sense.
			//It is to be noted that everywhere within the parser test this is used will need to be altered.


			//TO handle this, I'll have a generic Parse function that switches an atomic flag on and off, and once complete sets the flag to what it was previously.


			//In atomic mode the first modules are evaluated, but for the auxiliary set they must be set to Atomic to be able to run. So I will not be needing a new flag 
			// for this.

			//To actually handle this, it will just be a variable on search,try, etc. Most things will not be trying to parse atomically, so I'll probably mark it
			// it with false by default.

			//Things that are atomic
			// Parenthesis
			// Casts/Calls
			// Objects?
			// Unary

			//return ParseExpression();
		//}

		

		TokenStream* GetTokenizer();

		InputStream* GetInput();

		
		std::string name();

		std::string project();

		//Would like to seperate these from parser(steam) and move it to Parser__ (to be named Parser)
		//Would also like to remove the expression from this, there's no reason for it to be.
		static Record CreateExpression(std::string str, Syntax expr = SyntaxType::Header, std::vector<Record> children = {});

		//This has no reason to be in here specifically.
		static Record CreateExpression(RecordData data, Syntax expr, std::vector<Record> children = {});



		template<std::same_as<Record>... Rs>requires (sizeof...(Rs) != 0)
		static Record CreateExpression(RecordData data, Syntax expr, Rs... records)
		{
			return CreateExpression(data, expr, { records... });
		}



	private:
		TokenStream& tokenizer;
		ProcessChain _init = CreateChain();//{ nullptr, nullptr, this };

	public:
		//The idea is adding to this allows things to be registered regardless of order, or skip the line.
		// or really, anything. It's upto interpretation. But I wanted to use it for some preprocessors.
		//std::vector<Record> persistent;
		
		
		//Would like this to be private too honestly, and const when given.
		ProcessChain* contextChain = &_init;

		//Note, a statement like this would need parser memory
		//1 < 2 > (4);

		

		//TODO:An init function for Parser will need to take a parsemodule instead, which sets the base for init, as 
		// well as the entry parse module, who's carried out without consideration.
	};

	//TODO:The initial use of parse (somehow depending) should be establishing that the top most record is a script.


	struct Parser__
	{
		static Record CreateSyntaxTree(std::string project, std::string name, std::string text, ParseModule* mdl = nullptr, Column column = 1, Line line = 1);
	};
	
}



/*
//Dump zone


//This "should" just parse front line single items.
		Record ParseAtom() {
			//I'm well considering making this a parser, just so the context goes off.
			//Correction, I did make it one.
			if (IsType(TokenType::Punctuation, "(") == true) {
				next();
				Record exp = ParseExpression();
				SkipType(TokenType::Punctuation, ")");
				return exp;
			}

			Record result{};


			if (_SearchModule(result, nullptr) == false)
				unexpected();

			_SearchModule(result, &result);

			return result;
		}

		//This is to be moved to a specific function, and it's value is that of total. It's a special parsing function that only 1 parser exists for,
		// the top level
		//Scratch that.
		Record parse_toplevel() {
			//True name is basically parse script
			Record script{};

			//Make an easier way to initialize a record as a type
			script.GetTag() = _name;//Set to the name of the scope
			script.GetEnum<Expression>().type = ExpressionType::Script;



			while (!tokenizer.eof()) {
				script.PushBackChild(ParseExpression());

				//I would regardless like to end this off with ";" if possible
				if (!tokenizer.eof()) SkipType(TokenType::Punctuation, ";");
			}
			return script;
		}

		//This is kinda sorta used to parse new scopes. Might not actually be required, could be a parsing module
		Record parse_prog() {
			//Moved to scope parser
			auto parse_func = [=](Parser*, Record*) -> Record
			{
				return ParseExpression();
			};
			Record script{};

			std::vector<Record> children = Delimited("{", "}", ";", parse_func);
			if (children.size() == 0) return {};
			if (children.size() == 1) return children[0];

			//Add children or fucking whatever
			return script;
		}

		//maybe binary is a parse module, do note.
		// Also note, this does not exist.
		Record maybe_binary(Record left, int my_prec) {
			//Just making pretend right now.
			std::map <std::string, int> PRECEDENCE{};

			bool tok = IsType(TokenType::Operator);
			if (tok) {
				RecordData tar = tokenizer.peek();
				int his_prec = PRECEDENCE[tar.GetTag()];
				if (his_prec > my_prec) {
					tokenizer.next();
					return maybe_binary(Record{ tar.GetTag(), tar.GetEnum<uint64_t>(),
						Record{"left", left },
						Record{"right", maybe_binary(ParseAtom(), his_prec)}
						}, my_prec);
				}
			}
			return left;
		}


		
		//Keeping this so I can figure out why the fuck maybe binary is called before maybe call?
		// So atomic has maybe call, so NOW i just need to know, WHY DOES THAT FUCKING HAVE SO MANY CALLS?
		//Found out, it was so that it could be used for binary op (probably).
		// keeping here for lack of certainty
		Record parse_expression() {
			//return _ExecuteModule()
			return maybe_call(function() {
				return maybe_binary(parse_atom(), 0);
			});
		}
		

//*/