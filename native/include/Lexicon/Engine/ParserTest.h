#pragma once

#include "Lexicon/Engine/Parser.h"
#include "HeaderSettings.h"
#include "parse_strings.h"

#include "Lexicon/Engine/OperatorSetting.h"

namespace LEX::Impl
{
	//ENCHAIN::ABACCCAA


	//Effectively all this abomination exists solely so it's some what automatic to add things.


	template<class P>
	struct PivotSingleton
	{
		//This is a pivot class singleton so you can impose a constraint or concept on.
		
		//Should it also additionally extends the class, just so it can have vtables to it?

	//protected:
		//Want to fire this off when initing 
		
	};




	template <class T1, class T2 = void>
	struct ClassSingleton : public PivotSingleton<T2>
	{
		using singleton_type = ClassSingleton<T1, T2>;

		using element_type = T1;
		using pointer_type = element_type*;




		static pointer_type GetSingleton()
		{
			return _singleton;
		}

	protected:
		// Doesn't actually have to override, just overload, for the same reason serializable object handles itself the way it does.
	
		void InitSingleton() {}

		ClassSingleton() = default;
	private:

		struct _init_class
		{
			//If it exists, it should crash.
			_init_class()
			{
				if (_singleton)
					return;

				_singleton = new element_type();
				_singleton->InitSingleton();
			}
		};

		friend class _init_class;


		inline static _init_class _init = _init_class{};
		inline static pointer_type _singleton = nullptr;
	};


	
	template <class T, bool Register = true>
	struct ParseSingleton_ : public ClassSingleton<T, ParseModule>, public ParseModule
	{
		//Allows a singleton to exist, but allows for it to not be registered, like in the case for parsing 
		// modules not found in the handler.


		// Doesn't actually have to override, just overload, for the same reason serializable object handles itself the way it does.
		//Instead of this, I could make it an operator of a privated type, like the init class. This way its a public
		// function with access of the type to access being the thing that prevents use.
		void InitSingleton()
		{
			if constexpr (Register) {
				//ParseHandler::AddModule(reinterpret_cast<ParseModule*>(this));
				ParseHandler::AddModule(this);
				//RGL_LOG(info, "Added parse module {}", GetContext());
			}
		}

		virtual std::string_view GetModuleName()
		{
			return typeid(T).name();
		}
		
		
		static std::string_view GetName()
		{
			return ClassSingleton<T, ParseModule>::GetSingleton()->GetModuleName();
			//return TypeName<T>::value;
		}

		//Move this to another class, because not all parsers will be a singleton likely, but I'll still want
		// this as a simple way to get their names and purpose.
		std::string_view GetContext() override
		{
			return GetName();
		}
		
	};
	


	namespace detail
	{
		struct _module_factory_init
		{
			//This doesn't need to be static
			_module_factory_init(ModuleBuilder ctor)
			{
				//move ctor to some place where it can be used.
				ParseHandler::AddBuilder(ctor);
			}
		};
	}

	template <typename T1, std::derived_from<ParseModule> T2 = ParseModule>
	struct AutoParser : public T2
	{

	public:
		static std::unique_ptr<ParseModule> GetParseModule()
		{
			return std::make_unique<T1>();
		}

	private:



		inline static detail::_module_factory_init _init = GetParseModule;
	public:
	


		virtual std::string_view GetModuleName()
		{
			return typeid(T1).name();
		}


		static std::string_view GetName()
		{
			return {};//return ClassSingleton<T1, ParseModule>::GetSingleton()->GetModuleName();
			//return TypeName<T>::value;
		}

		//Move this to another class, because not all parsers will be a singleton likely, but I'll still want
		// this as a simple way to get their names and purpose.
		std::string_view GetContext() override
		{
			return {};//GetName();
		}

	};

	


#define PARSE_AUTO(mc_parser, ...) mc_parser : public AutoParser<mc_parser##__VA_OPT__(, public)__VA_ARGS__>


	//struct PARSE_AUTO(ThingParser){};


		inline void ClipString(std::string& str, size_t left, size_t right)  //needs to be able to clip the other side.
		{
			if (!left && !right)
				return;

			size_t shift = str.size() - left - right;

			str = str.substr(left, shift);
		}




		struct EndParser : public AutoParser<EndParser>
		{
			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
				return !target && parser->IsType(TokenType::Punctuation, ";");
			}

			Record HandleToken(Parser* parser, Record*) override
			{
				//At least one is required.
				parser->SkipType(TokenType::Punctuation, ";");

				while (parser->SkipIfType(TokenType::Punctuation, ";") == true);

				return {};
			}


