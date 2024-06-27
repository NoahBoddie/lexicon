#pragma once

#include "Lexicon/Impl/Parser.h"
#include "HeaderSettings.h"
#include "parse_strings.h"

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
				while (parser->SkipIfType(TokenType::Punctuation, ";") == true);

				return {};
			}
		};
		
		struct ScriptParser : ParseModule//public ParseSingleton<ScriptParser, false>
		{
			//Script parser n
			bool HasKeyword(std::string_view type) override
			{
				switch (Hash(type)) {
				case "decl_block"_h:
					return true;
				}

				return false;
			}

			bool CanHandle(Parser*, Record*, ParseFlag) const override { return false; }

			Record HandleToken(Parser* parser, Record* target) override
			{
				Record script = Parser::CreateExpression(parser->name(), SyntaxType::Script);

				script.EmplaceChild(Parser::CreateExpression(parser->project(), SyntaxType::Project));

				while (parser->eof() == false) {
					RGL_LOG(info, "tag: {}", parser->peek().GetTag());
					Record result = parser->ParseExpression();

					if (result)
						script.EmplaceChild(result);
					else
						report::parse::debug("disarding empty record");

					//I would regardless like to end this off with ";" if possible
					//Also, this needs a function called Clear instead, where it will continuously loop until it no longer needs to skip.

					//if (parser->eof() == false) parser->SkipType(TokenType::Punctuation, ";");
				}

				return script;
			}


			//Need some boilerplate resolver.
			std::string_view GetModuleName() override
			{
				return typeid(ScriptParser).name();
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
				
					logger::info("peek {}", peek.GetTag());
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

			bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const override
			{
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

			bool HasKeyword(std::string_view type) override
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
				if (target)
					logger::critical("{} {}", target->GetTag(), (int)target->SYNTAX().type);
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
						result.EmplaceChildren(std::move(pull.GetChild(KeywordType::TypeSpec).GetChildren()));

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

				target->EmplaceChildren(Record{ parse_strings::code, SyntaxType::None, parser->Delimited("{", "}", 
					[&]() { Record out; ParseModule::QueryModule<EndParser>(parser, out, nullptr); }, 
					&Parser::ParseExpression) });

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

				bool is_binary = true;//I don't know how to evaluate this part.

				//Target must not be binary either, because it's not possible to have 2 binary next to each other.
				return target && target->SYNTAX().type != SyntaxType::Declaration && 
					target->SYNTAX().type != SyntaxType::Identifier && parser->IsType(TokenType::Operator) && is_binary;
			}

			//TODO:Binary parser needs to be able to know be atomic, but also needs to know if the next operator is "." to be considered atomic.
			Record _HandleBinary(Parser* parser, Record left, int my_prec)
			{
				//Just making pretend right now.
				//TODO:Need to make an class to manage precedence
				static std::map<std::string, int> PRECEDENCE{
					{ "=", 1 },
					{ "||", 2 },
					{ "&&", 3 },
					{ "<", 7 },
					{ ">", 7 },
					{ "<=", 7 },
					{ ">=", 7 },
					{ "==", 7 },
					{ "!=", 7 },
					{ "+", 10 },
					{ "-", 10 },
					{ "*", 20 },
					{ "/", 20 },
					{ "%", 20 },
					{ ".", 25 },
				};
				//constexpr auto t = '/'
				RecordData tar = parser->peek();

				//TODO:Binary parser will need to work with unary later on, please be advised.
				//

				auto it = PRECEDENCE.find(tar.GetTag());
				if (PRECEDENCE.end() != it) {
					int his_prec = it->second;

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
				return _HandleBinary(parser, *target, 0);
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

			Record HandleToken(Parser* parser, Record* target) override
			{
				Record result;
				
				

				if (parser->SkipIfType(TokenType::Punctuation, "(") == true) {
					result.SYNTAX().type = SyntaxType::ExpressBlock;
					//result.GetTag() = parse_strings::expression_block;
					
					result.EmplaceChild(parser->ParseExpression());
					//result = parser->ParseExpression();

					parser->SkipType(TokenType::Punctuation, ")");
				} else {
					result.SYNTAX().type = SyntaxType::StateBlock;
					//result.GetTag() = parse_strings::statement_block;

					//std::vector<Record> children = parser->Delimited("{", "}", ";", &Parser::ParseExpression);
					std::vector<Record> children = parser->Delimited("{", "}", [&]() { Record out; ParseModule::QueryModule<EndParser>(parser, out, nullptr); }, &Parser::ParseExpression);
					if (auto size = children.size(); !size)
						return {};
					else if (size == 1)
						result = children[0];
					else
						result.EmplaceChildren(children);
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

					report::parse::info("thing? {}", tag);
						try {
							//Should crash if the value isn't valid. the actual value doesn't matter too much, that will get sorted later.
							// All the matters is it passes the transfer.
							auto test = std::stod(tag);

							return parser->CreateExpression(next, SyntaxType::Number);
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

				bool is_unary = true;
				//unary is a bit tricker, some things actually have shared symbols (like the minus) so I'll have to be specific
				if (!is_unary || parser->IsType(TokenType::Operator) == false)
					return false;

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
				return parser->GetBuiltModule<BinaryParser>()->GetContext() != context->GetContext() || (NULL_OP(parser)->IsType(TokenType::Operator, "."));
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
					index.PushBackChild(Parser::CreateExpression(parser->ConsumeType(TokenType::Number), SyntaxType::Number));
				} else {
					index.PushBackChild(Parser::CreateExpression("0", SyntaxType::Number));
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
					settings.PushBackChild(Parser::CreateExpression(parse_strings::generic, SyntaxType::None));
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
						attach.PushBackChild(HandleInterfaceIndex(parser));
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

					result.PushBackChild(body);
				} else if (requires_body) {
					parser->croak("type requires a body.");
				}
				if (has_body)
					return result;
				else
					return parser->EndExpression(result);

			}
		};



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
		};


	
}