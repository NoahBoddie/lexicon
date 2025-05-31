#pragma once

#include "Lexicon/Engine/Parser.h"
#include "HeaderSettings.h"
#include "parse_strings.h"

#include "Lexicon/Engine/OperatorSetting.h"

namespace LEX
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



	template <typename T1, StringLiteral Cat = "", std::derived_from<ParseModule> Module = ParseModule>
	class AutoParser : public Module//The idea behind the auto stream is that there are some par
	{
	private:
		static std::unique_ptr<ParseModule> GetParseModule() { return std::make_unique<T1>(); }

		inline static Module::SetBuilder _initBuild{ GetParseModule, Cat };

		std::string_view GetCategory() const
		{
			return Cat;
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
			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				return !target && stream->IsType(TokenType::Punctuation, ";");
			}

			Record HandleToken(ParsingStream* stream, Record*) override
			{
				//At least one is required.
				stream->SkipType(TokenType::Punctuation, ";");

				while (stream->SkipIfType(TokenType::Punctuation, ";") == true);

				return {};
			}


			std::string FailureMessage() const override
			{
				return "Expected ; punctuation.";
			}

		};
		


		struct EncapsulateParser : public AutoParser<EncapsulateParser>
		{
			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				//{} can only go in code blocks, while () can only go in expression blocks.
				//For this, I need better keywords, and the instantiables.
				if (target)// || stream->contextChain->HasKeyword("code_block") == false)
					return false;


				if (stream->IsType(TokenType::Punctuation, "{") == true) {
					return !(flag & ParseFlag::Atomic);
				}
				else {
					return stream->IsType(TokenType::Punctuation, "(");
				}
			}

			bool SkipIfOrTarget(ParsingStream* stream, TokenType type, std::string_view view, Record* target)
			{
				if (!target)
					return stream->SkipIfType(type, view);
				else
					return target->GetView() == view;
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				Record result;



				if (SkipIfOrTarget(stream, TokenType::Punctuation, "(", target) == true) {
					result.SYNTAX().type = SyntaxType::ExpressBlock;
					//result.GetTag() = parse_strings::expression_block;

					result.EmplaceChild(stream->ParseExpression());
					//result = stream->ParseExpression();

					stream->SkipType(TokenType::Punctuation, ")");
				}
				else {
					result.SYNTAX().type = SyntaxType::StateBlock;
					//result.GetTag() = parse_strings::statement_block;

					//std::vector<Record> children = stream->Delimited("{", "}", ";", &ParsingStream::ParseExpression);
					std::vector<Record> children = stream->Delimited(target ? "" : "{", "}", [&]() { Record out; ParseModule::TryModule<EndParser>(stream, out, nullptr); }, &ParsingStream::ParseExpression);
					if (auto size = children.size(); !size)
						return {};
					else if (size == 1)
						result = children[0];
					else
						result.EmplaceChildren(std::move(children));
				}

				return result;
			}



			bool IsAtomic() const override { return true; }
		};


		struct ErrorParser : public ParseModule
		{
			//This only really works if something is there to recover from it. Otherwise, it will toss whole variables, functions, etc.

			bool CanHandle(ParsingStream*, Record*, ParseFlag) const override { return true; }

			std::string_view GetCategory() const override { return ""; }

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


			Record HandleToken(ParsingStream* stream, Record* target) override
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
				// Which to safely do that I need to be able to have the script/line stream to be nested.

				//		dhd
				//	stream->next ( heheh (jrjrjr)hheh 
				//auto query = parserknk-> job next  g jn35uobh35uog  %();

				stream->FlagFailure();

				EncapsuleData data;

				while (!stream->eof() && IsSynchronizingToken(stream->peek(), target, data) == false) {
					//Do a possible check here for unrecognized tokens


					stream->next();
				}

				if (stream->eof() == false)
				{
					if (data.braces) {
						try
						{
							Record brace = ParsingStream::CreateExpression("{");
							return ParseModule::ExecuteModule<EncapsulateParser>(stream, &brace);
						}
						catch (ParseError error)
						{
							
							return HandleToken(stream, target);
						}
					}
				}
				return {};
			}

		};



		struct ScriptParser : ParseModule//public ParseSingleton<ScriptParser, false>
		{
			//Script stream n
			std::optional<bool> GetKeywordState(const std::string_view& type) override
			{
				switch (Hash(type)) {
				case "decl_block"_h:
					return true;
				}

				return false;
			}

			bool CanHandle(ParsingStream*, Record*, ParseFlag) const override { return false; }


			void HandleParse(ParsingStream* stream, Record& script)
			{
				//Make this easier to use, make it a function called "Recoverable"
				try
				{
					//Make this a recursive function
					Record result = stream->ParseExpression();

					if (result)
						script.EmplaceChild(result);
					else
						report::parse::debug("disarding empty record");
				}
				catch (ParseError error)
				{
					ParseModule::ExecuteModule<ErrorParser>(stream, nullptr);
					
				}
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				Record script = ParsingStream::CreateExpression(stream->name(), SyntaxType::Script);

				//script.EmplaceChild(ParsingStream::CreateExpression(stream->project(), SyntaxType::Project));
				
				while (stream->eof() == false) {

					//Please put get line and column so I can store the line and column of this shit. Thanks.

					try
					{
						//Make this a recursive function
						Record result = stream->ParseExpression();

						if (result)
							script.EmplaceChild(result);
						else
							report::parse::trace("disarding empty record");
					}
					catch (ParseError error)
					{
						ParseModule::ExecuteModule<ErrorParser>(stream, nullptr);

					}

					
					//I would regardless like to end this off with ";" if possible
					//Also, this needs a function called Clear instead, where it will continuously loop until it no longer needs to skip.

					//if (stream->eof() == false) stream->SkipType(TokenType::Punctuation, ";");
				}
				//Column column() { return _column; }
				//Line line() { return _line; }
				
				script.SYNTAX().column = stream->column();
				script.SYNTAX().line = stream->line();

				return script;
			}


		};

		struct LineParser : ParseModule
		{

			bool CanHandle(ParsingStream*, Record*, ParseFlag) const override { return false; }

			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				try
				{
					auto result = stream->ParseExpression();


					if (stream->eof() == false)
						//This is a fault
						report::parse::error("End of line not met after line finished parsing. line and column later");


					return result;
				}
				catch (ParseError& error)
				{
					//Now, it can actually submit a message, but I'll not care right now.
					ParseModule::ExecuteModule<ErrorParser>(stream, nullptr);
					return {};
				}
			}



		};


		struct IdenDeclBoilerPlate
		{
			Record _HandlePath(ParsingStream* stream, SyntaxType enforced_type)
			{

				RecordData left = stream->ConsumeType(TokenType::Identifier);
				
				if (stream->SkipIfType(TokenType::Punctuation, "::") == false)
					return ParsingStream::CreateExpression(left, SyntaxType::Identifier);

				//Scope name works for this sort of situation.
				Record path = ParsingStream::CreateExpression(parse_strings::path, SyntaxType::Path);

				Record left_or_center;

				Record* current = nullptr;

				do {
					if (current) {
						current->EmplaceChild(ParsingStream::CreateExpression(parse_strings::lhs, SyntaxType::Path, { left_or_center }));
						current = &current->EmplaceChild(ParsingStream::CreateExpression(parse_strings::rhs, SyntaxType::Path));
					}

					left_or_center = ParsingStream::CreateExpression(left, enforced_type);

					//stream->SkipType(TokenType::Punctuation, "::");

					left = stream->ConsumeType(TokenType::Identifier);

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

				} while (stream->SkipIfType(TokenType::Punctuation, "::") == true);

				current->EmplaceChild(left_or_center);

				return ParsingStream::CreateExpression(left, SyntaxType::Identifier, { path });

				//Whatever is right ends up being what ends up at the head field.
			}

			

		};



		struct IdentifierParser : public AutoParser<IdentifierParser>, public IdenDeclBoilerPlate
		{
			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				if (target)
					return false;


				return stream->IsType(TokenType::Punctuation, "::") || stream->IsType(TokenType::Identifier) || stream->IsType(TokenType::Keyword, "this"); 

			}

			bool IsAtomic() const override
			{
				return true;
			}


			Record _HandleThis(ParsingStream* stream)
			{
				RecordData next = stream->next();
				next.GetTag() = parse_strings::this_word;
				return ParsingStream::CreateExpression(next, SyntaxType::Field);
			}


			Record HandleToken(ParsingStream* stream, Record*) override
			{

				//if the presented field is an unscoped, it should attempt to put it in a box, to ensure that it's known that it's an outside field.


				if (stream->IsType(TokenType::Keyword, "this") == true) {
					return _HandleThis(stream);
				}
				else {
					SyntaxType enforced_type = SyntaxType::Scopename;

					if (stream->SkipIfType(TokenType::Punctuation, "::") == true) {
						if (stream->SkipIfType(TokenType::Punctuation, "::") == true)
							enforced_type = SyntaxType::ProjectName;
						else
							enforced_type = SyntaxType::Scriptname;
					}
					
					return _HandlePath(stream, enforced_type);
				}
				
				
			}



		};


		
		struct HeaderParser : public AutoParser<HeaderParser>
		{
			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
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

					//case SyntaxType::Boolean:
					//	if (target->GetView() != "maybe")
					//		return false;

					case SyntaxType::Identifier:
					
						break;

					default: 
						return false;
					}
					
				}
				auto peek = stream->peek();

				bool post = target && target->SYNTAX().type == SyntaxType::Declaration;

				if (!post && direct && peek.TOKEN().type == TokenType::Identifier)
					return true;
				else
					return peek.TOKEN().type == TokenType::Keyword && IsKeyword(peek.GetTag(), post);
			}
			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				bool post = target && target->SYNTAX().type == SyntaxType::Declaration;

				bool name_taken = !post && target && target->SYNTAX().type == SyntaxType::Identifier;

				Record* prev_header = post && target ? target->FindChild(parse_strings::header) : nullptr;

				Record _make;


				if (!post) 
				{
					_make = ParsingStream::CreateExpression(parse_strings::header, SyntaxType::Header, std::vector<Record>{ 3 });

					//Doing it like this ensures the order will not change.
					_make.GetChild(KeywordType::TypeQual) = ParsingStream::CreateExpression(parse_strings::type_qualifier, SyntaxType::Header);
					_make.GetChild(KeywordType::DeclSpec) = ParsingStream::CreateExpression(parse_strings::declare_specifier, SyntaxType::Header);
					_make.GetChild(KeywordType::TypeSpec) = ParsingStream::CreateExpression(parse_strings::type_specifier, SyntaxType::Header);

					//TODO: I want to make a function called ConsumeTarget. It's goal should be to eat the targets data and return a reference to the header.
					//if (target && target->GetView() == "maybe") {
					//	_make.GetChild(KeywordType::TypeQual).EmplaceChild(ParsingStream::CreateExpression(target->data(), SyntaxType::None));
					//}

					//auto& type_qual = 
					//auto& decl_spec = 
					//auto& type_spec = 

				} 
				else if (!prev_header) 
				{
					stream->croak("declaration has no header.");
				}


				Record& header = post ? *prev_header : _make;
			
				
				//remember, declare and declaration are the same thing.


				if (name_taken) {
					header.GetChild(KeywordType::TypeSpec).EmplaceChild(*target).SYNTAX().type = SyntaxType::Typename;
				}

				do {
					bool next = true;

					RecordData peek = stream->peek();
				
					
					auto type = peek.TOKEN().type;

					if (type == TokenType::Keyword) {
						get_switch (GetKeywordType(peek.GetTag(), post)) 
						{
						case KeywordType::DeclSpec:
							//I feel I could free a slot by doing something here.
							header.GetChild(KeywordType::DeclSpec).EmplaceChild(ParsingStream::CreateExpression(peek, post ? SyntaxType::None : SyntaxType::Declare));
							break;

						case KeywordType::TypeQual:
							header.GetChild(KeywordType::TypeQual).EmplaceChild(ParsingStream::CreateExpression(peek, SyntaxType::None));
							break;
						case KeywordType::TypeSpec:
							header.GetChild(KeywordType::TypeSpec).EmplaceChild(ParsingStream::CreateExpression(peek, SyntaxType::None));
							if (IsTypename(peek.GetTag()) == true)
								name_taken = true;
							break;

						default:
							stream->croak("Not a proper keyword");
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

						Record identifier = ParseModule::ExecuteModule<IdentifierParser>(stream, nullptr);
						identifier.SYNTAX().type = SyntaxType::Typename;
						header.GetChild(KeywordType::TypeSpec).EmplaceChild(identifier);

						name_taken = true;
						
						next = false;
					} 
					//else {
					//	stream->croak("Not keyword or identifier");
					//}

					if (next) {
						auto _nxt = stream->next();
						//logger::debug(">!!!{}", _nxt.GetTag());
					}

				} while (stream->IsType(TokenType::Identifier) || CanHandle(stream, target, ParseFlag::Atomic) == true);  //This doesn't need to be "do-while"

				//logger::info("end {}", stream->peek().GetTag());

				//If we are in post, no way we're going to want to copy the entirety of  the created records. So just send an empty one and we'll toss it
				return post ? Record{} : header;
			}

		};


		struct TypeofParser : public AutoParser<TypeofParser>
		{
			bool IsAtomic() const override
			{
				return true;
			}


			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				if (target)
					return false;

				return stream->IsType(TokenType::Keyword, "typeof");
			}



			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				Record result = ParsingStream::CreateExpression(stream->next(), SyntaxType::Typeof);//Turn into the main record

				result.GetTag().clear();//There's no need for the string atm, so just clean it out to save some space.

				stream->SkipType(TokenType::Punctuation, "(");

				result.EmplaceChild(ParseModule::UseModule<HeaderParser>(stream, nullptr));

				stream->SkipType(TokenType::Punctuation, ")");


				return result;
			}

		};




		struct DeclarationParser : public AutoParser<DeclarationParser>, IdenDeclBoilerPlate
		{

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				if (!target)
					return false;

				if (auto type = target->SYNTAX().type; type != SyntaxType::Header && type != SyntaxType::Identifier) {
					return false;
				}


				return stream->IsType(TokenType::Punctuation, "::") || stream->IsType(TokenType::Identifier);
			}


			
			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				Record header;

				if (target->SYNTAX().type == SyntaxType::Identifier) {
					header = ParseModule::UseModule<HeaderParser>(stream, target);
					target = &header;

					
				}
			
				

				//You'll notice this is just the Identifier stream. That it is.
				// This is ACTUALLY supposed to derive from it. However, since that's kinda hard to set up, 
				// boilerplate. At least until that gets resolved.
				SyntaxType enforced_type = SyntaxType::Scopename;
				

				if (stream->SkipIfType(TokenType::Punctuation, "::") == true) {
						enforced_type = SyntaxType::ProjectName;
				}
				
				auto result = _HandlePath(stream, enforced_type);
				
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



			std::optional<bool> GetKeywordState(const std::string_view& type) override
			{
				switch (Hash(type)) {
				case "statement"_h:
				case "temp_variable"_h:
					return true;
				}

				return false;
			}

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				if (stream->chain()->HasKeyword("temp_variable") == true)
					return false;

				if (target && target->SYNTAX().type == SyntaxType::Declare) {
					return stream->IsType(TokenType::Punctuation, ";") || stream->IsType(TokenType::Operator, "=");
				}

				return false;
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				target->SYNTAX().type = SyntaxType::Variable;

				if (stream->SkipIfType(TokenType::Operator, "=") == true) {
					//This should remove assignment from being a valid statement, and also allow assignment to happen
					// with the same expression.
					target->EmplaceChild(Record{ parse_strings::def_expression, SyntaxType::None, stream->ParseExpression() });
				}

				return stream->EndExpression(std::move(*target));
			}
		};


		struct FunctionParser : public AutoParser<FunctionParser>
		{
			//This is also a top level function. Use context to restrict placement.

			//virtual ParseKeyword GetKeywords() override
			//{
			//	return ParseKeyword::Statement;
			//}

			//std::string_view GetContext() override
			//{
			//	return "FunctionStatement";
			//}

			std::optional<bool> GetKeywordState(const std::string_view& type) override
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

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				//Invalid if target is not present.
				// maybe enforce VarDeclare

				return target && target->SYNTAX().type == SyntaxType::Declaration && stream->IsType(TokenType::Punctuation, "(");
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				//Need to figure out how to parse a var name right here, that's a part of the function of a function stream.
				//To do it, perhaps it can call the things it needs in order. Variable stream, scriptname stream, variable stream, variable stream,
				// assignment

				//The target needs to exist, else croak.
				if (!target) {  //This bit is pretty much not possible. But still gonna keep.
					stream->croak("Expected identifier before scriptname punctuation (may be dev error)");
				} else if (target->SYNTAX().type != SyntaxType::Declare) {
					stream->croak("PLACEHOLDER, expected declaration.");
				}
				//The target type needs to be changed to function here.

				size_t post = 0;
				auto peek1 = stream->peek();

				auto _delegate = [&](ParsingStream* stream, Record*) -> Record {
					//THIS version checks for the extension function syntax which changes what a static function extends. But it's rough, because I actually want
					// the internal syntax to exist without having to do "first_arg" all over the place.
					// I think instead what I need is another stream, one that only comes out to play when the current context is parsing Parameter.
					//The extension stream. It has to be branching, has to be a param type, and it has to have

					if (post++ == 0 && stream->SkipIfType(TokenType::Keyword, "this") == true) {
						
						Record result { parse_strings::extends, SyntaxType::None };
						
						//inspect if there's stuff in any of the other spaces and tell someone off for having them.
						auto peek = stream->peek();
						//header.GetChild(KeywordType::TypeSpec).EmplaceChild(ParsingStream::CreateExpression(peek, SyntaxType::None));
						//result.EmplaceChild(ParseModule::UseModule<IdentifierParser>(stream, nullptr)).SYNTAX().type = SyntaxType::Typename;
						Record pull = ParseModule::UseModule<HeaderParser>(stream, nullptr);
						result.EmplaceChildren(std::move(pull.GetChild(KeywordType::TypeSpec).children()));

						target->EmplaceChildren(result);

						
						return {};
					}

					//ParseModule::TryModule<Identifier

					Record result = ParseModule::UseModule<HeaderParser>(stream, nullptr);
					
					//For this next part identifiers aren't needed
					
					//We don't query identifier because we don't want that, we just want 1, not a path.
					if (stream->IsType(TokenType::Identifier) == true) {
						result = ParsingStream::CreateExpression(stream->next(), SyntaxType::Variable, { result });
					}
					else {
						Syntax syntax = result.SYNTAX();
						syntax.type = SyntaxType::Variable;//type don't matter much if it's outside of a function. But that may change.

						result = ParsingStream::CreateExpression(parse_strings::untitled, syntax, { result });
					}
					


					//Same as the bit for variable. if I can merge it, I would.
					if (stream->SkipIfType(TokenType::Operator, "=") == true) {
						result.EmplaceChild(ParsingStream::CreateExpression(parse_strings::extends, SyntaxType::None, { stream->ParseExpression() }));
					}

					return result;
				};

				//I realize all this ain't super needed. To account for extension, all I need to do is remove the first entry if it's named this, HERE,
				// and copy type, change it's name to extends, place it on the target. and Pop it out. Before that, maybe check for other thingy mabobs.
				// I also note that the stream module is VERY hands on, so I should use that to sort out which is using this or not.
				// I can just have the lambda check for each thing named this after the first entry, and then cull it.
				//target->EmplaceChildren(Record{ "params", SyntaxType::Total, stream->Delimited("(", ")", ",", ParseModule::UseModule<ParameterParser>) });
				target->EmplaceChildren(Record{ parse_strings::parameters, SyntaxType::None, stream->Delimited("(", ")", ",", _delegate) });

				{
					//Doesn't matter if it's successful or not, just needs to append if there is one. 
					// The result of header called like this is a waste value that we discard.
					Record waste;
					
					ParseModule::TryModule<HeaderParser>(stream, waste, target);
				}

				
				
				target->SYNTAX().type = SyntaxType::Function;

				if (stream->SkipIfType(TokenType::Punctuation, ";") == false) {
					target->EmplaceChildren(Record{ parse_strings::code, SyntaxType::None, stream->Delimited("{", "}",
						[&]() { Record out; ParseModule::TryModule<EndParser>(stream, out, nullptr); },
						&ParsingStream::ParseExpression) });

				}

				return std::move(*target);
			}

			//Context as a concept hits a snag here, because this would need 2.
		};

		struct BinaryParser : public AutoParser<BinaryParser>
		{
			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				bool atomic = flag & ParseFlag::Atomic;

				//RecordData token = stream->peek();

				//If atomic, the only thing that can be accepted would be if next is an access operator.
				if (atomic && stream->IsType(TokenType::Operator, ".") == false)
					return false;

				bool is_binary = GetPrecedence(stream->peek().GetView(), OperatorType::Binary);//I don't know how to evaluate this part.

				//Target must not be binary either, because it's not possible to have 2 binary next to each other.
				return target && target->SYNTAX().type != SyntaxType::Declaration && 
					target->SYNTAX().type != SyntaxType::Identifier && stream->IsType(TokenType::Operator) && is_binary;
			}

			//TODO:Binary stream needs to be able to know be atomic, but also needs to know if the next operator is "." to be considered atomic.
			Record _HandleBinary(ParsingStream* stream, Record left, int my_prec)
			{
				//Just making pretend right now.
				
				
				RecordData tar = stream->peek();

				//TODO:Binary stream will need to work with unary later on, please be advised.
				//

				int his_prec = GetPrecedence(tar.GetView(), OperatorType::Binary);
				if (his_prec) {
					

					if (his_prec > my_prec) {
						stream->next();

						//Used to use parse atom, be advised

						Record a_lhs = ParsingStream::CreateExpression(parse_strings::lhs, SyntaxType::None, { left });

						//Record a_rhs = ParsingStream::CreateExpression("right", ExpressionType::Header, { _HandleBinary(stream, stream->ParseExpression(), his_prec) });
						//Return to form with parse atomic.
						Record a_rhs = ParsingStream::CreateExpression(parse_strings::rhs, SyntaxType::None, { _HandleBinary(stream, stream->ParseAtomic(), his_prec) });

						Record pivot = stream->CreateExpression(tar, SyntaxType::Binary, a_lhs, a_rhs);

						return _HandleBinary(stream, pivot, my_prec);
					}
				} 
				else if (tar.TOKEN().type == TokenType::Operator) {
					stream->croak("Target did not have precedence despite being an operator");
				}

				return left;
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				//TODO:Make BinaryParser work without a target for the equals use. (Or make that the rule of assign?)
				return _HandleBinary(stream, *target, -1);
			}
			bool ContextAllowed(ParseModule* module, ModuleChain*) override
			{
				
				return module !=  this && !module->HasKeyword("statement");
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

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag) const override
			{
				auto peek = stream->peek();

				//Require some of these be keywords
				switch (peek.TOKEN().type) {
				case TokenType::Boolean:
				case TokenType::Number:
				case TokenType::String:
					return !target;

				case TokenType::Object:
					return false;
					//return target && (target->SYNTAX().type == SyntaxType::Identifier || target->SYNTAX().type == SyntaxType::Scopename) && stream->IsType(TokenType::Object);

				default:
					return false;
				}
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				auto next = stream->next();

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
							stream->croak("Invalid boolean");  //std::format("invalid boolean '{}' detected.", tag));
						}
						
						return stream->CreateExpression(next, SyntaxType::Boolean);
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

									return stream->CreateExpression(next, SyntaxType::Number);
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
								
								return stream->CreateExpression(next, SyntaxType::Integer);
							}
							catch (std::invalid_argument in_arg) {
								return func();
							}
							
						} catch (std::invalid_argument in_arg) {
							//TODO:Fix Format #3
							stream->croak("invalid string detected");  //std::format("invalid string {} detected. ({})", next.GetTag(), in_arg.what()));
							break;                                     //Throw happens above s
						}
					}

				case TokenType::Object:
					{
						//Clears the object markers (that aren't even currently used).

						target->SYNTAX().type = SyntaxType::Scopename;

						auto next = stream->next();

						auto& tag = next.GetTag();

						//Clears the ":{" and "}" items.
						ClipString(tag, 2, 1);

						return stream->CreateExpression(next, SyntaxType::Object, *target);
					}

				case TokenType::String:
					{
						//Clears the quotations.
						ClipString(tag, 1, 1);
						return stream->CreateExpression(next, SyntaxType::String);
					}

				default:
					//TODO:Fix Format #4
					stream->croak("Invalid token as literal");  //std::format("invalid token '{}' detected as a literal.", tag));
					break;
				}

				//Technically should be unreachable.
				throw nullptr;
			}

			//Atomic due to object literal stream
			bool IsAtomic() const override { return true; }
		};

	
		
		struct CallParser : public AutoParser<CallParser>
		{
			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag) const override
			{
				//VarUsage basically means no type allowed.
				return target && target->SYNTAX().type == SyntaxType::Identifier && stream->IsType(TokenType::Punctuation, "("); // && stream->contextChain->HasKeyword("code_block");
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				target->SYNTAX().type = SyntaxType::Call;

				//I believe this makes header the contained object, rather than creating an expresion. Also changed to use the version that takes functions with just 1 member.
				//target->EmplaceChildren(Record{ "args", ExpressionType::Header, stream->Delimited("(", ")", ",", [=](auto, auto) { return stream->ParseExpression(); }) });
				auto args = ParsingStream::CreateExpression(parse_strings::args, SyntaxType::None, stream->Delimited("(", ")", ",", &ParsingStream::ParseExpression));

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

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag) const override
			{
				return !target && stream->IsType(TokenType::Keyword, "return");
			}

			Record HandleToken(ParsingStream* stream, Record*) override
			{
				//If we are not currently in a function parsing issue.
				Record ret = stream->CreateExpression(stream->next(), SyntaxType::Return);

				//whatever is cycling this will get the ; statement, but we're getting the expression inside.
				if (stream->IsType(TokenType::Punctuation, ";") == false)
					//ret.EmplaceChildren(ParseModule::UseModule<BinaryParser>(stream, target));
					ret.EmplaceChildren(stream->ParseExpression());

				return ret;
			}
		};

		
	
		struct UnaryParser : public AutoParser<UnaryParser>
		{
			//This shit complicated, I'm saving this shit for last.


			//TODO:Operators need to split, as it's pretty hard without.
			// precedence can handle this I think, instead of just raw numbers what I can do is use
			//

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag) const override
			{
				//I can saw with a fair bit of certainty that this is not going to work well when encountering errors.
				// The unary CanHandle needs to be far more discriminating.

				
				//unary is a bit tricker, some things actually have shared symbols (like the minus) so I'll have to be specific
				if (stream->IsType(TokenType::Operator) == false)
					return false;

				if (GetPrecedence(stream->peek().GetView(), OperatorType::Unary) == 0) {
					return false;
				}

				//If previous cannot have been an operator.

				if (auto prev = stream->prev(); prev.TOKEN().type == TokenType::Operator && prev.GetTag() == ".")
					return false;
				
				return !target;
			}

			bool ContextAllowed(ParseModule* context, ModuleChain* chain) override
			{
				ParsingStream* stream = chain->process;

				//If last isn't .

				//if (NULL_OP(stream)->prev().GetTag() == ".")
				//	return false;
				//RGL_LOG(info, "TEST {} {}", BinaryParser::GetSingleton()->GetContext() != context->GetContext(), NULL_OP(stream)->);
				//And if the stream isn't binary OR is a member access. (This is done to get all member accesses out of the way before it shoves it in it's body.
				
				//This is the problem quite simply it's used an outdated system.
				return stream->GetBuiltModule<BinaryParser>() != context || stream->IsType(TokenType::Operator, ".");
			}

			//Precedence mostly follows the thing that's closest to the body, with subscript being the highest of them,
			// and '.' being before all other types. Let's see how that goes down.
			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				//Currently there are 2 types. Left unary, right unary. They can be seperated by them having a target or not.
				// right ones will always have targets, left will never.

				//So in the case of unary on the left, it seeks to do the thing on the left first, then compile itself.
				//In the case of right, it will simply encapsulate the target within itself

				Record unary;

				//Note, it cannot process a dual right after, so it needs to confirm the next thing isn't a dual operator

				if (!target) {
					RecordData op = stream->next();

					//Not a strong name to make this a header, because it's handled pretty straight forward.
					// Also, more likely this seeks to parse atomic
					//Record body = stream->ParseExpression();
					Record body = stream->ParseAtomic();

					unary = stream->CreateExpression(op, SyntaxType::Unary, body);
				} 
				else {
					stream->croak("Fuck you");
				}

				return unary;
			}

			bool IsAtomic() const override { return true; }
		};

		

		struct TypeParser : public AutoParser<TypeParser>
		{
			//I may use this at some other point though.

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag) const override
			{
				//Doesn't allow a previous target yet, but later attributes will be of consideration.
				if (target)
					return false;

				auto peek = stream->peek();

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

				
				return !target && (stream->IsType(TokenType::Keyword, "struct") || stream->IsType(TokenType::Keyword, "class"));
			}

			static Record HandleInterfaceIndex(ParsingStream* stream)
			{
				Record index = ParsingStream::CreateExpression(stream->ConsumeType(TokenType::Identifier), SyntaxType::None);

				if (stream->SkipIfType(TokenType::Punctuation, "::") == true) {
					if (stream->IsType(TokenType::Punctuation, "{") == true)
					{
						auto& parent = index.EmplaceChild(ParsingStream::CreateExpression("args", SyntaxType::None));
					
						//if it wants to use brackets to define it easier basically.
						auto children = stream->Delimited("{", "}", ",", [](ParsingStream* stream) { return ParsingStream::CreateExpression(stream->next(), SyntaxType::None); });
						parent.EmplaceChildren(std::move(children));
					}
					else {
						index.EmplaceChild(ParsingStream::CreateExpression(stream->ConsumeType(TokenType::Number), SyntaxType::Number));
					}
				} else {
					index.EmplaceChild(ParsingStream::CreateExpression("0", SyntaxType::Number));
				}

				return index;
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				RecordData data_type = stream->next();

				Record result = ParsingStream::CreateExpression(stream->ConsumeType(TokenType::Identifier), SyntaxType::Type);


				Record& settings = result.EmplaceChild(ParsingStream::CreateExpression(parse_strings::settings, SyntaxType::None));

				{
					//First setting is struct or class
				
					//ugly but it does what I want. Clean up later please.
					settings.EmplaceChild(ParsingStream::CreateExpression(parse_strings::data_type, SyntaxType::None, { ParsingStream::CreateExpression(data_type, SyntaxType::None) }));
					//settings.PushBackChild(ParsingStream::CreateExpression(data_type, SyntaxType::Header));
				}

				if constexpr (1!=1)//when pigs fly, or in other words when I implement generics
				{
					//This is the section for generics at a later point. As it is not generic, it is empty.
					settings.EmplaceChild(ParsingStream::CreateExpression(parse_strings::generic, SyntaxType::None));
				}




				
				//second setting is interface, bind, or regular. Optional

				bool requires_body = true;

				if (RecordData peek = stream->peek(); peek.TOKEN().type == TokenType::Keyword) {
					
					
					//Ugly as shit I know.
					Record& attach = settings.EmplaceChild(ParsingStream::CreateExpression(parse_strings::attach, SyntaxType::None)).
						EmplaceChild(ParsingStream::CreateExpression(stream->next(), SyntaxType::None));

					switch (Hash(peek.GetTag())) {
					case "bind"_h:  //Bind needs to push back a type name
						attach.EmplaceChild(ParseModule::UseModule<IdentifierParser>(stream, nullptr)).SYNTAX().type = SyntaxType::Scopename;
						break;

					case "intrinsic"_h:  //Intrinsic needs to push back a category name, and index.
					case "external"_h:   //external needs to push back category name and index.
						requires_body = false;
						attach.EmplaceChild(HandleInterfaceIndex(stream));
						break;

					default:
						stream->croak("Unexpected keyword used after class/struct type name.");
						break;
					}
				}

				//This is actually completely optional
				if (stream->SkipIfType(TokenType::Punctuation, ":") == true) {
					Record& derives = settings.EmplaceChild(ParsingStream::CreateExpression(parse_strings::derives, SyntaxType::None));
					
					size_t after = 0;

					while (!stream->IsType(TokenType::Punctuation, "{") && !stream->IsType(TokenType::Punctuation, ";"))
					{
						//Should only check after the first loop.
						if (after++)
						{
							stream->SkipType(TokenType::Punctuation, ",");
						}

						Record decl_spec;
						Record* specifiers = nullptr;

						if (stream->IsType(TokenType::Keyword) == true) 
						{
							specifiers = &decl_spec;

							decl_spec = ParsingStream::CreateExpression(parse_strings::declare_specifier, SyntaxType::Header);

							do 
							{
								auto next = stream->next();

								switch (Hash(next.GetTag())) {
								case "public"_h:
								case "private"_h:
								case "protected"_h:
								case "internal"_h:
									decl_spec.EmplaceChild(ParsingStream::CreateExpression(next, SyntaxType::None));
									break;

								default:
									report::parse::error("Unknown keyword, expected access modifier.");
									break;
								}

								

							} while (stream->IsType(TokenType::Keyword) == true);
						}

						Record& inherit = derives.EmplaceChild(ParseModule::UseModule<IdentifierParser>(stream, nullptr));

						if (specifiers)
							inherit.EmplaceChild(std::move(*specifiers));

						
					}


				}

				//If no body, this shit requires an end.
				bool has_body = false;

				if (stream->SkipIfType(TokenType::Punctuation, "{") == true) {
					has_body = true;
					
					Record body = ParsingStream::CreateExpression(parse_strings::body, SyntaxType::None);

					auto end_of_this = [&]() -> bool {
						return stream->SkipIfType(TokenType::Punctuation, "}");
					};

					while (end_of_this() == false) {
						body.EmplaceChild(stream->ParseExpression());
					}

					result.EmplaceChild(body);
				} 
				else if (requires_body) {
					stream->croak("type requires a body.");
				}

				if (has_body)
					return result;
				else
					return stream->EndExpression(result);

			}
		};



		//*
		//Remove
		struct FormatParser : public AutoParser<FormatParser>
		{
			//The idea of this should be the lowest priority possible. It expects an identifier and if nothing else claims it then it's a field.



			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				//Must have a target, the target must be an identifier, and mustn't have a path.
				if (!target)
					return false;

				return stream->IsType(TokenType::Format);
				//The rules are slightly more lax because Format really really always needs to happen here, bar no other.
				// so incorrect use is grounds for stream failure rather than anything else.


				return target && target->SYNTAX().type == SyntaxType::Identifier && stream->IsType(TokenType::Format);
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				if (target->SYNTAX().type != SyntaxType::Identifier) {
					stream->croak("expected identifier");
				}

				if (target->FindChild(parse_strings::path) != nullptr) {
					stream->croak("expected identifier without path");
				}

				


				RecordData format = stream->next();


				

				auto& tag = format.GetTag();
				
				if (tag.ends_with(parse_strings::format_end) == false) {
					stream->croak("format token must end with 'end_format'");
				}


				auto size = tag.size();
				
				auto offset = std::strlen(parse_strings::format_start);

				size -= offset;
				size -= std::strlen(parse_strings::format_end);

				tag = tag.substr(offset, size);


				
				
				target->SYNTAX().type = SyntaxType::None;
				
				return stream->CreateExpression(format, SyntaxType::Format, std::move(*target));
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

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{

				return target && target->SYNTAX().type == SyntaxType::Identifier;
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
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

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				return !target && stream->IsType(TokenType::Keyword, "if");
			}



			void HandleBlock(Record& block, ParsingStream* stream)
			{


				if (auto is_if = stream->IsType(TokenType::Keyword, "if"); is_if || stream->SkipIfType(TokenType::Punctuation, ":") == true)
				{
					//This really shouldn't allow encasulate stream to go in. But for now this will handle.
					Record body = stream->ParseExpression();
					

					if (body.SYNTAX().type == SyntaxType::StateBlock)
						block.EmplaceChildren(std::move(body.children()));
					else if (body)
						block.EmplaceChildren(std::move(body));

					if (!is_if)
						ParseModule::UseModule<EndParser>(stream, nullptr);
				}
				else if (stream->IsType(TokenType::Punctuation, "{") == true)
				{
					Record body = ParseModule::UseModule<EncapsulateParser>(stream, nullptr);

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
					stream->croak("invalid token found proceeding if statement.");
				}

			}



			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				//if:conditional
				//<express>
				//expression here
				//<state>
				//statement here
				//<else>
				//statement here

				Record result = ParsingStream::CreateExpression(stream->next(), SyntaxType::If);
				

				

				stream->SkipType(TokenType::Punctuation, "(");

				Record& express_block = result.EmplaceChild(ParsingStream::CreateExpression(parse_strings::expression_block, SyntaxType::None));

				express_block.EmplaceChild(stream->ParseExpression());

				stream->SkipType(TokenType::Punctuation, ")");

				Record& statement_block = result.EmplaceChild(ParsingStream::CreateExpression(parse_strings::statement_block, SyntaxType::None));

			
				HandleBlock(statement_block, stream);

				if (stream->SkipIfType(TokenType::Keyword, "else") == true)
				{
					Record& else_block = result.EmplaceChild(ParsingStream::CreateExpression(parse_strings::alternate_block, SyntaxType::None));
					HandleBlock(else_block, stream);
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

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				return target && stream->IsType(TokenType::Keyword, "as");
			}





			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				auto next = stream->next();
				
				if (stream->peek().GetView() == "maybe")
				{
					next = stream->next();
				}

				Record cast = ParsingStream::CreateExpression(next, SyntaxType::Cast);

				cast.EmplaceChild(ParsingStream::CreateExpression(parse_strings::lhs, SyntaxType::None, { std::move(*target) }));
				cast.EmplaceChild(ParsingStream::CreateExpression(parse_strings::rhs, SyntaxType::None, { ParseModule::UseModule<HeaderParser>(stream, nullptr) }));

				return cast;
			}

			bool IsAtomic() const override { return true; }


		};



		struct GenericParser : public AutoParser<GenericParser>
		{
			//The idea of this should be the lowest priority possible. It expects an identifier and if nothing else claims it then it's a field.


			//uint32_t GetPriority() const override
			//{
			//	return ModulePriority::None;
			//}

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override
			{
				return !target && stream->IsType(TokenType::Keyword, "generic");
			}





			Record HandleToken(ParsingStream* stream, Record*) override
			{
				auto generic = ParsingStream::CreateExpression(stream->next(), SyntaxType::Header);

				generic.GetTag() = parse_strings::generic;


				auto hits = stream->Delimited("<", ">", ",", [](ParsingStream* stream) { return ParsingStream::CreateExpression(stream->next(), SyntaxType::Identifier); });

				generic.EmplaceChildren(std::move(hits));

				auto target = stream->ParseExpression();

				//check if it can use generic or not
				switch (target.SYNTAX().type)
				{
				case SyntaxType::Function:
				case SyntaxType::Variable:
				case SyntaxType::Type:
					break;

				default:
					report::parse::info("cannot make expression generic");
				}

				target.EmplaceChild(std::move(generic));

				return target;
			}

			bool IsAtomic() const override { return true; }


		};






		////////////////////////////////////////////
		//Preprocessors                          ///
		////////////////////////////////////////////







		struct PreprocessorParser : public ParseModule
		{
			std::optional<bool> GetKeywordState(const std::string_view& type) override
			{
				switch (Hash(type)) {
				case "preprocessor"_h:
					return true;
				}

				return false;
			}

			bool CanHandle(ParsingStream*, Record*, ParseFlag) const override { return false; }




			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				
				Record prep = ParsingStream::CreateExpression(parse_strings::preprocessor, SyntaxType::Directive);

				while (stream->eof() == false) {
					auto next = stream->next();

					if (next.GetView() != "#") {
						continue;
					}
					
					auto result = stream->ParseExpression();

					stream->SkipType(TokenType::Whitespace, "\n");

					if (result)
						prep.EmplaceChild(result);
					else
						report::parse::debug("disarding empty record");
				}

				return prep;
			}

			virtual Tokenizer* GetTokenizer() const
			{
				return std::addressof(prepTokens);
			}

			std::string_view GetCategory() const
			{
				return "preprocessor";
			}

			bool ContextAllowed(ParseModule* context, ModuleChain* link) override
			{
				return context->HasKeyword("preprocessor");
			}
		};



		struct PreprocessorModule : public ParseModule
		{
			std::optional<bool> GetKeywordState(const std::string_view& type) override
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

			virtual bool CanProcess(ParsingStream* stream, Record*, ParseFlag) const = 0;

			bool CanHandle(ParsingStream* stream, Record* target, ParseFlag flag) const override final
			{
				return stream->chain()->HasKeyword("preprocessor") && CanProcess(stream, target, flag);
			}

		};

		template <typename T>
		struct AutoProcessor : public AutoParser<T, "preprocessor", PreprocessorModule> 		
		{

			Tokenizer* GetTokenizer() const override
			{
				return std::addressof(prepTokens);
			}
		
		};

		/*The Old version.
		struct RequiresParser : public AutoProcessor<RequiresParser>
		{
			bool CanProcess(ParsingStream* stream, Record* target, ParseFlag) const override 
			{ 
				
				return !target && stream->IsType(TokenType::Identifier, "requires");
			}


			enum Mode
			{
				kScript,		//A script that's loaded in this directory
				kProject,	//A project exists
				kOption,		//Script was loaded with a particular compiler option.
				kDirectory,	//A file or folder loaded at this relative directory. Has to be loaded, meaning the sub directory has to be involved somehow.
				kMacro
			};


			Record MakeRequirement(ParsingStream* stream, Mode mode)
			{
				std::string_view name;
				
				Record out;
				parse_strings;
				switch (mode)
				{
					case kScript:
					{
						name = parse_strings::script_req;

						auto next = stream->next();

						if (next.TOKEN().type == TokenType::String)
							ClipString(next.GetTag(), 1, 1);

						out = ParsingStream::CreateExpression(next, SyntaxType::None);



						break;
					}
					case kProject:
					{
						name = parse_strings::project_req;
						
						stream->next();
						out = ParsingStream::CreateExpression(stream->next(), SyntaxType::None);
						stream->next();
						break;
					}
					case kOption:
					{
						name = parse_strings::option_req;
						
						stream->next();

						out = ParsingStream::CreateExpression(stream->next(), SyntaxType::None);
						break;
					}
					case kDirectory:
					{
						name = parse_strings::directory_req;
						stream->next();

						out = ParsingStream::CreateExpression(stream->ConsumeType(TokenType::Identifier), SyntaxType::None);
						
						Record* to = &out;

						while (stream->SkipIfType(TokenType::Operator, "/") == true)
						{
							to = &to->EmplaceChild(ParsingStream::CreateExpression(stream->ConsumeType(TokenType::Identifier), SyntaxType::None));
						}


						break;
					}
				}
				return ParsingStream::CreateExpression(name, SyntaxType::Requirement, { out });
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				auto loc = stream->next();
				
				Mode mode;

				if (stream->IsType(TokenType::String) == true)
				{
					mode = kScript;
				}
				else if (stream->IsType(TokenType::Punctuation, "...") == true)
					mode = kDirectory;
				else if (stream->IsType(TokenType::Punctuation, "/:") == true)
					mode = kOption;
				else if (stream->IsType(TokenType::Operator, "<") == true)
					mode = kProject;
				else {
					stream->croak("unknown requirement");
					mode = kScript;
				}

				auto result = MakeRequirement(stream, mode);

				result.SYNTAX().line = loc.TOKEN().line;
				result.SYNTAX().column = loc.TOKEN().column;
				return result;
			}

		};
		//*/


		struct RelationParser : public AutoProcessor<RelationParser>
		{
			bool CanProcess(ParsingStream* stream, Record* target, ParseFlag) const override
			{

				return !target && ( stream->IsType(TokenType::Identifier, "import") || stream->IsType(TokenType::Identifier, "include") );
			}


			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				auto relation = ParsingStream::CreateExpression(stream->next(), SyntaxType::Relationship);

				auto next = stream->ConsumeType(TokenType::Identifier);


				relation.EmplaceChild(ParsingStream::CreateExpression(next, SyntaxType::None));

				return relation;
			}

		};

		struct RequiresParser : public AutoProcessor<RequiresParser>
		{
			bool CanProcess(ParsingStream* stream, Record* target, ParseFlag) const override
			{

				return !target && stream->IsType(TokenType::Identifier, "requires");
			}

			Record HandleToken(ParsingStream* stream, Record* target) override
			{

				auto result = ParsingStream::CreateExpression(stream->next(), SyntaxType::Requirement);


				auto& child = result.EmplaceChild(ParsingStream::CreateExpression(stream->ConsumeType(TokenType::Identifier, "option"), SyntaxType::Prefunc));

				stream->SkipType(TokenType::Punctuation, "(");

				child.EmplaceChild(ParsingStream::CreateExpression(stream->next(), SyntaxType::Identifier));

				stream->SkipType(TokenType::Punctuation, ")");



				return result;
			}

		};

		struct PreIfParser : public AutoProcessor<PreIfParser>
		{
			bool CanProcess(ParsingStream* stream, Record* target, ParseFlag) const override
			{
				return !target && (stream->IsType(TokenType::Identifier, "if") || stream->IsType(TokenType::Identifier, "endif"));
			}

			Record HandleConditional(ParsingStream* stream)
			{
				auto result = ParsingStream::CreateExpression(stream->next(), SyntaxType::Conditional);

				//This should allow for the /: punctuator instead.
				;




				auto& child = result.EmplaceChild(ParsingStream::CreateExpression(stream->ConsumeType(TokenType::Identifier, "option"), SyntaxType::Prefunc));
				
				stream->SkipType(TokenType::Punctuation, "(");

				child.EmplaceChild(ParsingStream::CreateExpression(stream->next(), SyntaxType::Identifier));

				stream->SkipType(TokenType::Punctuation, ")");



				return result;
			}

			Record HandleEnd(ParsingStream* stream)
			{
				auto result = ParsingStream::CreateExpression(stream->next(), SyntaxType::Conditional);

		
				return result;
			}


			Record HandleToken(ParsingStream* stream, Record* target) override
			{
				auto peek = stream->peek();

				if (peek.GetView() == "if")
				{
					return HandleConditional(stream);
				}
				else if (peek.GetView() == "endif")
				{
					return HandleEnd(stream);
				}

				stream->croak("shouldn't happen");
				return{};
			}

		};
}