			std::string FailureMessage() const override
			{
				return "Expected ; punctuation.";
			}

		};
		


		struct EncapsulateParser : public AutoParser<EncapsulateParser>
		{
			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
				//{} can only go in code blocks, while () can only go in expression blocks.
				//For this, I need better keywords, and the instantiables.
				if (target)// || parser->contextChain->HasKeyword("code_block") == false)
					return false;


				if (parser->IsType(TokenType::Punctuation, "{") == true) {
					return !(flag & ParseFlag::Atomic);
				}
				else {
					return parser->IsType(TokenType::Punctuation, "(");
				}
			}

			bool SkipIfOrTarget(Parser* parser, TokenType type, std::string_view view, Record* target)
			{
				if (!target)
					return parser->SkipIfType(type, view);
				else
					return target->GetView() == view;
			}

			Record HandleToken(Parser* parser, Record* target) override
			{
				Record result;



				if (SkipIfOrTarget(parser, TokenType::Punctuation, "(", target) == true) {
					result.SYNTAX().type = SyntaxType::ExpressBlock;
					//result.GetTag() = parse_strings::expression_block;

					result.EmplaceChild(parser->ParseExpression());
					//result = parser->ParseExpression();

					parser->SkipType(TokenType::Punctuation, ")");
				}
				else {
					result.SYNTAX().type = SyntaxType::StateBlock;
					//result.GetTag() = parse_strings::statement_block;

					//std::vector<Record> children = parser->Delimited("{", "}", ";", &Parser::ParseExpression);
					std::vector<Record> children = parser->Delimited(target ? "" : "{", "}", [&]() { Record out; ParseModule::QueryModule<EndParser>(parser, out, nullptr); }, &Parser::ParseExpression);
					if (auto size = children.size(); !size)
						return {};
					else if (size == 1)
						result = children[0];
					else
						result.EmplaceChildren(std::move(children));
				}

				return result;
			}


			bool ContextAllowed_(ProcessContext* context, ProcessChain*)//from parenthesis parser.
			{
				//parenthesis will not process statements, so no if's no switches, no functions, etc. Probably want to do something to prevent it from handling types as well.
				// But I'm kinda lazy ngl
				return !context->IsContextType("Statement");
			}


			bool IsAtomic() const override { return true; }
		};


		struct ErrorParser : ParseModule
		{
			//This only really works if something is there to recover from it. Otherwise, it will toss whole variables, functions, etc.

			bool CanHandle(Parser*, Record*, ParseFlag) const override { return true; }



			struct EncapsuleData
			{
				//I think a vector of enums might work better if multiple case arise.
				uint32_t braces = 0;
			};

			bool IsSynchronizingToken(RecordData&& peek, Record* target, EncapsuleData& data) const
			{

				if (peek.TOKEN().type == TokenType::Punctuation)
				{
					switch (Hash(peek.GetView()))
					{
						//These are the universial punctuators
					case "}"_h:
						data.braces++;
					case ":"_h:
					case ";"_h:
						return true;
					}


					if (target) {
						return peek.GetView() == target->GetView();
					}
				}


				return false;
			}


			Record HandleToken(Parser* parser, Record* target) override
			{
				//I have a theory that when expecting a thing it will throw out input until it either encounters
				// said desired input, or will take any input at all

				//Seemingly it's a specific type of deal. Like, if you expect "expecting )" to do it, but in the above case it requires {.

				//so largely, it seems that there's specific modes that are given, but universially, ; is the thing that ends all of them.
				// Characters that seem to upset this process are puncutators like : and }. Possibly because they too end all things, but once it rolls over
				// it's still an unexpected character. So once it finds it's out character, do not get rid of it.

				//Also in accordance with the above, the beginning character must be evaluated as well.

				//Here's one problem though, these things have to encapsulate, so I think it needs to consume () {} <>

				//I think to safely do this I need to some how jump into the middle of the encapsulateParser.
				// Which to safely do that I need to be able to have the script/line parser to be nested.

				//		dhd
				//	parser->next ( heheh (jrjrjr)hheh 
				//auto query = parserknk-> job next  g jn35uobh35uog  %();

				parser->FlagFailure();

				EncapsuleData data;

				while (!parser->eof() && IsSynchronizingToken(parser->peek(), target, data) == false) {
					//Do a possible check here for unrecognized tokens


					parser->next();
				}

				if (parser->eof() == false)
				{
					if (data.braces) {
						try
						{
							Record brace = Parser::CreateExpression("{");
							return ParseModule::ExecuteModule<EncapsulateParser>(parser, &brace);
						}
						catch (ParseError error)
						{
							
							return HandleToken(parser, target);
						}
					}
				}
				return {};
			}

			std::string_view GetModuleName() override
			{
				return typeid(ErrorParser).name();
			}
		};



		struct ScriptParser : ParseModule//public ParseSingleton<ScriptParser, false>
		{
			//Script parser n
			std::optional<bool> GetKeywordState(std::string_view type) override
			{
				switch (Hash(type)) {
				case "decl_block"_h:
					return true;
				}

				return false;
			}

			bool CanHandle(Parser*, Record*, ParseFlag) const override { return false; }


			void HandleParse(Parser* parser, Record& script)
			{
				//Make this easier to use, make it a function called "Recoverable"
				try
				{
					//Make this a recursive function
					Record result = parser->ParseExpression();

					if (result)
						script.EmplaceChild(result);
					else
						report::parse::debug("disarding empty record");
				}
				catch (ParseError error)
				{
					ParseModule::ExecuteModule<ErrorParser>(parser, nullptr);
					
				}
			}

			Record HandleToken(Parser* parser, Record* target) override
			{
				Record script = Parser::CreateExpression(parser->name(), SyntaxType::Script);

				//script.EmplaceChild(Parser::CreateExpression(parser->project(), SyntaxType::Project));
				
				while (parser->eof() == false) {

					//Please put get line and column so I can store the line and column of this shit. Thanks.

					try
					{
						//Make this a recursive function
						Record result = parser->ParseExpression();

						if (result)
							script.EmplaceChild(result);
						else
							report::parse::trace("disarding empty record");
					}
					catch (ParseError error)
					{
						ParseModule::ExecuteModule<ErrorParser>(parser, nullptr);

					}

					
					//I would regardless like to end this off with ";" if possible
					//Also, this needs a function called Clear instead, where it will continuously loop until it no longer needs to skip.

					//if (parser->eof() == false) parser->SkipType(TokenType::Punctuation, ";");
				}
				//Column column() { return _column; }
				//Line line() { return _line; }
				
				script.SYNTAX().column = parser->GetInput()->column();
				script.SYNTAX().line = parser->GetInput()->line();

				return script;
			}


			//Need some boilerplate resolver.
			std::string_view GetModuleName() override
			{
				return typeid(ScriptParser).name();
			}

		};

		struct LineParser : ParseModule
		{

			bool CanHandle(Parser*, Record*, ParseFlag) const override { return false; }

			Record HandleToken(Parser* parser, Record* target) override
			{
				try
				{
					auto result = parser->ParseExpression();


					if (parser->eof() == false)
						//This is a fault
						report::parse::error("End of line not met after line finished parsing. line and column later");


					return result;
				}
				catch (ParseError& error)
				{
					//Now, it can actually submit a message, but I'll not care right now.
					ParseModule::ExecuteModule<ErrorParser>(parser, nullptr);
					return {};
				}
			}


			//Need some boilerplate resolver.
			std::string_view GetModuleName() override
			{
				return typeid(LineParser).name();
			}

		};


		struct IdenDeclBoilerPlate
		{
			Record _HandlePath(Parser* parser, SyntaxType enforced_type)
			{

				RecordData left = parser->ConsumeType(TokenType::Identifier);
				
				if (parser->SkipIfType(TokenType::Punctuation, "::") == false)
					return Parser::CreateExpression(left, SyntaxType::Identifier);

				//Scope name works for this sort of situation.
				Record path = Parser::CreateExpression(parse_strings::path, SyntaxType::Path);

				Record left_or_center;

				Record* current = nullptr;

				do {
					if (current) {
						current->EmplaceChild(Parser::CreateExpression(parse_strings::lhs, SyntaxType::Path, { left_or_center }));
						current = &current->EmplaceChild(Parser::CreateExpression(parse_strings::rhs, SyntaxType::Path));
					}

					left_or_center = Parser::CreateExpression(left, enforced_type);

					//parser->SkipType(TokenType::Punctuation, "::");

					left = parser->ConsumeType(TokenType::Identifier);

					if (!current) {
						current = &path;
					}

					//enforced type is a type that's changed into a different type depending.
					// If it was a project, next is a script.
					// if it was a script, next is a scope.
					// If scope, keeps being a scope.
					switch (enforced_type) {
					case SyntaxType::ProjectName:
						enforced_type = SyntaxType::Scriptname;
						break;
					case SyntaxType::Scriptname:
						enforced_type = SyntaxType::Typename;
						break;

					default:
						enforced_type = SyntaxType::Scopename;
						[[fallthrough]];

					case SyntaxType::Typename:
						//Typename feeds into itself
						[[fallthrough]];
					case SyntaxType::Scopename:
						//And so does scope name.
						break;
					}

				} while (parser->SkipIfType(TokenType::Punctuation, "::") == true);

				current->EmplaceChild(left_or_center);

				return Parser::CreateExpression(left, SyntaxType::Identifier, { path });

				//Whatever is right ends up being what ends up at the head field.
			}

			

		};



		struct IdentifierParser : public AutoParser<IdentifierParser>, public IdenDeclBoilerPlate
		{
			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
				if (target)
					return false;


				return parser->IsType(TokenType::Punctuation, "::") || parser->IsType(TokenType::Identifier) || parser->IsType(TokenType::Keyword, "this"); 

			}

			bool IsAtomic() const override
			{
				return true;
			}


			Record _HandleThis(Parser* parser)
			{
				RecordData next = parser->next();
				next.GetTag() = parse_strings::this_word;
				return Parser::CreateExpression(next, SyntaxType::Field);
			}


			Record HandleToken(Parser* parser, Record*) override
			{

				//if the presented field is an unscoped, it should attempt to put it in a box, to ensure that it's known that it's an outside field.


				if (parser->IsType(TokenType::Keyword, "this") == true) {
					return _HandleThis(parser);
				}
				else {
					SyntaxType enforced_type = SyntaxType::Scopename;

					if (parser->SkipIfType(TokenType::Punctuation, "::") == true) {
						if (parser->SkipIfType(TokenType::Punctuation, "::") == true)
							enforced_type = SyntaxType::ProjectName;
						else
							enforced_type = SyntaxType::Scriptname;
					}
					
					return _HandlePath(parser, enforced_type);
				}
				
				
			}



		};


		
		struct HeaderParser : public AutoParser<HeaderParser>
		{
			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
				bool direct = !!(flag & ParseFlag::Direct);

				//This can 
				if (target) {
					switch (target->SYNTAX().type) 
					{
					
					case SyntaxType::Declaration:
						if (!direct)//If this is not direct, this isn't allowed to go after a declaration. Must be manual.
							return false;
						[[fallthrough]];

					case SyntaxType::Identifier:
					
						break;

					default: 
						return false;
					}
					
				}
				auto peek = parser->peek();

				bool post = target && target->SYNTAX().type == SyntaxType::Declaration;

				if (!post && direct && peek.TOKEN().type == TokenType::Identifier)
					return true;
				else
					return peek.TOKEN().type == TokenType::Keyword && IsKeyword(peek.GetTag(), post);
			}
			Record HandleToken(Parser* parser, Record* target) override
			{
				bool post = target && target->SYNTAX().type == SyntaxType::Declaration;

				bool name_taken = !post && target && target->SYNTAX().type == SyntaxType::Identifier;

				Record* prev_header = post && target ? target->FindChild(parse_strings::header) : nullptr;

				Record _make;


				if (!post) 
				{
					_make = Parser::CreateExpression(parse_strings::header, SyntaxType::Header, std::vector<Record>{ 3 });

					//Doing it like this ensures the order will not change.
					_make.GetChild(KeywordType::TypeQual) = Parser::CreateExpression(parse_strings::type_qualifier, SyntaxType::Header);
					_make.GetChild(KeywordType::DeclSpec) = Parser::CreateExpression(parse_strings::declare_specifier, SyntaxType::Header);
					_make.GetChild(KeywordType::TypeSpec) = Parser::CreateExpression(parse_strings::type_specifier, SyntaxType::Header);

					//auto& type_qual = 
					//auto& decl_spec = 
					//auto& type_spec = 

				} 
				else if (!prev_header) 
				{
					parser->croak("declaration has no header.");
				}


				Record& header = post ? *prev_header : _make;
			
				
				//remember, declare and declaration are the same thing.


				if (name_taken) {
					header.GetChild(KeywordType::TypeSpec).EmplaceChild(*target).SYNTAX().type = SyntaxType::Typename;
				}

				do {
					bool next = true;

					RecordData peek = parser->peek();
				
					
					auto type = peek.TOKEN().type;

					if (type == TokenType::Keyword) {
						get_switch (GetKeywordType(peek.GetTag(), post)) 
						{
						case KeywordType::DeclSpec:
							//I feel I could free a slot by doing something here.
							header.GetChild(KeywordType::DeclSpec).EmplaceChild(Parser::CreateExpression(peek, post ? SyntaxType::None : SyntaxType::Declare));
							break;

						case KeywordType::TypeQual:
							header.GetChild(KeywordType::TypeQual).EmplaceChild(Parser::CreateExpression(peek, SyntaxType::None));
							break;
						case KeywordType::TypeSpec:
							header.GetChild(KeywordType::TypeSpec).EmplaceChild(Parser::CreateExpression(peek, SyntaxType::None));
							if (IsTypename(peek.GetTag()) == true)
								name_taken = true;
							break;

						default:
							parser->croak("Not a proper keyword");
						}
					} 
					else// if (type == TokenType::Identifier || peek.GetTag() == "::")
					{
						//Here, if the type name is already taken and we hit identifier stuff, we just quit. 
						// this is the header, not the field or name. Declaration is to handle that.
						
						if (name_taken){
							//logger::debug("almost end{}", peek.GetTag());
							break;
						}

						Record identifier = ParseModule::ExecuteModule<IdentifierParser>(parser, nullptr);
						identifier.SYNTAX().type = SyntaxType::Typename;
						header.GetChild(KeywordType::TypeSpec).EmplaceChild(identifier);

						name_taken = true;
						
						next = false;
					} 
					//else {
					//	parser->croak("Not keyword or identifier");
					//}

					if (next) {
						auto _nxt = parser->next();
						//logger::debug(">!!!{}", _nxt.GetTag());
					}

				} while (parser->IsType(TokenType::Identifier) || CanHandle(parser, target, ParseFlag::Atomic) == true);  //This doesn't need to be "do-while"

				//logger::info("end {}", parser->peek().GetTag());

				//If we are in post, no way we're going to want to copy the entirety of  the created records. So just send an empty one and we'll toss it
				return post ? Record{} : header;
			}

		};


		struct DeclarationParser : public AutoParser<DeclarationParser>, IdenDeclBoilerPlate
		{
			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
				if (!target)
					return false;

				if (auto type = target->SYNTAX().type; type != SyntaxType::Header && type != SyntaxType::Identifier) {
					return false;
				}


				return parser->IsType(TokenType::Punctuation, "::") || parser->IsType(TokenType::Identifier);
			}


			
			Record HandleToken(Parser* parser, Record* target) override
			{
				Record header;

				if (target->SYNTAX().type == SyntaxType::Identifier) {
					header = ParseModule::TryModule<HeaderParser>(parser, target);
					target = &header;

					
				}
			
				

				//You'll notice this is just the Identifier parser. That it is.
				// This is ACTUALLY supposed to derive from it. However, since that's kinda hard to set up, 
				// boilerplate. At least until that gets resolved.
				SyntaxType enforced_type = SyntaxType::Scopename;
				

				if (parser->SkipIfType(TokenType::Punctuation, "::") == true) {
						enforced_type = SyntaxType::ProjectName;
				}
				
				auto result = _HandlePath(parser, enforced_type);
				
				result.SYNTAX().type = SyntaxType::Declaration;
				
				result.EmplaceChild(std::move(*target));

				return result;

			}

		};

		struct VariableParser : public AutoParser<VariableParser>
		{
			//Variable doesn't really seem to work name wise, as this thing works multiple jobs.

			uint32_t GetPriority() const override
			{
				return ModulePriority::High;
			}



			std::optional<bool> GetKeywordState(std::string_view type) override
			{
				switch (Hash(type)) {
				case "statement"_h:
				case "temp_variable"_h:
					return true;
				}

				return false;
			}

			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
				if (parser->contextChain->HasKeyword("temp_variable") == true)
					return false;

				if (target && target->SYNTAX().type == SyntaxType::Declare) {
					return parser->IsType(TokenType::Punctuation, ";") || parser->IsType(TokenType::Operator, "=");
				}

				return false;
			}

			Record HandleToken(Parser* parser, Record* target) override
			{
				target->SYNTAX().type = SyntaxType::Variable;

				if (parser->SkipIfType(TokenType::Operator, "=") == true) {
					//This should remove assignment from being a valid statement, and also allow assignment to happen
					// with the same expression.
					target->EmplaceChild(Record{ parse_strings::def_expression, SyntaxType::None, parser->ParseExpression() });
				}

				return parser->EndExpression(std::move(*target));
			}
		};


		struct FunctionParser : public AutoParser<FunctionParser>
		{
			//This is also a top level function. Use context to restrict placement.

			virtual ParseKeyword GetKeywords() override
			{
				return ParseKeyword::Statement;
			}

			std::string_view GetContext() override
			{
				return "FunctionStatement";
			}

			std::optional<bool> GetKeywordState(std::string_view type) override
			{
				switch (Hash(type)) {
				case "code_block"_h:
				case "statement"_h:
					return true;
				}

				return false;
			}

			uint32_t GetPriority() const override
			{
				return ModulePriority::High - 50;
			}

			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
				//Invalid if target is not present.
				// maybe enforce VarDeclare

				return target && target->SYNTAX().type == SyntaxType::Declaration && parser->IsType(TokenType::Punctuation, "(");
			}

			Record HandleToken(Parser* parser, Record* target) override
			{
				//Need to figure out how to parse a var name right here, that's a part of the function of a function parser.
				//To do it, perhaps it can call the things it needs in order. Variable parser, scriptname parser, variable parser, variable parser,
				// assignment

				//The target needs to exist, else croak.
				if (!target) {  //This bit is pretty much not possible. But still gonna keep.
					parser->croak("Expected identifier before scriptname punctuation (may be dev error)");
				} else if (target->SYNTAX().type != SyntaxType::Declare) {
					parser->croak("PLACEHOLDER, expected declaration.");
				}
				//The target type needs to be changed to function here.

				size_t post = 0;
				auto peek1 = parser->peek();

				auto _delegate = [&](Parser* parser, Record*) -> Record {
					//THIS version checks for the extension function syntax which changes what a static function extends. But it's rough, because I actually want
					// the internal syntax to exist without having to do "first_arg" all over the place.
					// I think instead what I need is another parser, one that only comes out to play when the current context is parsing Parameter.
					//The extension parser. It has to be branching, has to be a param type, and it has to have

					if (post++ == 0 && parser->SkipIfType(TokenType::Keyword, "this") == true) {
						
						Record result { parse_strings::extends, SyntaxType::None };
						
						//inspect if there's stuff in any of the other spaces and tell someone off for having them.
						auto peek = parser->peek();
						//header.GetChild(KeywordType::TypeSpec).EmplaceChild(Parser::CreateExpression(peek, SyntaxType::None));
						//result.EmplaceChild(ParseModule::TryModule<IdentifierParser>(parser, nullptr)).SYNTAX().type = SyntaxType::Typename;
						Record pull = ParseModule::TryModule<HeaderParser>(parser, nullptr);
						result.EmplaceChildren(std::move(pull.GetChild(KeywordType::TypeSpec).children()));

						target->EmplaceChildren(result);

						
						return {};
					}

					//ParseModule::QueryModule<Identifier

					Record result = ParseModule::TryModule<HeaderParser>(parser, nullptr);
					
					//For this next part identifiers aren't needed
					
					//We don't query identifier because we don't want that, we just want 1, not a path.
					if (parser->IsType(TokenType::Identifier) == true) {
						result = Parser::CreateExpression(parser->next(), SyntaxType::Variable, { result });
					}
					else {
						Syntax syntax = result.SYNTAX();
						syntax.type = SyntaxType::Variable;//type don't matter much if it's outside of a function. But that may change.

						result = Parser::CreateExpression(parse_strings::untitled, syntax, { result });
					}
					


					//Same as the bit for variable. if I can merge it, I would.
					if (parser->SkipIfType(TokenType::Operator, "=") == true) {
						result.EmplaceChild(Parser::CreateExpression(parse_strings::extends, SyntaxType::None, { parser->ParseExpression() }));
					}

					return result;
				};

				//I realize all this ain't super needed. To account for extension, all I need to do is remove the first entry if it's named this, HERE,
				// and copy type, change it's name to extends, place it on the target. and Pop it out. Before that, maybe check for other thingy mabobs.
				// I also note that the parser module is VERY hands on, so I should use that to sort out which is using this or not.
				// I can just have the lambda check for each thing named this after the first entry, and then cull it.
				//target->EmplaceChildren(Record{ "params", SyntaxType::Total, parser->Delimited("(", ")", ",", ParseModule::TryModule<ParameterParser>) });
				target->EmplaceChildren(Record{ parse_strings::parameters, SyntaxType::None, parser->Delimited("(", ")", ",", _delegate) });

				{
					//Doesn't matter if it's successful or not, just needs to append if there is one. 
					// The result of header called like this is a waste value that we discard.
					Record waste;
					
					ParseModule::QueryModule<HeaderParser>(parser, waste, target);
				}

				
				
				target->SYNTAX().type = SyntaxType::Function;

				if (parser->SkipIfType(TokenType::Punctuation, ";") == false) {
					target->EmplaceChildren(Record{ parse_strings::code, SyntaxType::None, parser->Delimited("{", "}",
						[&]() { Record out; ParseModule::QueryModule<EndParser>(parser, out, nullptr); },
						&Parser::ParseExpression) });

				}

				return std::move(*target);
			}

			//Context as a concept hits a snag here, because this would need 2.
		};


		
		struct BinaryParser : public AutoParser<BinaryParser>
		{
			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
				bool atomic = flag & ParseFlag::Atomic;

				//RecordData token = parser->peek();

				//If atomic, the only thing that can be accepted would be if next is an access operator.
				if (atomic && parser->IsType(TokenType::Operator, ".") == false)
					return false;

				bool is_binary = GetPrecedence(parser->peek().GetView(), OperatorType::Binary);//I don't know how to evaluate this part.

				//Target must not be binary either, because it's not possible to have 2 binary next to each other.
				return target && target->SYNTAX().type != SyntaxType::Declaration && 
					target->SYNTAX().type != SyntaxType::Identifier && parser->IsType(TokenType::Operator) && is_binary;
			}

			//TODO:Binary parser needs to be able to know be atomic, but also needs to know if the next operator is "." to be considered atomic.
			Record _HandleBinary(Parser* parser, Record left, int my_prec)
			{
				//Just making pretend right now.
				
				
				RecordData tar = parser->peek();

				//TODO:Binary parser will need to work with unary later on, please be advised.
				//

				int his_prec = GetPrecedence(tar.GetView(), OperatorType::Binary);
				if (his_prec) {
					

					if (his_prec > my_prec) {
						parser->next();

						//Used to use parse atom, be advised

						Record a_lhs = Parser::CreateExpression(parse_strings::lhs, SyntaxType::None, { left });

						//Record a_rhs = Parser::CreateExpression("right", ExpressionType::Header, { _HandleBinary(parser, parser->ParseExpression(), his_prec) });
						//Return to form with parse atomic.
						Record a_rhs = Parser::CreateExpression(parse_strings::rhs, SyntaxType::None, { _HandleBinary(parser, parser->ParseAtomic(), his_prec) });

						Record pivot = parser->CreateExpression(tar, SyntaxType::Binary, a_lhs, a_rhs);

						return _HandleBinary(parser, pivot, my_prec);
					}
				} 
				else if (tar.TOKEN().type == TokenType::Operator) {
					parser->croak("Target did not have precedence despite being an operator");
				}

				return left;
			}

			Record HandleToken(Parser* parser, Record* target) override
			{
				//TODO:Make BinaryParser work without a target for the equals use. (Or make that the rule of assign?)
				return _HandleBinary(parser, *target, -1);
			}
			bool ContextAllowed(ProcessContext* context, ProcessChain*) override
			{
				
				return &typeid(context) != &typeid(this) && !context->HasKeyword("statement");


				//It cant be in the middle of switches was something, but not accurate
				//TODO: Needs addressing, during what statement type should one not expect binary again?
				// Note, it only cares about the last one so it can do stuff in a parenthesis statement.
				return GetContext() != context->GetContext() && !context->IsContextType("Statement");
			}


			bool IsAtomic() const override
			{
				return true;
			}
		};


		
		
		struct LiteralParser : public AutoParser<LiteralParser>
		{
			uint32_t GetPriority() const override
			{
				return ModulePriority::High;
			}

			bool CanHandle(Parser* parser, Record* target, ParseFlag) const override
			{
				auto peek = parser->peek();

				//Require some of these be keywords
				switch (peek.TOKEN().type) {
				case TokenType::Boolean:
				case TokenType::Number:
				case TokenType::String:
					return !target;

				case TokenType::Object:
					return false;
					//return target && (target->SYNTAX().type == SyntaxType::Identifier || target->SYNTAX().type == SyntaxType::Scopename) && parser->IsType(TokenType::Object);

				default:
					return false;
				}
			}

			Record HandleToken(Parser* parser, Record* target) override
			{
				auto next = parser->next();

				auto& tag = next.GetTag();

				switch (next.TOKEN().type) {
				case TokenType::Boolean:
					{
						if (tag == "true") {
							next.GetTag() = parse_strings::true_value;
						}
						else if (tag == "maybe") {
							next.GetTag() = parse_strings::maybe_value;
						}
						else if (tag == "false") {
							next.GetTag() = parse_strings::false_value;
						}
						else {
							//TODO:Fix Format #5
							parser->croak("Invalid boolean");  //std::format("invalid boolean '{}' detected.", tag));
						}
						
						return parser->CreateExpression(next, SyntaxType::Boolean);
					}

				case TokenType::Number:
					{
						//This will need a different way to handle this.
						//How do you invalidate this?
						// Too many periods
						//unknown letters
						
						//Here's how I'll handle it. The amount of characters processed must equal the number of characters possible. if not, various errors
						// will occur based on what's found.
						// period, too many periods.
						// unknown character

						//Only time unknown character is allowed is if it's a float.
					
				
						try {
							//Should crash if the value isn't valid. the actual value doesn't matter too much, that will get sorted later.
							// All the matters is it passes the transfer.
							


							auto func = [&]()->Record
								{
									size_t processed = 0;

									auto test = std::stod(tag, &processed);

									auto remains = tag.size() - processed;
									//I'm going to be real, I don't know how to reverse this and I'm tired b.
									if (remains == 1 && tag[processed] == 'f' || !remains) {}
									else throw std::invalid_argument{ "Nothing to say" };

									return parser->CreateExpression(next, SyntaxType::Number);
								};

							try
							{
								//This is still entirely fucked up and terrible. But, I would like to try to reprise this at some point.

								size_t processed = 0;

								
								auto test = std::stoll(tag, &processed);

								if (tag.size() > processed) {
									test = std::stoll(tag, &processed, 16);
								
									if (tag.size() > processed)
										return func();
								}
								
								return parser->CreateExpression(next, SyntaxType::Integer);
							}
							catch (std::invalid_argument in_arg) {
								return func();
							}
							
						} catch (std::invalid_argument in_arg) {
							//TODO:Fix Format #3
							parser->croak("invalid string detected");  //std::format("invalid string {} detected. ({})", next.GetTag(), in_arg.what()));
							break;                                     //Throw happens above s
						}
					}

				case TokenType::Object:
					{
						//Clears the object markers (that aren't even currently used).

						target->SYNTAX().type = SyntaxType::Scopename;

						auto next = parser->next();

						auto& tag = next.GetTag();

						//Clears the ":{" and "}" items.
						ClipString(tag, 2, 1);

						return parser->CreateExpression(next, SyntaxType::Object, *target);
					}

				case TokenType::String:
					{
						//Clears the quotations.
						ClipString(tag, 1, 1);
						return parser->CreateExpression(next, SyntaxType::String);
					}

				default:
					//TODO:Fix Format #4
					parser->croak("Invalid token as literal");  //std::format("invalid token '{}' detected as a literal.", tag));
					break;
				}

				//Technically should be unreachable.
				throw nullptr;
			}

			//Atomic due to object literal parser
			bool IsAtomic() const override { return true; }
		};

	
		
		struct CallParser : public AutoParser<CallParser>
		{
			bool CanHandle(Parser* parser, Record* target, ParseFlag) const override
			{
				//VarUsage basically means no type allowed.
				return target && target->SYNTAX().type == SyntaxType::Identifier && parser->IsType(TokenType::Punctuation, "("); // && parser->contextChain->HasKeyword("code_block");
			}

			Record HandleToken(Parser* parser, Record* target) override
			{
				target->SYNTAX().type = SyntaxType::Call;

				//I believe this makes header the contained object, rather than creating an expresion. Also changed to use the version that takes functions with just 1 member.
				//target->EmplaceChildren(Record{ "args", ExpressionType::Header, parser->Delimited("(", ")", ",", [=](auto, auto) { return parser->ParseExpression(); }) });
				auto args = Parser::CreateExpression(parse_strings::args, SyntaxType::None, parser->Delimited("(", ")", ",", &Parser::ParseExpression));

				target->EmplaceChildren(args);

				return std::move(*target);
			}

			bool IsAtomic() const override { return true; }
		};

			
		struct ReturnParser : public AutoParser<ReturnParser>
		{
			//Variable doesn't really seem to work name wise, as this thing works multiple jobs.

			uint32_t GetPriority() const override
			{
				return ModulePriority::High;
			}

			bool CanHandle(Parser* parser, Record* target, ParseFlag) const override
			{
				return !target && parser->IsType(TokenType::Keyword, "return");
			}

			Record HandleToken(Parser* parser, Record*) override
			{
				//If we are not currently in a function parsing issue.
				Record ret = parser->CreateExpression(parser->next(), SyntaxType::Return);

				//whatever is cycling this will get the ; statement, but we're getting the expression inside.
				if (parser->IsType(TokenType::Punctuation, ";") == false)
					//ret.EmplaceChildren(ParseModule::TryModule<BinaryParser>(parser, target));
					ret.EmplaceChildren(parser->ParseExpression());

				return ret;
			}
		};

		
	
		struct UnaryParser : public AutoParser<UnaryParser>
		{
			//This shit complicated, I'm saving this shit for last.


			//TODO:Operators need to split, as it's pretty hard without.
			// precedence can handle this I think, instead of just raw numbers what I can do is use
			//

			bool CanHandle(Parser* parser, Record* target, ParseFlag) const override
			{
				//I can saw with a fair bit of certainty that this is not going to work well when encountering errors.
				// The unary CanHandle needs to be far more discriminating.

				
				//unary is a bit tricker, some things actually have shared symbols (like the minus) so I'll have to be specific
				if (parser->IsType(TokenType::Operator) == false)
					return false;

				if (GetPrecedence(parser->peek().GetView(), OperatorType::Unary) == 0) {
					return false;
				}

				//If previous cannot have been an operator.

				if (auto prev = parser->prev(); prev.TOKEN().type == TokenType::Operator && prev.GetTag() == ".")
					return false;
				
				return !target;
			}

			bool ContextAllowed(ProcessContext* context, ProcessChain* chain) override
			{
				Parser* parser = chain->process->As<Parser>();

				//If last isn't .

				//if (NULL_OP(parser)->prev().GetTag() == ".")
				//	return false;
				//RGL_LOG(info, "TEST {} {}", BinaryParser::GetSingleton()->GetContext() != context->GetContext(), NULL_OP(parser)->);
				//And if the parser isn't binary OR is a member access. (This is done to get all member accesses out of the way before it shoves it in it's body.
				
				//This is the problem quite simply it's used an outdated system.
				return parser->GetBuiltModule<BinaryParser>() != context || parser->IsType(TokenType::Operator, ".");
			}

			//Precedence mostly follows the thing that's closest to the body, with subscript being the highest of them,
			// and '.' being before all other types. Let's see how that goes down.
			Record HandleToken(Parser* parser, Record* target) override
			{
				//Currently there are 2 types. Left unary, right unary. They can be seperated by them having a target or not.
				// right ones will always have targets, left will never.

				//So in the case of unary on the left, it seeks to do the thing on the left first, then compile itself.
				//In the case of right, it will simply encapsulate the target within itself

				Record unary;

				//Note, it cannot process a dual right after, so it needs to confirm the next thing isn't a dual operator

				if (!target) {
					RecordData op = parser->next();

					//Not a strong name to make this a header, because it's handled pretty straight forward.
					// Also, more likely this seeks to parse atomic
					//Record body = parser->ParseExpression();
					Record body = parser->ParseAtomic();

					unary = parser->CreateExpression(op, SyntaxType::Unary, body);
				} 
				else {
					parser->croak("Fuck you");
				}

				return unary;
			}

			bool IsAtomic() const override { return true; }
		};

		

		struct TypeParser : public AutoParser<TypeParser>
		{
			//I may use this at some other point though.

			bool CanHandle(Parser* parser, Record* target, ParseFlag) const override
			{
				//Doesn't allow a previous target yet, but later attributes will be of consideration.
				if (target)
					return false;

				auto peek = parser->peek();

				if (peek.TOKEN().type != TokenType::Keyword)
					return false;

				switch (Hash(peek.GetTag())) 
				{
				case "struct"_h:
				case "class"_h:
				case "interface"_h:
					return true;

				default: 
					return false;

				}

				
				return !target && (parser->IsType(TokenType::Keyword, "struct") || parser->IsType(TokenType::Keyword, "class"));
			}

			static Record HandleInterfaceIndex(Parser* parser)
			{
				Record index = Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier), SyntaxType::None);

				if (parser->SkipIfType(TokenType::Punctuation, "::") == true) {
					if (parser->IsType(TokenType::Punctuation, "{") == true)
					{
						auto& parent = index.EmplaceChild(Parser::CreateExpression("args", SyntaxType::None));
					
						//if it wants to use brackets to define it easier basically.
						auto children = parser->Delimited("{", "}", ",", [](Parser* parser) { return Parser::CreateExpression(parser->next(), SyntaxType::None); });
						parent.EmplaceChildren(std::move(children));
					}
					else {
						index.EmplaceChild(Parser::CreateExpression(parser->ConsumeType(TokenType::Number), SyntaxType::Number));
					}
				} else {
					index.EmplaceChild(Parser::CreateExpression("0", SyntaxType::Number));
				}

				return index;
			}

			Record HandleToken(Parser* parser, Record* target) override
			{
				RecordData data_type = parser->next();

				Record result = Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier), SyntaxType::Type);


				Record& settings = result.EmplaceChild(Parser::CreateExpression(parse_strings::settings, SyntaxType::None));

				{
					//First setting is struct or class
				
					//ugly but it does what I want. Clean up later please.
					settings.EmplaceChild(Parser::CreateExpression(parse_strings::data_type, SyntaxType::None, { Parser::CreateExpression(data_type, SyntaxType::None) }));
					//settings.PushBackChild(Parser::CreateExpression(data_type, SyntaxType::Header));
				}

				if constexpr (1!=1)//when pigs fly, or in other words when I implement generics
				{
					//This is the section for generics at a later point. As it is not generic, it is empty.
					settings.EmplaceChild(Parser::CreateExpression(parse_strings::generic, SyntaxType::None));
				}




				
				//second setting is interface, bind, or regular. Optional

				bool requires_body = true;

				if (RecordData peek = parser->peek(); peek.TOKEN().type == TokenType::Keyword) {
					
					
					//Ugly as shit I know.
					Record& attach = settings.EmplaceChild(Parser::CreateExpression(parse_strings::attach, SyntaxType::None)).
						EmplaceChild(Parser::CreateExpression(parser->next(), SyntaxType::None));

					switch (Hash(peek.GetTag())) {
					case "bind"_h:  //Bind needs to push back a type name
						attach.EmplaceChild(ParseModule::TryModule<IdentifierParser>(parser, nullptr)).SYNTAX().type = SyntaxType::Scopename;
						break;

					case "intrinsic"_h:  //Intrinsic needs to push back a category name, and index.
					case "external"_h:   //external needs to push back category name and index.
						requires_body = false;
						attach.EmplaceChild(HandleInterfaceIndex(parser));
						break;

					default:
						parser->croak("Unexpected keyword used after class/struct type name.");
						break;
					}
				}

				//This is actually completely optional
				if (parser->SkipIfType(TokenType::Punctuation, ":") == true) {
					Record& derives = settings.EmplaceChild(Parser::CreateExpression(parse_strings::derives, SyntaxType::None));
					
					size_t after = 0;

					while (!parser->IsType(TokenType::Punctuation, "{") && !parser->IsType(TokenType::Punctuation, ";"))
					{
						//Should only check after the first loop.
						if (after++)
						{
							parser->SkipType(TokenType::Punctuation, ",");
						}

						Record decl_spec;
						Record* specifiers = nullptr;

						if (parser->IsType(TokenType::Keyword) == true) 
						{
							specifiers = &decl_spec;

							decl_spec = Parser::CreateExpression(parse_strings::declare_specifier, SyntaxType::Header);

							do 
							{
								auto next = parser->next();

								switch (Hash(next.GetTag())) {
								case "public"_h:
								case "private"_h:
								case "protected"_h:
								case "internal"_h:
									decl_spec.EmplaceChild(Parser::CreateExpression(next, SyntaxType::None));
									break;

								default:
									report::parse::error("Unknown keyword, expected access modifier.");
									break;
								}

								

							} while (parser->IsType(TokenType::Keyword) == true);
						}

						Record& inherit = derives.EmplaceChild(ParseModule::TryModule<IdentifierParser>(parser, nullptr));

						if (specifiers)
							inherit.EmplaceChild(std::move(*specifiers));

						
					}


				}

				//If no body, this shit requires an end.
				bool has_body = false;

				if (parser->SkipIfType(TokenType::Punctuation, "{") == true) {
					has_body = true;
					
					Record body = Parser::CreateExpression(parse_strings::body, SyntaxType::None);

					auto end_of_this = [&]() -> bool {
						return parser->SkipIfType(TokenType::Punctuation, "}");
					};

					while (end_of_this() == false) {
						body.EmplaceChild(parser->ParseExpression());

						//I would regardless like to end this off with ";" if possible
						//Also, this needs a function called Clear instead, where it will continuously loop until it no longer needs to skip.
						if (end_of_this() == false)
							parser->SkipType(TokenType::Punctuation, ";");
					}

					result.EmplaceChild(body);
				} else if (requires_body) {
					parser->croak("type requires a body.");
				}
				if (has_body)
					return result;
				else
					return parser->EndExpression(result);

			}
		};



		//*
		//Remove
		struct FormatParser : public AutoParser<FormatParser>
		{
			//The idea of this should be the lowest priority possible. It expects an identifier and if nothing else claims it then it's a field.



			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
				//Must have a target, the target must be an identifier, and mustn't have a path.
				if (!target)
					return false;

				return parser->IsType(TokenType::Format);
				//The rules are slightly more lax because Format really really always needs to happen here, bar no other.
				// so incorrect use is grounds for parser failure rather than anything else.


				return target && target->SYNTAX().type == SyntaxType::Identifier && parser->IsType(TokenType::Format);
			}

			Record HandleToken(Parser* parser, Record* target) override
			{
				if (target->SYNTAX().type != SyntaxType::Identifier) {
					parser->croak("expected identifier");
				}

				if (target->FindChild(parse_strings::path) != nullptr) {
					parser->croak("expected identifier without path");
				}

				


				RecordData format = parser->next();


				

				auto& tag = format.GetTag();
				
				if (tag.ends_with(parse_strings::format_end) == false) {
					parser->croak("format token must end with 'end_format'");
				}


				auto size = tag.size();
				
				auto offset = std::strlen(parse_strings::format_start);

				size -= offset;
				size -= std::strlen(parse_strings::format_end);

				tag = tag.substr(offset, size);


				
				
				target->SYNTAX().type = SyntaxType::None;
				
				return parser->CreateExpression(format, SyntaxType::Format, std::move(*target));
			}

			bool IsAtomic() const override
			{
				return true;
			}
		};
		//*/

		struct FieldParser : public AutoParser<FieldParser>
		{
			//The idea of this should be the lowest priority possible. It expects an identifier and if nothing else claims it then it's a field.

			
			uint32_t GetPriority() const override
			{
				return ModulePriority::None;
			}

			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{

				return target && target->SYNTAX().type == SyntaxType::Identifier;
			}

			Record HandleToken(Parser* parser, Record* target) override
			{
				target->SYNTAX().type = SyntaxType::Field;

				return std::move(*target);
			}

			bool IsAtomic() const override
			{
				return true;
			}

			bool CanParseEOF() const override
			{
				return true;
			}
		};

		struct IfParser : public AutoParser<IfParser>
		{
			//The idea of this should be the lowest priority possible. It expects an identifier and if nothing else claims it then it's a field.


			//uint32_t GetPriority() const override
			//{
			//	return ModulePriority::None;
			//}

			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
				return !target && parser->IsType(TokenType::Keyword, "if");
			}



			void HandleBlock(Record& block, Parser* parser)
			{


				if (auto is_if = parser->IsType(TokenType::Keyword, "if"); is_if || parser->SkipIfType(TokenType::Punctuation, ":") == true)
				{
					//This really shouldn't allow encasulate parser to go in. But for now this will handle.
					Record body = parser->ParseExpression();
					

					if (body.SYNTAX().type == SyntaxType::StateBlock)
						block.EmplaceChildren(std::move(body.children()));
					else if (body)
						block.EmplaceChildren(std::move(body));

					if (!is_if)
						ParseModule::TryModule<EndParser>(parser, nullptr);
				}
				else if (parser->IsType(TokenType::Punctuation, "{") == true)
				{
					Record body = ParseModule::TryModule<EncapsulateParser>(parser, nullptr);

					auto& children = body.children();


					if (body.SYNTAX().type == SyntaxType::StateBlock)
						block.EmplaceChildren(std::move(children));
					else if (body)
						block.EmplaceChildren(std::move(body));

					//if it's empty the statement block remains empty.
				}
				else
				{
					//croak
					parser->croak("invalid token found proceeding if statement.");
				}

			}



			Record HandleToken(Parser* parser, Record* target) override
			{
				//if:conditional
				//<express>
				//expression here
				//<state>
				//statement here
				//<else>
				//statement here

				Record result = Parser::CreateExpression(parser->next(), SyntaxType::If);
				

				

				parser->SkipType(TokenType::Punctuation, "(");

				Record& express_block = result.EmplaceChild(Parser::CreateExpression(parse_strings::expression_block, SyntaxType::None));

				express_block.EmplaceChild(parser->ParseExpression());

				parser->SkipType(TokenType::Punctuation, ")");

				Record& statement_block = result.EmplaceChild(Parser::CreateExpression(parse_strings::statement_block, SyntaxType::None));

			
				HandleBlock(statement_block, parser);

				if (parser->SkipIfType(TokenType::Keyword, "else") == true)
				{
					Record& else_block = result.EmplaceChild(Parser::CreateExpression(parse_strings::alternate_block, SyntaxType::None));
					HandleBlock(else_block, parser);
				}

				return result;
			}

		};
	



		struct CastParser : public AutoParser<CastParser>
		{
			//The idea of this should be the lowest priority possible. It expects an identifier and if nothing else claims it then it's a field.


			//uint32_t GetPriority() const override
			//{
			//	return ModulePriority::None;
			//}

			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
				return target && parser->IsType(TokenType::Keyword, "as");
			}





			Record HandleToken(Parser* parser, Record* target) override
			{
				auto next = parser->next();
				
				if (parser->peek().GetView() == "maybe")
				{
					next = parser->next();
				}

				Record cast = Parser::CreateExpression(next, SyntaxType::Cast);

				cast.EmplaceChild(Parser::CreateExpression(parse_strings::lhs, SyntaxType::None, { std::move(*target) }));
				cast.EmplaceChild(Parser::CreateExpression(parse_strings::rhs, SyntaxType::None, { ParseModule::TryModule<HeaderParser>(parser, nullptr) }));

				return cast;
			}

			bool IsAtomic() const override { return true; }


		};








		////////////////////////////////////////////
		//Preprocessors                          ///
		////////////////////////////////////////////







		struct PreprocessorParser : public ParseModule
		{
			std::optional<bool> GetKeywordState(std::string_view type) override
			{
				switch (Hash(type)) {
				case "preprocessor"_h:
					return true;
				}

				return false;
			}

			bool CanHandle(Parser*, Record*, ParseFlag) const override { return false; }

			std::string_view GetModuleName() override
			{
				return typeid(PreprocessorParser).name();
			}



			Record HandleToken(Parser* parser, Record* target) override
			{
				
				Record prep = Parser::CreateExpression(parse_strings::preprocessor, SyntaxType::Directive);

				while (parser->eof() == false) {
					auto next = parser->next();

					if (next.GetView() != "#") {
						continue;
					}
					
					auto result = parser->ParseExpression();

					parser->SkipType(TokenType::Whitespace, "\n");

					if (result)
						prep.EmplaceChild(result);
					else
						report::parse::debug("disarding empty record");
				}

				return prep;
			}



			bool ContextAllowed(ProcessContext* context, ProcessChain* link) override
			{
				return context->HasKeyword("preprocessor");
			}



			ParseMode GetParseMode() const override
			{
				return ParseMode::kPreprocess;
			}
		};



		struct PreprocessorModule : public ParseModule
		{
			std::optional<bool> GetKeywordState(std::string_view type) override
			{
				switch (Hash(type)) {
				case "preprocessor"_h:
					return true;
				}

				return false;
			}


			uint32_t GetPriority() const override final
			{
				return ModulePriority::Max;
			}

			virtual bool CanProcess(Parser* parser, Record*, ParseFlag) const = 0;

			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override final
			{
				return parser->contextChain->HasKeyword("preprocessor") && CanProcess(parser, target, flag);
			}

		};

		template <typename T>
		struct AutoProcessor : public AutoParser<T, PreprocessorModule>{};

		/*The Old version.
		struct RequiresParser : public AutoProcessor<RequiresParser>
		{
			bool CanProcess(Parser* parser, Record* target, ParseFlag) const override 
			{ 
				
				return !target && parser->IsType(TokenType::Keyword, "requires");
			}


			enum Mode
			{
				kScript,		//A script that's loaded in this directory
				kProject,	//A project exists
				kOption,		//Script was loaded with a particular compiler option.
				kDirectory,	//A file or folder loaded at this relative directory. Has to be loaded, meaning the sub directory has to be involved somehow.
				kMacro
			};


			Record MakeRequirement(Parser* parser, Mode mode)
			{
				std::string_view name;
				
				Record out;
				parse_strings;
				switch (mode)
				{
					case kScript:
					{
						name = parse_strings::script_req;

						auto next = parser->next();

						if (next.TOKEN().type == TokenType::String)
							ClipString(next.GetTag(), 1, 1);

						out = Parser::CreateExpression(next, SyntaxType::None);



						break;
					}
					case kProject:
					{
						name = parse_strings::project_req;
						
						parser->next();
						out = Parser::CreateExpression(parser->next(), SyntaxType::None);
						parser->next();
						break;
					}
					case kOption:
					{
						name = parse_strings::option_req;
						
						parser->next();

						out = Parser::CreateExpression(parser->next(), SyntaxType::None);
						break;
					}
					case kDirectory:
					{
						name = parse_strings::directory_req;
						parser->next();

						out = Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier), SyntaxType::None);
						
						Record* to = &out;

						while (parser->SkipIfType(TokenType::Operator, "/") == true)
						{
							to = &to->EmplaceChild(Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier), SyntaxType::None));
						}


						break;
					}
				}
				return Parser::CreateExpression(name, SyntaxType::Requirement, { out });
			}

			Record HandleToken(Parser* parser, Record* target) override
			{
				auto loc = parser->next();
				
				Mode mode;

				if (parser->IsType(TokenType::String) == true)
				{
					mode = kScript;
				}
				else if (parser->IsType(TokenType::Punctuation, "...") == true)
					mode = kDirectory;
				else if (parser->IsType(TokenType::Punctuation, "/:") == true)
					mode = kOption;
				else if (parser->IsType(TokenType::Operator, "<") == true)
					mode = kProject;
				else {
					parser->croak("unknown requirement");
					mode = kScript;
				}

				auto result = MakeRequirement(parser, mode);

				result.SYNTAX().line = loc.TOKEN().line;
				result.SYNTAX().column = loc.TOKEN().column;
				return result;
			}

		};
		//*/


		struct RelationParser : public AutoProcessor<RelationParser>
		{
			bool CanProcess(Parser* parser, Record* target, ParseFlag) const override
			{

				return !target && ( parser->IsType(TokenType::Keyword, "import") || parser->IsType(TokenType::Keyword, "include") );
			}


			Record HandleToken(Parser* parser, Record* target) override
			{
				auto relation = Parser::CreateExpression(parser->next(), SyntaxType::Relationship);

				auto next = parser->ConsumeType(TokenType::Identifier);


				relation.EmplaceChild(Parser::CreateExpression(next, SyntaxType::None));

				return relation;
			}

		};

		struct RequiresParser : public AutoProcessor<RequiresParser>
		{
			bool CanProcess(Parser* parser, Record* target, ParseFlag) const override
			{

				return !target && parser->IsType(TokenType::Keyword, "requires");
			}

			Record HandleToken(Parser* parser, Record* target) override
			{

				auto result = Parser::CreateExpression(parser->next(), SyntaxType::Requirement);


				auto& child = result.EmplaceChild(Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier, "option"), SyntaxType::Prefunc));

				parser->SkipType(TokenType::Punctuation, "(");

				child.EmplaceChild(Parser::CreateExpression(parser->next(), SyntaxType::Identifier));

				parser->SkipType(TokenType::Punctuation, ")");



				return result;
			}

		};

		struct PreIfParser : public AutoProcessor<PreIfParser>
		{
			bool CanProcess(Parser* parser, Record* target, ParseFlag) const override
			{
				return !target && (parser->IsType(TokenType::Keyword, "if") || parser->IsType(TokenType::Identifier, "endif"));
			}

			Record HandleConditional(Parser* parser)
			{
				auto result = Parser::CreateExpression(parser->next(), SyntaxType::Conditional);

				//This should allow for the /: punctuator instead.
				;




				auto& child = result.EmplaceChild(Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier, "option"), SyntaxType::Prefunc));
				
				parser->SkipType(TokenType::Punctuation, "(");

				child.EmplaceChild(Parser::CreateExpression(parser->next(), SyntaxType::Identifier));

				parser->SkipType(TokenType::Punctuation, ")");



				return result;
			}

			Record HandleEnd(Parser* parser)
			{
				auto result = Parser::CreateExpression(parser->next(), SyntaxType::Conditional);

		
				return result;
			}


			Record HandleToken(Parser* parser, Record* target) override
			{
				auto peek = parser->peek();

				if (peek.GetView() == "if")
				{
					return HandleConditional(parser);
				}
				else if (peek.GetView() == "endif")
				{
					return HandleEnd(parser);
				}

				parser->croak("shouldn't happen");
				return{};
			}

		};

}