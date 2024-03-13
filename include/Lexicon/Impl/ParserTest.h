#pragma once

#include "Lexicon/Impl/Parser.h"

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



	inline int testers = 0;
	
	
	template <class T, bool Register = true>
	struct ParseSingleton : public ClassSingleton<T, ParseModule>, public ParseModule
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
				testers++;
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
	



	
	//*

	inline void ClipString(std::string& str, size_t left, size_t right)//needs to be able to clip the other side.
	{
		if (!left && !right)
			return;

		size_t shift = str.size() - left - right;

		str = str.substr(left, shift);
	}




	struct EndParser : public ParseSingleton<EndParser>
	{

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{

			return !target && parser->IsType(TokenType::Punctuation, ";");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			//do
			//{
				//parser->SkipType(TokenType::Punctuation, ";");
			//} 
			while (parser->SkipIfType(TokenType::Punctuation, ";") == true);
			
			return {};
		}
	};

	struct EncapsulateParser : public ParseSingleton<EncapsulateParser, false>
	{

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			if (target || parser->contextChain->HasKeyword("code_block") == false)
				return false;
			
			if (!atomic && parser->IsType(TokenType::Punctuation, "{") == true)
				return true;

			return parser->IsType(TokenType::Punctuation, "(");
		}


		Record HandleToken(Parser* parser, Record* target) override
		{
			Record result;
			
			if (parser->SkipIfType(TokenType::Punctuation, "(") == true)
			{
				result = parser->ParseExpression();
				parser->SkipType(TokenType::Punctuation, ")");
			}
			else
			{
				Record scope{};

				//std::vector<Record> children = parser->Delimited("{", "}", ";", parse_func);
				std::vector<Record> children = parser->Delimited("{", "}", ";", &Parser::ParseExpression);
				if (children.size() == 1) result = children[0];

				result.EmplaceChildren(children);
			}
			

			return result;
		}


		bool IsAtomic() override { return true; }

	};

	inline Record& GetBottomRecord_(Record& rec)
	{
		Record* focus = &rec;

		while (focus->size() != 0)
		{
			focus = &focus->GetFront();
		}

		return *focus;
	}

	inline bool IsDeclarationRecord(Record& rec)
	{
		switch (rec.SYNTAX().type)
		{
		case SyntaxType::Variable:
		case SyntaxType::Declare:
			return true;

		default:
			return false;
		}
	}

	struct IdentifierParser : public ParseSingleton<IdentifierParser>
	{
		bool IsViableKeyword(RecordData data)
		{
			if (data.TOKEN().type != TokenType::Keyword)
				return false;

			switch (Hash(data.GetTag()))
			{
				"this"_h;
			}
		}
		//Don't do this here.


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			if (parser->IsType(TokenType::Punctuation, "::") == true) {
				return !target;
			}

			if (parser->IsType(TokenType::Identifier) == true) {
				return !target;//!target || (target->SYNTAX().type == SyntaxType::Header && target->GetTag() == "<header>");
			}

			return false;
			//return !target && parser->IsType(TokenType::Punctuation, "::") || parser->IsType(TokenType::Identifier);
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			bool expl_extern = parser->SkipIfType(TokenType::Punctuation, "::");

			Record id = Parser::CreateExpression(parser->next(), SyntaxType::Field);;

			while (parser->SkipIfType(TokenType::Punctuation, "::") == true)
			{
				//parser->next();

				id.SYNTAX().type = SyntaxType::Scopename;

				id = Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier), SyntaxType::Field, id);
			}

			return id;
		}
	};


	struct VariableParser : public ParseSingleton<VariableParser>
	{
		//Variable doesn't really seem to work name wise, as this thing works multiple jobs.


		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			
			if (target && target->SYNTAX().type == SyntaxType::Declare) {
				return  parser->IsType(TokenType::Punctuation, ";") || parser->IsType(TokenType::Operator, "=");
			}

			return false;
		}

		Record HandleToken(Parser* parser, Record* target) override
		{

			target->SYNTAX().type = SyntaxType::Variable;
			
			if (parser->SkipIfType(TokenType::Operator, "=") == true)
			{
				//This should remove assignment from being a valid statement, and also allow assignment to happen 
				// with the same expression.
				target->EmplaceChild(Record{ "def", SyntaxType::Header, parser->ParseExpression() });
			}

			
			return parser->EndExpression(*target);
		}
	};


	enum KeywordType
	{
		None,
		DeclSpec,
		TypeQual,
		TypeSpec,
	};


	struct HeaderParser : public ParseSingleton<HeaderParser>
	{
		
		bool IsTypeName(std::string& name)
		{
			switch (Hash(name))
			{
			case "bool"_h:
			case "string"_h:
			case "int"_h:
			case "float"_h:
			case "double"_h:
			case "void"_h:
				return true;
			}

			return false;
		}

		KeywordType IsViableKeyword(std::string& name, bool post = false) const
		{
			switch (Hash(name))
			{
			case "readonly"_h:
			case "static"_h:
			decl_spec:
				return KeywordType::DeclSpec;

			case "signed"_h:
			case "unsigned"_h:
			case "short"_h:
			case "long"_h:
			case "bool"_h:
			case "string"_h:
			case "int"_h:
			case "float"_h:
			case "double"_h:
			case "void"_h:
				return KeywordType::TypeSpec;


			case "const"_h:
			case "mutable"_h:
				if (post)
					goto decl_spec;

				__fallthrough;
			case "ref"_h:

				return KeywordType::TypeQual;
			}

			return KeywordType::None;
		}

		bool CanContinue(RecordData peek, bool allow_identifier = true) const
		{
			if (peek.TOKEN().type == TokenType::Keyword && IsViableKeyword(peek.GetTag()) != None)
				return true;

			return allow_identifier && peek.TOKEN().type == TokenType::Identifier;
		}

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			RecordData peek = parser->peek();

			//We confirm that we can actually operate first
			//if (peek.TOKEN().type != TokenType::Keyword || IsViableKeyword(peek.GetTag()) != None) {
			if (CanContinue(peek) == false) {
				return false;
			}


			if (target) {
				//If this has a target, ensure the target is either an identifier, or a function.
				if (target->SYNTAX().type != SyntaxType::Scopename && target->SYNTAX().type != SyntaxType::Field) {// && target->SYNTAX().type != SyntaxType::Function){
					return false;
				}
			}

			return true;
		}


		Record HandleToken(Parser* parser, Record* target) override
		{
			Record target_name;

			if (!target && parser->IsType(TokenType::Identifier) == true) {
				//This ordinarily shouldn't happen, but in the event that it's forcibly used, this provides a short hand to just fire the header.

				target_name = ParseModule::ExecuteModule<IdentifierParser>(parser, nullptr);
				target = &target_name;
			}



			//This should be manually used after a function NOT, searched for.




			//This function will seek to consume until there are no more keywords. It can consume an identifier, but only once.

			Record header = Parser::CreateExpression("<header>", SyntaxType::Header,
				{
					Parser::CreateExpression("type_qual", SyntaxType::Header),
					Parser::CreateExpression("decl_spec", SyntaxType::Header),
					Parser::CreateExpression("type_spec", SyntaxType::Header),
				});

			//I'd like to make references to or pointers to each of these, will help when dealing with functions.

			bool allow_iden = true;
			bool post = false;
			if (target)
			{

				switch (target->SYNTAX().type)
				{
				case SyntaxType::Scopename:
				case SyntaxType::Field:
					header.FindChild("type_spec")->EmplaceChild(*target);
					break;

				case SyntaxType::Function:
					parser->croak("I don't fucking care nothing in post yet I don't want to think.");
					post = true;
					break;
				}

				allow_iden = false;

			}



			//for (RecordData next = parser->next(); !parser->eof(); next = parser->next())
			//while ( CanContinue(next, allow_iden))
			do
			{
				bool next = true;

				RecordData peek = parser->peek();
				logger::debug("~{}", peek.GetTag());

				//next = parser->next();
				logger::debug(">{}", peek.GetTag());
				logger::debug(">???{}", parser->peek().GetTag());
				//continue;

				auto type = peek.TOKEN().type;

				if (type == TokenType::Keyword)
				{
					switch (IsViableKeyword(peek.GetTag(), post))
					{
					case KeywordType::DeclSpec:
						header.FindChild("decl_spec")->EmplaceChild(Parser::CreateExpression(peek, post ? SyntaxType::Header : SyntaxType::Declare));
						break;

					case KeywordType::TypeQual:
						header.FindChild("type_qual")->EmplaceChild(Parser::CreateExpression(peek, SyntaxType::Header));
						break;
					case KeywordType::TypeSpec:
						header.FindChild("type_spec")->EmplaceChild(Parser::CreateExpression(peek, SyntaxType::Header));
						if (IsTypeName(peek.GetTag()) == true)
							allow_iden = false;
						break;

					default:
						parser->croak("Not a proper keyword");
					}
				}
				else if (type == TokenType::Identifier)
				{
					//IdenParser cant handle this yet
					//header.FindChild("type_spec")->EmplaceChild(ParseModule::ExecuteModule<IdentifierParser>(parser, nullptr));

					Record identifier = ParseModule::ExecuteModule<IdentifierParser>(parser, nullptr);


					if (allow_iden)
					{
						identifier.SYNTAX().type = SyntaxType::Scopename;
						header.FindChild("type_spec")->EmplaceChild(identifier);
						
						logger::critical("__a {}", identifier.GetTag());

						//If next is an identifier, this identifier is a type.
						// if the next is not, then this is a type specifier and we should keep moving.
						if (parser->IsType(TokenType::Identifier) == true) {
							//If the peeked type is an identifier, the previous identifier is a type, and the qualifiers should be placed in the new one, and returned.


							//Later, identifier should handle this.
							//Just never fucking do this here. It physically cannot work.
							//header.FindChild("type_spec")->EmplaceChild(identifier);
							identifier = ParseModule::ExecuteModule<IdentifierParser>(parser, nullptr);
							identifier.EmplaceChild(header);
							return identifier;
						}
						else {
							//If the next is a keyword, this is assumed to be a type name and will be placed in type specifiers
							//header.FindChild("type_spec")->EmplaceChild(identifier);
						}
					}
					
					else//if (!allow_iden)
					{
						logger::critical("__b");

						//If we already have an identifier turned type, then what's been given is a name.
						identifier.EmplaceChild(header);
						//TODO: You'll want to remove it from being a field so it's not confused for call.
						identifier.SYNTAX().type = SyntaxType::Declare;
						return identifier;
					}

					next = false;//don't actually need this do we?
					allow_iden = false;
				}
				else
					parser->croak("Not keyword or identifier");

				if (next)
					logger::debug(">!!!{}", parser->next().GetTag());

			} while (CanContinue(parser->peek(), true));//This doesn't need to be "do-while"

			//Honestly, this should never be returned. Ever.
			logger::debug("X {}", parser->peek().GetTag());

			return header;
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct IdentifierParser;
	struct HeaderParser;
	struct VariableParser;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





	struct LiteralParser : public ParseSingleton<LiteralParser>
	{
		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			auto peek = parser->peek();

			switch (peek.TOKEN().type)
			{
			case TokenType::Boolean:
			case TokenType::Number:
			case TokenType::String:
				return !target;

			case TokenType::Object:
				return target && (target->SYNTAX().type == SyntaxType::Field || target->SYNTAX().type == SyntaxType::Scopename) && parser->IsType(TokenType::Object);

			default:
				return false;
			}
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			auto next = parser->next();

			auto& tag = next.GetTag();

			switch (next.TOKEN().type)
			{
			case TokenType::Boolean:
			{
				if (tag == "true" || tag == "false") {
					return parser->CreateExpression(next, SyntaxType::Boolean);
				}
				else {
					//TODO:Fix Format #5
					parser->croak("Invalid boolean");//std::format("invalid boolean '{}' detected.", tag));
				}

				break;
			}

			case TokenType::Number: {
				try
				{
					//Should crash if the value isn't valid.
					auto test = std::stof(tag);

					return parser->CreateExpression(next, SyntaxType::Number);
				}
				catch (std::invalid_argument in_arg)
				{
					//TODO:Fix Format #3
					parser->croak("invalid string detected");//std::format("invalid string {} detected. ({})", next.GetTag(), in_arg.what()));
					break;//Throw happens above s
				}
			}

			case TokenType::Object:{
				//Clears the object markers (that aren't even currently used).
				
				target->SYNTAX().type = SyntaxType::Scopename;

				auto next = parser->next();

				auto& tag = next.GetTag();

				//Clears the ":{" and "}" items.
				ClipString(tag, 2, 1);

				return parser->CreateExpression(next, SyntaxType::Object, *target);
			}

			case TokenType::String: {
				//Clears the quotations.
				ClipString(tag, 1, 1);
				return parser->CreateExpression(next, SyntaxType::Number);
			}

			default:
				//TODO:Fix Format #4
				parser->croak("Invalid token as literal");//std::format("invalid token '{}' detected as a literal.", tag));
				break;
			}

			//Technically should be unreachable.
			throw nullptr;
		}

		//Atomic due to object literal parser
		bool IsAtomic() override { return true; }
	};



	struct TypeParser : public ParseSingleton<TypeParser>
	{
		//I may use this at some other point though.

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//Doesn't allow a previous target yet, but later attributes will be of consideration.
			return !target && (parser->IsType(TokenType::Keyword, "struct") || parser->IsType(TokenType::Keyword, "class"));
		}

		static Record HandleInterfaceIndex(Parser* parser)
		{
			Record index = Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier), SyntaxType::Header);

			if (parser->SkipIfType(TokenType::Punctuation, "::") == true) {
				index.PushBackChild(Parser::CreateExpression(parser->ConsumeType(TokenType::Number), SyntaxType::Number));
			}
			else {
				index.PushBackChild(Parser::CreateExpression("0", SyntaxType::Number));
			}

			return index;
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			RecordData data_type = parser->next();


			Record type = Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier), SyntaxType::Type);

			Record settings = Parser::CreateExpression("settings", SyntaxType::Header);

			//First setting is struct or class
			settings.PushBackChild(Parser::CreateExpression(data_type, SyntaxType::Header));

			//This is the section for generics at a later point. As it is not generic, it is empty.
			settings.PushBackChild(Parser::CreateExpression("generic", SyntaxType::Header));

			//second setting is interface, bind, or regular. Optional

			bool requires_body = true;

			if (RecordData peek = parser->peek(); peek.SYNTAX().type == TokenType::Keyword)
			{
				Record attach = Parser::CreateExpression(peek, SyntaxType::Header);

				parser->next();

				switch (Hash(peek.GetTag()))
				{
				case "bind"_h:			//Bind needs to push back a type name
					attach.EmplaceChild(ParseModule::TryModule<IdentifierParser>(parser, nullptr)).SYNTAX().type = SyntaxType::Scopename;
					break;

				case "intrinsic"_h:		//Intrinsic needs to push back a category name, and index.
				case "interface"_h:		//interface needs to push back category name and index.
					requires_body = false;
					attach.PushBackChild(HandleInterfaceIndex(parser));
					break;


				default:
					parser->croak("Unexpected keyword used after class/struct type name.");
					break;
				}

				settings.PushBackChild(attach);

			}
			else {
				settings.PushBackChild(Parser::CreateExpression("data", SyntaxType::Header));
			}



			//This is actually completely optional
			if (parser->SkipIfType(TokenType::Punctuation, ":") == true)
			{
				parser->croak("inheritence unsupported");
			}
			else {
				settings.PushBackChild(Parser::CreateExpression("<no child>", SyntaxType::Header));
			}

			type.PushBackChild(settings);


			if (parser->SkipIfType(TokenType::Punctuation, "{") == true)
			{
				Record body = Parser::CreateExpression("body", SyntaxType::Header);

				auto end_of_this = [&]() -> bool
					{
						return parser->SkipIfType(TokenType::Punctuation, "}");
					};

				while (end_of_this() == false) {
					body.EmplaceChild(parser->ParseExpression());

					//I would regardless like to end this off with ";" if possible
					//Also, this needs a function called Clear instead, where it will continuously loop until it no longer needs to skip.
					if (end_of_this() == false) parser->SkipType(TokenType::Punctuation, ";");
				}

				type.PushBackChild(body);
			}
			else if (requires_body)
			{
				parser->croak("type requires a body.");
			}

			return type;
		}
	};

	//A parser that's exclusively used whenever one seeks to parse a type name. Approach is similar to that of header.
	struct TypenameParser : public ParseSingleton<TypenameParser, false>
	{

		bool IsViableKeyword(RecordData peek) const
		{
			if (peek.TOKEN().type != TokenType::Keyword) {
				return false;
			}

			switch (Hash(peek.GetTag()))
			{
			case "signed"_h:
			case "unsigned"_h:
			case "short"_h:
			case "long"_h:
			case "bool"_h:
			case "string"_h:
			case "int"_h:
			case "float"_h:
			case "double"_h:
				return true;

			default:
				return false;
			}
		}

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//This should probably basically never fire unless used by name.
			return target && parser->IsType(TokenType::Identifier) || IsViableKeyword(parser->peek());
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			//Record result;

			if (parser->IsType(TokenType::Identifier) == true) {
				target->EmplaceChild(ParseModule::TryModule<IdentifierParser>(parser, nullptr));
			}
			else if (parser->IsType(TokenType::Keyword) == true){
				
				do
				{
					target->EmplaceChild(Parser::CreateExpression(parser->next(), SyntaxType::Header));
				}
				while(CanHandle(parser, target, true) == true);

				//result = Parser::CreateExpression(parser->next(), SyntaxType::Typename);
			}
			else {
				parser->croak("Not work. Rip.");
			}

			return {};

		}
	};

	struct FunctionParser : public ParseSingleton<FunctionParser>
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
			switch (Hash(type))
			{

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


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//Invalid if target is not present.
			// maybe enforce VarDeclare
			if (target)
			logger::critical("{} {}", target->GetTag(), (int)target->SYNTAX().type);
			return parser->IsType(TokenType::Punctuation, "(") && target && target->SYNTAX().type == SyntaxType::Declare;
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			//Need to figure out how to parse a var name right here, that's a part of the function of a function parser.
			//To do it, perhaps it can call the things it needs in order. Variable parser, scriptname parser, variable parser, variable parser,
			// assignment


			//The target needs to exist, else croak.
			if (!target) {//This bit is pretty much not possible. But still gonna keep.
				parser->croak("Expected identifier before scriptname punctuation (may be dev error)");
			}
			else if (target->SYNTAX().type != SyntaxType::Declare) {
				parser->croak("PLACEHOLDER, expected declaration.");
			}
			//The target type needs to be changed to function here.


			target->SYNTAX().type = SyntaxType::Function;

			size_t post = 0;

			auto _delegate = [&](Parser* parser, Record*) -> Record
				{
					//THIS version checks for the extension function syntax which changes what a static function extends. But it's rough, because I actually want
					// the internal syntax to exist without having to do "first_arg" all over the place.
					// I think instead what I need is another parser, one that only comes out to play when the current context is parsing Parameter.
					//The extension parser. It has to be branching, has to be a param type, and it has to have 

					if (post++ == 0 && parser->SkipIfType(TokenType::Keyword, "this") == true)
					{
						Record result{ "extend", SyntaxType::Header };
						ParseModule::TryModule<TypenameParser>(parser, &result);

						//Record result;
						//if (ParseModule::QueryModule<IdentifierParser>(parser, result, nullptr) == false)
						//{result = Parser::CreateExpression(parser->next(), SyntaxType::Typename);}

						result.SYNTAX().type = SyntaxType::Scopename;
						

						//target->EmplaceChildren(Record{ "extends", result.FindChild("type") });
						//target->EmplaceChildren(Record{ "extend", SyntaxType::Header, result });
						target->EmplaceChildren(result);


						//we send an empty record to denote the discarding of this.
						return {};
					}

					Record going = ParseModule::TryModule<HeaderParser>(parser, nullptr);

					//Same as the bit for variable. if I can merge it, I would.
					if (parser->SkipIfType(TokenType::Operator, "=") == true)
					{
						going.EmplaceChild(Parser::CreateExpression("def", SyntaxType::Header, { parser->ParseExpression() }));
					}

					return going;


				};

			//I realize all this ain't super needed. To account for extension, all I need to do is remove the first entry if it's named this, HERE,
			// and copy type, change it's name to extends, place it on the target. and Pop it out. Before that, maybe check for other thingy mabobs.
			// I also note that the parser module is VERY hands on, so I should use that to sort out which is using this or not.
			// I can just have the lambda check for each thing named this after the first entry, and then cull it.
			//target->EmplaceChildren(Record{ "params", SyntaxType::Total, parser->Delimited("(", ")", ",", ParseModule::TryModule<ParameterParser>) });
			target->EmplaceChildren(Record{ "params", SyntaxType::Total, parser->Delimited("(", ")", ",", _delegate) });

			//Do header check right here.

			//target->EmplaceChildren(Record{ "code", SyntaxType::Header, parser->Delimited("{", "}", ";", &Parser::ParseExpression) });
			target->EmplaceChildren(Record{ "code", SyntaxType::Header, parser->Delimited("{", "}", 
				[=]() { Record out; ParseModule::QueryModule<EndParser>(parser, out, nullptr); }, 
				&Parser::ParseExpression) });

			return *target;
		}

		//Context as a concept hits a snag here, because this would need 2.
	};


	struct BinaryParser : public ParseSingleton<BinaryParser>
	{
		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//RecordData token = parser->peek();

			//If atomic, the only thing that can be accepted would be if next is an access operator.
			if (atomic && parser->IsType(TokenType::Operator, ".") == false)
				return false;

			bool is_binary = true;
			//Target must not be binary either, because it's not possible to have 2 binary next to each other.
			return target && !IsDeclarationRecord(*target)  && parser->IsType(TokenType::Operator) && is_binary;
		}

		//TODO:Binary parser needs to be able to know be atomic, but also needs to know if the next operator is "." to be considered atomic.
		Record _HandleBinary(Parser* parser, Record left, int my_prec) {
			//Just making pretend right now.
			//TODO:Need to make an class to manage precedence
			static std::map <std::string, int> PRECEDENCE
			{
				{"=", 1},
				{"||", 2},
				{"&&", 3},
				{"<", 7},
				{">", 7},
				{"<=", 7},
				{">=", 7},
				{"==", 7},
				{"!=", 7},
				{"+", 10},
				{"-", 10},
				{"*", 20},
				{"/", 20},
				{"%", 20},
				{".", 25},
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

					Record a_lhs = Parser::CreateExpression("left", SyntaxType::Header, { left });

					//Record a_rhs = Parser::CreateExpression("right", ExpressionType::Header, { _HandleBinary(parser, parser->ParseExpression(), his_prec) });
					//Return to form with parse atomic.
					Record a_rhs = Parser::CreateExpression("right", SyntaxType::Header, { _HandleBinary(parser, parser->ParseAtomic(), his_prec) });


					Record pivot = parser->CreateExpression(tar, SyntaxType::Binary, a_lhs, a_rhs);

					return _HandleBinary(parser, pivot, my_prec);
				}
			}
			else if (tar.SYNTAX().type == SyntaxType::Binary)
			{
				parser->croak("Target did not have precedence despite being binary");
			}

			return left;
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			//TODO:Make BinaryParser work without a target for the equals use. (Or make that the rule of assign?)
			return _HandleBinary(parser, *target, 0);
		}
		bool ContextAllowed(ProcessContext* context, ProcessChain*)  override
		{
			//It cant be in the middle of switches was something, but not accurate
			//TODO: Needs addressing, during what statement type should one not expect binary again?
			// Note, it only cares about the last one so it can do stuff in a parenthesis statement.
			return GetContext() != context->GetContext() && !context->IsContextType("Statement");
		}
	};

	struct UnaryParser : public ParseSingleton<UnaryParser, true>
	{

		//TODO:Operators need to split, as it's pretty hard without.
		// precedence can handle this I think, instead of just raw numbers what I can do is use
		//

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//I can saw with a fair bit of certainty that this is not going to work well when encountering errors.
			// The unary CanHandle needs to be far more discriminating.

			bool is_unary = true;
			//unary is a bit tricker, some things actually have shared symbols (like the minus) so I'll have to be specific
			if (!is_unary || parser->IsType(TokenType::Operator) == false)
				return false;

			//Previous must either an operator, or a non-literal

			auto prev = parser->prev();

			if (prev && (prev.TOKEN().type != TokenType::Operator || prev.GetTag() == "."))
				return false;

			return !target;

			//The shit above is so temporary it will make your head fucking spin. There are targeting unary operators
			// but I haven't the set up for them.


			//If there either was nothing of the previous bit was an operator.
			return !target && (!prev || prev.TOKEN().type == TokenType::Operator);
		}

		bool ContextAllowed(ProcessContext* context, ProcessChain* chain)  override
		{
			Parser* parser = chain->process->As<Parser>();

			//If last isn't .


			//if (NULL_OP(parser)->prev().GetTag() == ".")
			//	return false;
			//RGL_LOG(info, "TEST {} {}", BinaryParser::GetSingleton()->GetContext() != context->GetContext(), NULL_OP(parser)->);
			//And if the parser isn't binary OR is a member access. (This is done to get all member accesses out of the way before it shoves it in it's body.
			return BinaryParser::GetSingleton()->GetContext() != context->GetContext() || (NULL_OP(parser)->IsType(TokenType::Operator, "."));
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

			if (!target)
			{
				RecordData op = parser->next();

				//Not a strong name to make this a header, because it's handled pretty straight forward.
				// Also, more likely this seeks to parse atomic
				//Record body = parser->ParseExpression();
				Record body = parser->ParseAtomic();

				unary = parser->CreateExpression(op, SyntaxType::Unary, body);
			}
			else
			{
				parser->croak("Fuck you");

			}

			return unary;
		}

		bool IsAtomic() override { return true; }
	};



	struct ParenthesisParser : public ParseSingleton<ParenthesisParser>
	{
		//This is basically used in most situations where a lose level of control wishes to be
		// applied 


		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//It will not parse nested and must be within a function statement.
			//Disabling this for now.
			return !target && parser->IsType(TokenType::Punctuation, "(") && parser->contextChain->HasKeyword("code_block");// && parser->contextChain->IsContextType("FunctionStatement");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{

			parser->SkipType(TokenType::Punctuation, "(");

			Record result = parser->ParseExpression();

			parser->SkipType(TokenType::Punctuation, ")");

			return result;
		}


		bool ContextAllowed(ProcessContext* context, ProcessChain*)  override
		{
			//parenthesis will not process statements, so no if's no switches, no functions, etc. Probably want to do something to prevent it from handling types as well.
			// But I'm kinda lazy ngl
			return !context->IsContextType("Statement");
		}

		bool IsAtomic() override { return true; }
	};


	struct ScopeParser : public ParseSingleton<ScopeParser>
	{
		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{

			//TODO:Make using context chains easier PLEASE
			//TODO:Test out directly getting FunctionParser instead.
			//If there is no nested target AND we are in a function definition AND the next type is a puncutation with '{'
			return !target && parser->contextChain->IsContextType("FunctionStatement") && parser->IsType(TokenType::Punctuation, "{");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			//auto parse_func = [](Parser* _psr, Record*) -> Record
			//{
			//	return _psr->ParseExpression();
			//};

			//TODO:Address the type that this expression should work.
			Record scope{};

			//std::vector<Record> children = parser->Delimited("{", "}", ";", parse_func);
			std::vector<Record> children = parser->Delimited("{", "}", ";", &Parser::ParseExpression);
			if (children.size() == 0) return scope;
			if (children.size() == 1) return children[0];

			scope.EmplaceChildren(children);


			return scope;
		}
	};



	struct CallParser : public ParseSingleton<CallParser>
	{

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//VarUsage basically means no type allowed.
			return target && target->SYNTAX().type == SyntaxType::Field && parser->IsType(TokenType::Punctuation, "(") && parser->contextChain->HasKeyword("code_block");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			target->SYNTAX().type = SyntaxType::Call;

			//I believe this makes header the contained object, rather than creating an expresion. Also changed to use the version that takes functions with just 1 member.
			//target->EmplaceChildren(Record{ "args", ExpressionType::Header, parser->Delimited("(", ")", ",", [=](auto, auto) { return parser->ParseExpression(); }) });
			auto args = Parser::CreateExpression("args", SyntaxType::Header, parser->Delimited("(", ")", ",", &Parser::ParseExpression));

			target->EmplaceChildren(args);

			return *target;
		}

		bool IsAtomic() override { return true; }
	};

	struct ScriptParser : public ParseSingleton<ScriptParser, false>
	{
		//singleton, but not allowed to be registered, must be used manually.

		//Not registered, no need for priority.

		//Script parser cannot launch other than manually.

		bool HasKeyword(std::string_view type) override
		{
			switch (Hash(type))
			{
			case "decl_block"_h:
				return true;
			}


			return false;
		}

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override { return false; }

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
	};


	struct ReturnParser : public ParseSingleton<ReturnParser>
	{
		//Variable doesn't really seem to work name wise, as this thing works multiple jobs.


		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			return !target && parser->IsType(TokenType::Keyword, "return");
		}

		Record HandleToken(Parser* parser, Record* target) override
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


	struct ThisParser : public ParseSingleton<ThisParser>
	{
		//Variable doesn't really seem to work name wise, as this thing works multiple jobs.


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{

			return !target && parser->IsType(TokenType::Keyword, "this");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			RecordData next = parser->next();
			next.GetTag() = "<this>";
			return Parser::CreateExpression(next, SyntaxType::Field);
		}

	};





	//Concept canned. Too distinct.
	struct KeywordParser : public ParseSingleton<KeywordParser, false>
	{
		//Variable doesn't really seem to work name wise, as this thing works multiple jobs.


		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			if (!target && parser->IsType(TokenType::Keyword) == true)
			{
				switch (Hash(parser->peek().GetTag()))
				{
				case "this"_h:
				case "return"_h:
					
					return true;
				}
			}

			return false;

			return !target && parser->IsType(TokenType::Keyword, "return");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{

			switch (Hash(parser->peek().GetTag()))
			{
				case "this"_h:
				{
					RecordData next = parser->next();
					next.GetTag() = "<this>";
					return Parser::CreateExpression(next, SyntaxType::Field);
				}
				case "return"_h:
				{
					//If we are not currently in a function parsing issue.
					Record ret = parser->CreateExpression(parser->next(), SyntaxType::Return);

					//whatever is cycling this will get the ; statement, but we're getting the expression inside.
					if (parser->IsType(TokenType::Punctuation, ";") == false)
						//ret.EmplaceChildren(ParseModule::TryModule<BinaryParser>(parser, target));
						ret.EmplaceChildren(parser->ParseExpression());

					return ret;
				}
			}

			
		}
	};


#ifdef disabling_old
	//*


	constexpr bool enabling_ = false;

	//LEX::Impl::TokenType inline static const unsigned fooday(){}






	//TODO:Merge Literal and ObjectLiteral parse modules, they were seperated off of an ill concieved concept of depth.
	// Depth is no longer implemented, and nor should object literals being completely seperate.

	//Should drop the parser part of the name, that way it's easy to identify for context. Then put them in a namespace called parsers.
	struct LiteralParser : public ParseSingleton<LiteralParser>
	{
		uint32_t GetPriority() const override
		{ 
			return ModulePriority::High; 
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			auto peek = parser->peek();
			
			bool req_tar = false;
			bool has_tar = target;

			switch (peek.TOKEN().type)
			{
			case TokenType::Boolean:
			case TokenType::Number:
			case TokenType::String:
				break;
			
			//case TokenType::Object:
			//	req_tar = true;
			//	break;

			default: 
				return false;
			}
			
			return req_tar == has_tar;
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			auto next = parser->next();
				
			auto& tag = next.GetTag();

			switch (next.TOKEN().type)
			{
			case TokenType::Boolean:
			{
				if (tag == "true" || tag == "false"){
					return parser->CreateExpression(next, SyntaxType::Boolean);
				}
				else {
					//TODO:Fix Format #5
					parser->croak("Invalid boolean");//std::format("invalid boolean '{}' detected.", tag));
				}
					
				break;
			}
					
			case TokenType::Number: {
				try
				{
					//Should crash if the value isn't valid.
					auto test  = std::stof(tag);

					return parser->CreateExpression(next, SyntaxType::Number);
				}
				catch (std::invalid_argument in_arg)
				{
					//TODO:Fix Format #3
					parser->croak("invalid string detected");//std::format("invalid string {} detected. ({})", next.GetTag(), in_arg.what()));
					break;//Throw happens above s
				}
			}

			//case TokenType::Object:{
				//Clears the object markers (that aren't even currently used).
				//ClipString(tag, 2);
				//return parser->CreateExpression(next, ExpressionType::Object);
			//}

			case TokenType::String:{
				//Clears the quotations.
				ClipString(tag, 1, 1);
				return parser->CreateExpression(next, SyntaxType::Number);
			}

			default:
				//TODO:Fix Format #4
				parser->croak("Invalid token as literal");//std::format("invalid token '{}' detected as a literal.", tag));
				break;
			}
				
			//Technically should be unreachable.
			throw nullptr;
		}

		//Atomic due to object literal parser
		bool IsAtomic() override { return true; }
	};

	//Object is shaping to function a little weird, so I'm setting the depth to be something a bit different.
	struct ObjectLiteral : public ParseSingleton<ObjectLiteral, false>
	{

		uint32_t GetPriority() const override
		{
			return ModulePriority::Max;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{//the target req is new, should it be here?
			return target && parser->IsType(TokenType::Object);
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			if (!target || (target->SYNTAX().type != SyntaxType::Field && target->SYNTAX().type != SyntaxType::Scopename)) {
				parser->croak("Placeholder: No type detected");
				//Or type name detected
			}

			target->SYNTAX().type = SyntaxType::Scopename;

			auto next = parser->next();

			auto& tag = next.GetTag();


			//Clears the ":{" and "}" items.
			ClipString(tag, 2, 1);
				
			return parser->CreateExpression(next, SyntaxType::Object, *target);
		}


		std::string_view GetContext() override
		{
			//Basically the same as this type.
			
			return TypeName<LiteralParser>::value;
		}
	};


	
	struct VariableParser : public ParseSingleton<VariableParser, enabling_>
	{
		//Variable doesn't really seem to work name wise, as this thing works multiple jobs.


		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			
			return parser->IsType(TokenType::Identifier) && (!target || target->SYNTAX().type == SyntaxType::Field);
		}

		Record HandleToken(Parser* parser, Record* target) override
		{




			//I would like to refine the conditions on this, namely I'd like it to use
			if (target) {//Unsure of, but basically if this is a nested 
				//At this point we know that what it is has to be a type (but I may want to condition that), so we edit the first child of target
				// to be typename
				
				//Not sure why I'm getting the front,this doesn't work if it has no children.
				//target->GetFront()->SYNTAX().type = ExpressionType::Typename;
				target->SYNTAX().type = SyntaxType::Scopename;

				//I'm doing this to make a place for type name, might be wrong actually.
				auto result = parser->CreateExpression(parser->next(), SyntaxType::VarDeclare, Record{ "type", SyntaxType::Total, *target });
				
				if (parser->IsType(TokenType::Operator, "=") == true)
				{
					//This should remove assignment from being a valid statement, and also allow assignment to happen 
					// with the same expression.
					parser->next();
					result.EmplaceChild(Record{ "def", SyntaxType::Header, parser->ParseExpression() });
				}

				

				//TODO: Assign parser is going to have to be a part of variable declaration, assignment can happen to the already defined

				//ATOMIC USE, manual use of Binary parser expected.
				return result;
			}
			else {
				//Technically about the use, rather the declaration
				return parser->CreateExpression(parser->next(), SyntaxType::VarUsage);
			}
		}
	};

	struct ThisParser : public ParseSingleton<ThisParser>
	{
		//Variable doesn't really seem to work name wise, as this thing works multiple jobs.


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{

			return !target && parser->IsType(TokenType::Keyword, "this");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			RecordData next = parser->next();
			next.GetTag() = "<this>";
			return Parser::CreateExpression(next, SyntaxType::Field);
		}
	};

		

	struct LineParser : public ParseSingleton<LineParser, false>
	{


		bool HasKeyword(std::string_view type) override
		{
			switch (Hash(type))
			{
			case "code_block"_h:
				return true;
			}


			return false;
		}

		//singleton, but not allowed to be registered, must be used manually.

		//Not registered, no need for priority.

		//Script parser cannot launch other than manually.
		bool CanHandle(Parser* parser, Record* target, bool atomic) const override { return false; }

		Record HandleToken(Parser* parser, Record* target) override
		{
			//might be what's used for formulas
			Record line = parser->ParseExpression();

			//script.EmplaceChild(Parser::CreateExpression(parser->project(), SyntaxType::Project));

			//if (parser->eof() == true) 
			{

				//RGL_LOG(info, "tag: {}", parser->peek().GetTag());
				//script.EmplaceChild(parser->ParseExpression());

				
			}

			return line;
		}
	};

	struct ScriptParser : public ParseSingleton<ScriptParser, false>
	{
		//singleton, but not allowed to be registered, must be used manually.

		//Not registered, no need for priority.

		//Script parser cannot launch other than manually.
		
		bool HasKeyword(std::string_view type) override
		{
			switch (Hash(type))
			{
			case "decl_block"_h:
				return true;
			}


			return false;
		}
		
		bool CanHandle(Parser* parser, Record* target, bool atomic) const override { return false; }

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
				if (parser->eof() == false) parser->SkipType(TokenType::Punctuation, ";");
			}

			return script;
		}
	};


	struct ScriptnameParser : public ParseSingleton<ScriptnameParser, enabling_>
	{
		//I would actually like to revise how this works, and it's actually called the ScopeResolutionParser. Maybe, scopenameparser is better.
		// But I would like this to be able to loop until it stops having a scope to analyze.


		//Type name is handled by detecting the puncutation ::


		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//Can be atomic or Nested.
			return parser->IsType(TokenType::Punctuation, "::");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			if (target) {
				if (target->SYNTAX().type != SyntaxType::VarUsage && target->SYNTAX().type != SyntaxType::VarDeclare)
					parser->croak("Expected identifier before scriptname punctuation (may be dev error)");

				//Basically "Scriptname::Type Sciptname::Identifier..." shouldn't happen. This isn't entirely true however. I think I might move
				// this into the condition so the chain is broken before it starts doing this again.
				if (target->FindChild("type") != nullptr)
					parser->croak("Expected identifier without type.");
			
			
				target->SYNTAX().type = SyntaxType::Scopename;
			}


			Record result;
			
			bool first = true;

			do
			{
				//Toss the "::"
				parser->next();

				if (parser->IsType(TokenType::Identifier) == false) {
					parser->croak("Expecting Identifier after scriptname punctuation");
				}

				if (first){
					result = parser->CreateExpression(parser->next(), SyntaxType::VarUsage);

					
					if (target) {
						result.EmplaceChild(*target);
					}
					else {
						//So if there is no target, that means it was like "::this". That means use a global specifically.
						// However, currently there's no way to find that out if the above is the only thing present. Perhaps it should have a tag or some sort.
					}
				}
				else{
					//This is no longer the last in a line, so it isn't a var, it's a scopename.
					result.SYNTAX().type = SyntaxType::Scopename;
					
					result = parser->CreateExpression(parser->next(), SyntaxType::VarUsage, result);
				}
				
				
				first = false;

			} while (parser->IsType(TokenType::Punctuation, "::") == true);

			return result;
			//I'll excuse this for right now.
			RecordData scriptname = target ? target->GetData() : parser->next();

			if (parser->IsType(TokenType::Identifier) == false){
				parser->croak("Expecting Identifier after scriptname punctuation");
			}

			//At this time, we don't know what comes next is actually a type name, could be a function. So for now, VarName
			//THIS IS BROKEN, I'm not sure why it uses script name twice like this, but get your shit in order.
			return parser->CreateExpression(target->GetData(), SyntaxType::Scopename, parser->CreateExpression(scriptname, SyntaxType::VarDeclare));
		}
	};


	struct ArrayDeclareParser
	{
		//This isn't set in stone but the idea should be that if the previous type looks to be a VarDeclare, as the next token it looks
		// for puncutation of "[", and immediately skips both, then adding into a section for the types called qualifiers.
		// in this section i can probably just make one string instead, but what I'll do is have one string that has a bunch of qualifiers begin and end with /
		// so it'll search for "/isarray/" when getting the type.

		//Note, these qualities


		//This might work 2 jobs, seeing as I need the subscript parser as well, however, either way, they will both be atomic.
	};


	struct BinaryParser : public ParseSingleton<BinaryParser>
	{
		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//RecordData token = parser->peek();
			
			//If atomic, the only thing that can be accepted would be if next is an access operator.
			if (atomic && parser->IsType(TokenType::Operator, ".") == false)
				return false;

			bool is_binary = true;
			//Target must not be binary either, because it's not possible to have 2 binary next to each other.
			return target && parser->IsType(TokenType::Operator) && is_binary;
		}

		//TODO:Binary parser needs to be able to know be atomic, but also needs to know if the next operator is "." to be considered atomic.
		Record _HandleBinary(Parser* parser, Record left, int my_prec) {
			//Just making pretend right now.
			//TODO:Need to make an class to manage precedence
			static std::map <std::string, int> PRECEDENCE
			{
				{"=", 1},
				{"||", 2},
				{"&&", 3},
				{"<", 7},
				{">", 7},
				{"<=", 7},
				{">=", 7},
				{"==", 7},
				{"!=", 7},
				{"+", 10},
				{"-", 10},
				{"*", 20},
				{"/", 20},
				{"%", 20},
				{".", 25},
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

					Record a_lhs = Parser::CreateExpression("left", SyntaxType::Header, { left });

					//Record a_rhs = Parser::CreateExpression("right", ExpressionType::Header, { _HandleBinary(parser, parser->ParseExpression(), his_prec) });
					//Return to form with parse atomic.
					Record a_rhs = Parser::CreateExpression("right", SyntaxType::Header, { _HandleBinary(parser, parser->ParseAtomic(), his_prec) });


					Record pivot = parser->CreateExpression(tar, SyntaxType::Binary, a_lhs, a_rhs);

					return _HandleBinary(parser, pivot, my_prec);
				}
			}
			else if (tar.SYNTAX().type == SyntaxType::Binary)
			{
				parser->croak("Target did not have precedence despite being binary");
			}

			return left;
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			//TODO:Make BinaryParser work without a target for the equals use. (Or make that the rule of assign?)
			return _HandleBinary(parser, *target, 0);
		}
		bool ContextAllowed(ProcessContext* context, ProcessChain*)  override
		{
			//It cant be in the middle of switches was something, but not accurate
			//TODO: Needs addressing, during what statement type should one not expect binary again?
			// Note, it only cares about the last one so it can do stuff in a parenthesis statement.
			return GetContext() != context->GetContext() && !context->IsContextType("Statement");
		}
	};

	struct AssignParser : public ParseSingleton<AssignParser, false>
	{

		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		//The idea of this is it's supposed to handle fresh assignments that are effectively declarations.
		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			return target && target->SYNTAX().type == SyntaxType::VarDeclare && parser->IsType(TokenType::Operator, "=");
		}
		


		Record HandleToken(Parser* parser, Record* target) override
		{
			//First, we toss the parenthesis.
			parser->next();

			//TODO: Assign parser is going to have to be a part of variable declaration, assignment can happen to the already defined

			//ATOMIC USE, manual use of Binary parser expected.
			target->EmplaceChild(Record{ "def", SyntaxType::Total, parser->ParseExpression() });

			return *target;
		}

	};


	struct ReturnParser : public ParseSingleton<ReturnParser>
	{
		//Variable doesn't really seem to work name wise, as this thing works multiple jobs.


		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			return !target && parser->IsType(TokenType::Keyword, "return");
		}

		Record HandleToken(Parser* parser, Record* target) override
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


	struct ParameterParser : public ParseSingleton<ParameterParser, false>
	{
		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//This should probably basically never fire unless used by name.
			return parser->IsType(TokenType::Identifier);
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			Record base{ "", SyntaxType::Header };

			//It will do all these in sequence, and fail should any of them, that's a parsing failure.

			if (parser->IsType(TokenType::Identifier) == true) {
				RGL_LOG(debug, "iden");
				//Why is the
				base = ParseModule::TryModule<VariableParser>(parser, nullptr);
				//base = new_base;
				//"a: {}, b: {}", new_base.PrintAs<Expression>(), base.PrintAs<Expression>());
				target = &base;
			}
			
			//NOT else if because both can exist.
			if (parser->IsType(TokenType::Punctuation, "::") == true) {
				RGL_LOG(debug, "punc");
				base = ParseModule::TryModule<ScriptnameParser>(parser, target);
				target = &base;
				//wasn't before
				//base = ParseModule::TryModule<VariableParser>(parser, target);

			}

			if (!target)
				parser->croak("PLACEHOLDER: iden nor punc checks worked.");

			//if (parser->IsType(TokenType::Punctuation, "[") == true)
			//	ParseModule::TryModule<Array>(parser, nullptr);
				

			base = ParseModule::TryModule<VariableParser>(parser, target);

			//Going into default evaluation, note, this is when Parameters rules of initialization become a fair bit restrictive.
			if (parser->IsType(TokenType::Operator, "=") == true) {
				RGL_LOG(debug, "def");
				//Record def{ "default", ExpressionType::Total, ParseModule::TryModule<BinaryParser>(parser, target) };
				//base.EmplaceChildren(def);
				base = ParseModule::TryModule<AssignParser>(parser, target);
			}

			//If everything is here, it should be a VarDeclare Expression, with a type, qualifier, and default records denoting the respective settings.
			return base;
		}
	};


	//A bunch parser, this doesn't deal with this (as its hard to detect in situations other than "iden iden"
	//[[deprecated("Handled by variable basically.")]]
	struct TypenameParser : public ParseSingleton<TypenameParser, enabling_>
	{
		//I may use this at some other point though.

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			return target && target->SYNTAX().type == SyntaxType::VarUsage && parser->IsType(TokenType::Identifier);// || parser->IsType(TokenType::Punctuation, "::");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			/*
			Record result;

			//The idea is you want to keep peeking the old

			while (true)
			{
				if (target)
				{
					//If there's a target, we'll be expecting this.
					parser->SkipType(TokenType::Punctuation, "::");
				}
				else
				{
					result = 
					target = parser->CreateExpression(target->GetData(), SyntaxType::Scopename)
				}

			}

			//*/


			if (!target || target->SYNTAX().type != SyntaxType::VarUsage) {
				parser->croak("Placeholder: No variable detected");
			}

			parser->SkipType(TokenType::Punctuation, "::");

			return parser->CreateExpression(target->GetData(), SyntaxType::Scopename);
		}
	};

	

	struct FunctionParser : public ParseSingleton<FunctionParser>
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
			switch (Hash(type))
			{
			case "code_block"_h:
				return true;
			}


			return false;
		}



		uint32_t GetPriority() const override
		{
			return ModulePriority::High - 50;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//Invalid if target is not present.
			// maybe enforce VarDeclare
			return parser->IsType(TokenType::Punctuation, "(") && target && target->SYNTAX().type == SyntaxType::VarDeclare;
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			//Need to figure out how to parse a var name right here, that's a part of the function of a function parser.
			//To do it, perhaps it can call the things it needs in order. Variable parser, scriptname parser, variable parser, variable parser,
			// assignment


			//The target needs to exist, else croak.
			if (!target) {//This bit is pretty much not possible. But still gonna keep.
				parser->croak("Expected identifier before scriptname punctuation (may be dev error)");
			}
			else if (target->SYNTAX().type != SyntaxType::VarDeclare) {
				parser->croak("PLACEHOLDER, expected type name or something like that.");
			}
			//The target type needs to be changed to function here.


			target->SYNTAX().type = SyntaxType::Function;

			size_t post = 0;

			auto _delegate = [&](Parser* parser, Record*) -> Record
			{
				//THIS version checks for the extension function syntax which changes what a static function extends. But it's rough, because I actually want
				// the internal syntax to exist without having to do "first_arg" all over the place.
				// I think instead what I need is another parser, one that only comes out to play when the current context is parsing Parameter.
				//The extension parser. It has to be branching, has to be a param type, and it has to have 

				if (post++ == 0 && parser->SkipIfType(TokenType::Keyword, "this") == true)
				{

					Record result = ParseModule::TryModule<VariableParser>(parser, nullptr);
					if (ParseModule::QueryModule<TypenameParser>(parser, result, &result) == false)
					{
						result.SYNTAX().type = SyntaxType::Scopename;
					}

					//target->EmplaceChildren(Record{ "extends", result.FindChild("type") });
					target->EmplaceChildren(Record{ "extend", SyntaxType::Header, result });
						

					//we send an empty record to denote the discarding of this.
					return {};
				}


				return ParseModule::TryModule<ParameterParser>(parser, target);


			};

			//I realize all this ain't super needed. To account for extension, all I need to do is remove the first entry if it's named this, HERE,
			// and copy type, change it's name to extends, place it on the target. and Pop it out. Before that, maybe check for other thingy mabobs.
			// I also note that the parser module is VERY hands on, so I should use that to sort out which is using this or not.
			// I can just have the lambda check for each thing named this after the first entry, and then cull it.
			//target->EmplaceChildren(Record{ "params", SyntaxType::Total, parser->Delimited("(", ")", ",", ParseModule::TryModule<ParameterParser>) });
			target->EmplaceChildren(Record{ "params", SyntaxType::Total, parser->Delimited("(", ")", ",", _delegate) });


			//target->EmplaceChildren(Record{ "params", ExpressionType::Total, parser->Delimited("(", ")", ",", _delegate) });

			//Ensures that the proper token is there.
			//parser->SkipType(TokenType::Punctuation, "{");
			//target->EmplaceChildren(Record{ "code", ExpressionType::Header, parser->ParseExpression() });
			//parser->SkipType(TokenType::Punctuation, "}");

			//changed to not use lambda
			//target->EmplaceChildren(Record{ "code", ExpressionType::Header, parser->Delimited("{", "}", ";", [=](auto, auto) { return parser->ParseExpression(); }) });
			target->EmplaceChildren(Record{ "code", SyntaxType::Header, parser->Delimited("{", "}", ";", &Parser::ParseExpression) });

			return *target;
		}

		//Context as a concept hits a snag here, because this would need 2.
	};




	struct IdentifierParser : public ParseSingleton<IdentifierParser, false>
	{
		//This may become the primary, but the current issue is that it doesn't at all deal with the concept of
		// something definitely being a variable, and will only use field. I'll deal with that prospect later.
		// For now, it's exclusively a direct parser

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			return !target && parser->IsType(TokenType::Identifier);// || parser->IsType(TokenType::Punctuation, "::");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			Record id = Parser::CreateExpression(parser->next(), SyntaxType::Field);

			while (parser->IsType(TokenType::Punctuation, "::") == true)
			{
				parser->next();

				id.SYNTAX().type = SyntaxType::Scopename;
			
				id = Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier), SyntaxType::Field, id);
			}

			return id;
		}
	};


	
	struct TypeDeclareParser : public ParseSingleton<TypeDeclareParser>
	{
		//I may use this at some other point though.

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//Doesn't allow a previous target yet, but later attributes will be of consideration.
			return !target && (parser->IsType(TokenType::Keyword, "struct") || parser->IsType(TokenType::Keyword, "class"));
		}

		static Record HandleInterfaceIndex(Parser* parser)
		{
			Record index = Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier), SyntaxType::Header);
			
			if (parser->SkipIfType(TokenType::Punctuation, "::") == true){
				index.PushBackChild(Parser::CreateExpression(parser->ConsumeType(TokenType::Number), SyntaxType::Number));
			}
			else{
				index.PushBackChild(Parser::CreateExpression("0", SyntaxType::Number));
			}

			return index;
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			RecordData data_type = parser->next();

			
			Record type = Parser::CreateExpression(parser->ConsumeType(TokenType::Identifier), SyntaxType::TypeDeclare);

			Record settings = Parser::CreateExpression("settings", SyntaxType::Header);
			
			//First setting is struct or class
			settings.PushBackChild(Parser::CreateExpression(data_type, SyntaxType::Header));
			
			//This is the section for generics at a later point. As it is not generic, it is empty.
			settings.PushBackChild(Parser::CreateExpression("generic", SyntaxType::Header));

			//second setting is interface, bind, or regular. Optional

			bool requires_body = true;

			if (RecordData peek = parser->peek(); peek.SYNTAX().type == TokenType::Keyword)
			{
				Record attach = Parser::CreateExpression(peek, SyntaxType::Header);

				parser->next();

				switch (Hash(peek.GetTag()))
				{
				case "bind"_h:			//Bind needs to push back a type name
					attach.EmplaceChild(ParseModule::TryModule<IdentifierParser>(parser, nullptr)).SYNTAX().type = SyntaxType::Scopename;
					break;

				case "intrinsic"_h:		//Intrinsic needs to push back a category name, and index.
				case "interface"_h:		//interface needs to push back category name and index.
					requires_body = false;
					attach.PushBackChild(HandleInterfaceIndex(parser));
					break;
				

				default:
					parser->croak("Unexpected keyword used after class/struct type name.");
					break;
				}

				settings.PushBackChild(attach);

			}
			else{
				settings.PushBackChild(Parser::CreateExpression("data", SyntaxType::Header));
			}

			

			//This is actually completely optional
			if (parser->SkipIfType(TokenType::Punctuation, ":") == true)
			{
				parser->croak("inheritence unsupported");
			}
			else {
				settings.PushBackChild(Parser::CreateExpression("<no child>", SyntaxType::Header));
			}

			type.PushBackChild(settings);


			if (parser->SkipIfType(TokenType::Punctuation, "{") == true)
			{
				Record body = Parser::CreateExpression("body", SyntaxType::Header);

				auto end_of_this = [&]() -> bool
				{
					return parser->SkipIfType(TokenType::Punctuation, "}");
				};

				while (end_of_this() == false) {
					body.EmplaceChild(parser->ParseExpression());

					//I would regardless like to end this off with ";" if possible
					//Also, this needs a function called Clear instead, where it will continuously loop until it no longer needs to skip.
					if (end_of_this() == false) parser->SkipType(TokenType::Punctuation, ";");
				}

				type.PushBackChild(body);
			}
			else if (requires_body)
			{
				parser->croak("type requires a body.");
			}
			
			return type;
		}
	};





	//Wont handle for right now.
	struct UnaryParser : public ParseSingleton<UnaryParser, true>
	{

		//TODO:Operators need to split, as it's pretty hard without.
		// precedence can handle this I think, instead of just raw numbers what I can do is use
		//

		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//I can saw with a fair bit of certainty that this is not going to work well when encountering errors.
			// The unary CanHandle needs to be far more discriminating.

			bool is_unary = true;
			//unary is a bit tricker, some things actually have shared symbols (like the minus) so I'll have to be specific
			if (!is_unary || parser->IsType(TokenType::Operator) == false)
				return false;

			//Previous must either an operator, or a non-literal

			auto prev = parser->prev();

			if (prev && (prev.TOKEN().type != TokenType::Operator || prev.GetTag() == "."))
				return false;

			return !target;

			//The shit above is so temporary it will make your head fucking spin. There are targeting unary operators
			// but I haven't the set up for them.

			
			//If there either was nothing of the previous bit was an operator.
			return !target && (!prev || prev.TOKEN().type == TokenType::Operator);
		}
		
		bool ContextAllowed(ProcessContext* context, ProcessChain* chain)  override
		{
			Parser* parser = chain->process->As<Parser>();

			//If last isn't .
			
			
			//if (NULL_OP(parser)->prev().GetTag() == ".")
			//	return false;
			//RGL_LOG(info, "TEST {} {}", BinaryParser::GetSingleton()->GetContext() != context->GetContext(), NULL_OP(parser)->);
			//And if the parser isn't binary OR is a member access. (This is done to get all member accesses out of the way before it shoves it in it's body.
			return BinaryParser::GetSingleton()->GetContext() != context->GetContext() || (NULL_OP(parser)->IsType(TokenType::Operator, "."));
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

			if (!target)
			{
				RecordData op = parser->next();

				//Not a strong name to make this a header, because it's handled pretty straight forward.
				// Also, more likely this seeks to parse atomic
				//Record body = parser->ParseExpression();
				Record body = parser->ParseAtomic();

				unary = parser->CreateExpression(op, SyntaxType::Unary, body);
			}
			else
			{
				parser->croak("Fuck you");

			}

			return unary;
		}

		bool IsAtomic() override { return true; }
	};

	struct ParenthesisParser : public ParseSingleton<ParenthesisParser, enabling_>
	{
		//This is basically used in most situations where a lose level of control wishes to be
		// applied 


		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//It will not parse nested and must be within a function statement.
			//Disabling this for now.
			return !target && parser->IsType(TokenType::Punctuation, "(");// && parser->contextChain->IsContextType("FunctionStatement");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{

			parser->SkipType(TokenType::Punctuation, "(");
				
			Record result = parser->ParseExpression();

			parser->SkipType(TokenType::Punctuation, ")");

			return result;
		}


		bool ContextAllowed(ProcessContext* context, ProcessChain*)  override
		{
			//parenthesis will not process statements, so no if's no switches, no functions, etc. Probably want to do something to prevent it from handling types as well.
			// But I'm kinda lazy ngl
			return !context->IsContextType("Statement");
		}

		bool IsAtomic() override { return true; }
	};

	
	struct CallParser : public ParseSingleton<CallParser>
	{
		
		bool CanHandle(Parser* parser, Record* target, bool atomic) const override
		{
			//VarUsage basically means no type allowed.
			return parser->IsType(TokenType::Punctuation, "(") && target && target->SYNTAX().type == SyntaxType::VarUsage;
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			target->SYNTAX().type = SyntaxType::Call;
			
			//I believe this makes header the contained object, rather than creating an expresion. Also changed to use the version that takes functions with just 1 member.
			//target->EmplaceChildren(Record{ "args", ExpressionType::Header, parser->Delimited("(", ")", ",", [=](auto, auto) { return parser->ParseExpression(); }) });
			auto args = Parser::CreateExpression("args", SyntaxType::Header, parser->Delimited("(", ")", ",", &Parser::ParseExpression));
			
			target->EmplaceChildren(args);
			
			return *target;
		}

		bool IsAtomic() override { return true; }
	};


	//*/
#endif


	

}