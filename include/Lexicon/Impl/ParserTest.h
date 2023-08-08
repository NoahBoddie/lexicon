#pragma once

#include "Lexicon/Impl/Parser.h"

namespace LEX::Impl
{

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

	//TODO:Merge Literal and ObjectLiteral parse modules, they were seperated off of an ill concieved concept of depth.
	// Depth is no longer implemented, and nor should object literals being completely seperate.

	//Should drop the parser part of the name, that way it's easy to identify for context. Then put them in a namespace called parsers.
	struct LiteralParser : public ParseSingleton<LiteralParser>
	{
		uint32_t GetPriority() const override
		{ 
			return ModulePriority::High; 
		}


		bool CanHandle(Parser* parser, Record* target) const override
		{
			auto peek = parser->peek();
			
			switch (peek.TOKEN().type)
			{
			case TokenType::Boolean:
			case TokenType::Number:
			//case TokenType::Object:
			case TokenType::String:
				return true;
			}
			
			return false;
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
					return parser->CreateExpression(next, ExpressionType::Boolean);
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

					return parser->CreateExpression(next, ExpressionType::Number);
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
				return parser->CreateExpression(next, ExpressionType::Number);
			}

			default:
				//TODO:Fix Format #4
				parser->croak("Invalid token as literal");//std::format("invalid token '{}' detected as a literal.", tag));
				break;
			}
				
			//Technically should be unreachable.
			throw nullptr;
		}


	};

	//Object is shaping to function a little weird, so I'm setting the depth to be something a bit different.
	struct ObjectLiteral : public ParseSingleton<ObjectLiteral, false>
	{

		uint32_t GetPriority() const override
		{
			return ModulePriority::Max;
		}


		bool CanHandle(Parser* parser, Record* target) const override
		{
			return parser->IsType(TokenType::Object);
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			if (!target || target->EXPRESSION().type != ExpressionType::Typename) {
				parser->croak("Placeholder: No type detected");
				//Or type name detected
			}

			auto next = parser->next();

			auto& tag = next.GetTag();


			//Clears the ":{" and "}" items.
			ClipString(tag, 2, 1);
				
			return parser->CreateExpression(next, ExpressionType::Object, *target);
		}


		std::string_view GetContext() override
		{
			//Basically the same as this type.
			
			return TypeName<LiteralParser>::value;
		}
	};

	//When a depth has a hit, I think it should just fucking restart

	//
	//{ScriptName(VariableParser)}{::(ScriptNameParser)}{TypeName(Variable[nesest in typename])} {Variable} (call/op/function)

	//Variable
	//ScriptName detected, forcibly reads next and nests it under the target record (or creates an empty scriptname as replacement).
	//*TypeName is consumed
	//<!>@1
	//Variable(or in this case, very specifically Variable declaration) kicks in with an identifier. The Scriptname/type name are put into it.
	// if I choose to, I can make sure that the previous record (which should probably have a function check for a type name below if a script name is found).
	//<!> TO make sure one is on speed with the process, right now we have a DeclaredVariable that has it's first child (it's type) set to a scriptname that contains
	// a type name.
	//From here, if "(" is detected, 2 things happen. In script scope, this is a function. Else, it's a call. But if a type is detect, it will say
	// ";" expected. I may be able to do something with that syntax later, maybe specifically for constructables or something idk. I'll leave it for now.
	// So to reiterate how it will tell the difference, declare and use var are different. Call must be on UseVar, like name.
		
	//@1 A big thing to note here, is that "(" and "{" should be allowed here too, even if it's a type name. Syntax wise though, call handles it.
	// Expression wise, I think it will be called Construct

	//These functions should likely instead ask, (can you do this depth). I think that might be the better option, so it can have multiple.
	//Because I just realized the "::" can be the first thing too.

	//Revision, fuck depth. There's only 2 depths now, and there's only ones that come after (which use target) and the ones that dont
	// the rest is handled via priority

	//Also might differentiate global and variable expressions. Might help some.



	
	struct VariableParser : public ParseSingleton<VariableParser>
	{
		//Variable doesn't really seem to work name wise, as this thing works multiple jobs.


		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target) const override
		{
			return parser->IsType(TokenType::Identifier);
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			if (target) {//Unsure of, but basically if this is a nested 
				//At this point we know that what it is has to be a type (but I may want to condition that), so we edit the first child of target
				// to be typename
					
				target->GetFront()->EXPRESSION().type = ExpressionType::Typename;

				//I'm doing this to make a place for type name, might be wrong actually.
				//return parser->CreateExpression(parser->next(), ExpressionType::Variable, Record{ "type", ExpressionType::Total, *target });
				return parser->CreateExpression(parser->next(), ExpressionType::VarDeclare, Record{ "type", ExpressionType::Total, *target });
			}
			else {
				//Technically about the use, rather the declaration
				return parser->CreateExpression(parser->next(), ExpressionType::VarUsage);
			}
		}
	};

		
	struct ScopeParser : public ParseSingleton<ScopeParser>
	{
		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target) const override
		{
				
			//TODO:Make using context chains easier PLEASE
			//TODO:Test out directly getting FunctionParser instead.
			//If there is no nested target AND we are in a function definition AND the next type is a puncutation with '{'
			return !target && parser->contextChain->IsContextType("FunctionStatement") && parser->IsType(TokenType::Punctuation, "{");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			auto parse_func = [](Parser* _psr, Record*) -> Record
			{
				return _psr->ParseExpression();
			};

			//TODO:Address the type that this expression should work.
			Record scope{};

			std::vector<Record> children = parser->Delimited("{", "}", ";", parse_func);
			if (children.size() == 0) return scope;
			if (children.size() == 1) return children[0];

			scope.EmplaceChildren(children);

					
			return scope;
		}
	};


	struct ScriptParser : public ParseSingleton<ScriptParser, false>
	{
		//singleton, but not allowed to be registered, must be used manually.

		//Not registered, no need for priority.

		//Script parser cannot launch other than manually.
		bool CanHandle(Parser* parser, Record* target) const override { return false; }

		Record HandleToken(Parser* parser, Record* target) override
		{
			Record script = Parser::CreateExpression(parser->name(), ExpressionType::Script);
				
			script.EmplaceChild(Parser::CreateExpression(parser->project(), ExpressionType::Project));

			while (parser->eof() == false) {
				RGL_LOG(info, "tag: {}", parser->peek().GetTag());
				script.EmplaceChild(parser->ParseExpression());

				//I would regardless like to end this off with ";" if possible
				if (parser->eof() == false) parser->SkipType(TokenType::Punctuation, ";");
			}

			return script;
		}
	};


	struct ScriptnameParser : public ParseSingleton<ScriptnameParser>
	{
		//Type name is handled by detecting the puncutation ::


		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target) const override
		{
			//Can be atomic or Nested.
			return parser->IsType(TokenType::Punctuation, "::");
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			if (target) {
				if (target->EXPRESSION().type != ExpressionType::Variable && target->EXPRESSION().type != ExpressionType::VarName)
					parser->croak("Expected identifier before scriptname punctuation (may be dev error)");

				//Basically "Scriptname::Type Sciptname::Identifier..." shouldn't happen.
				if (target->FindChild("type") != nullptr)
					parser->croak("Expected identifier without type.");
			}

			RecordData scriptname = target ? target->GetData() : parser->next();

			if (parser->IsType(TokenType::Identifier) == false){
				parser->croak("Expecting Identifier after scriptname punctuation");
			}

			//At this time, we don't know what comes next is actually a type name, could be a function. So for now, VarName
			return parser->CreateExpression(target->GetData(), ExpressionType::Scriptname, parser->CreateExpression(parser->next(), ExpressionType::VarDeclare));
		}
	};


	struct ArrayDeclareParser
	{
		//This isn't set in stone but the idea should be that if the previous type looks to be a VarDeclare, as the next token it looks
		// for puncutation of "[", and immediately skips both, then adding into a section for the types called qualifiers.
		// in this section i can probably just make one string instead, but what I'll do is have one string that has a bunch of qualifiers begin and end with /
		// so it'll search for "/isarray/" when getting the type.

		//Note, these qualities
	};


	struct BinaryParser : public ParseSingleton<BinaryParser>
	{
		bool CanHandle(Parser* parser, Record* target) const override
		{
			//RecordData token = parser->peek();
			
			bool is_binary = true;
			//Target must not be binary either, because it's not possible to have 2 binary next to each other.
			return target && parser->IsType(TokenType::Operator) && is_binary;
		}


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
				{"%", 20}
			};

			RecordData tar = parser->peek();

			//TODO:Binary parser will need to work with unary later on, please be advised.

			int his_prec = PRECEDENCE[tar.GetTag()];
			if (his_prec > my_prec) {
				parser->next();

				//Used to use parse atom, be advised
				
				Record a_lhs = Parser::CreateExpression("left", ExpressionType::Header, { left });
				
				Record a_rhs = Parser::CreateExpression("right", ExpressionType::Header, { _HandleBinary(parser, parser->ParseExpression(), his_prec) });

				
				Record pivot = parser->CreateExpression(tar, ExpressionType::Binary, a_lhs, a_rhs);

				return _HandleBinary(parser, pivot, my_prec);
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


	struct ParameterParser : public ParseSingleton<ParameterParser, false>
	{
		bool CanHandle(Parser* parser, Record* target) const override
		{
			//This should probably basically never fire unless used by name.
			return parser->IsType(TokenType::Identifier);
		}

		static ParameterParser* test()
		{
			return nullptr;
		}

		int testers() {
			return 1;
		}

		void T(int i = test()->testers() ? 1 : 2)
		{

		}
		Record HandleToken(Parser* parser, Record* target) override
		{
			Record base;

			//It will do all these in sequence, and fail should any of them, that's a parsing failure.

			if (parser->IsType(TokenType::Identifier) == true) {
				base = ParseModule::TryModule<VariableParser>(parser, nullptr);
				target = &base;
			}
				
			if (parser->IsType(TokenType::Punctuation, "::") == true) {
				base = ParseModule::TryModule<ScriptnameParser>(parser, target);
				target = &base;
				ParseModule::TryModule<VariableParser>(parser, target);

			}

			if (!target)
				parser->croak("PLACEHOLDER: iden nor punc checks worked.");

			//if (parser->IsType(TokenType::Punctuation, "[") == true)
			//	ParseModule::TryModule<Array>(parser, nullptr);
				

			ParseModule::TryModule<VariableParser>(parser, target);

			//Going into default evaluation, note, this is when Parameters rules of initialization become a fair bit restrictive.
			if (parser->IsType(TokenType::Operator, "=") == true) {
				Record def{ "default", ExpressionType::Total, ParseModule::TryModule<BinaryParser>(parser, target) };
				base.EmplaceChildren(def);
			}

			//If everything is here, it should be a VarDeclare Expression, with a type, qualifier, and default records denoting the respective settings.
			return base;
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


		uint32_t GetPriority() const override
		{
			return ModulePriority::High - 50;
		}


		bool CanHandle(Parser* parser, Record* target) const override
		{
			//Invalid if target is not present.
			// maybe enforce VarDeclare
			return parser->IsType(TokenType::Punctuation, "(") && target && target->EXPRESSION().type == ExpressionType::VarDeclare;
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			//Need to figure out how to parse a var name right here, that's a part of the function of a function parser.
			//To do it, perhaps it can call the things it needs in order. Variable parser, scriptname parser, variable parser, variable parser,
			// assignment


			//The target needs to exist, else croak.
			if (!target) {
				parser->croak("Expected identifier before scriptname punctuation (may be dev error)");
			}
			else if (target->EXPRESSION().type != ExpressionType::VarDeclare) {
				parser->croak("PLACEHOLDER, expected type name or something like that.");
			}
			//The target type needs to be changed to function here.


			target->EXPRESSION().type = ExpressionType::Function;

			//bool first = false;
			#ifdef CloseOut
			auto _delegate = [&](Parser* parser, Record*)
			{
				//THIS version checks for the extension function syntax which changes what a static function extends. But it's rough, because I actually want
				// the internal syntax to exist without having to do "first_arg" all over the place.
				// I think instead what I need is another parser, one that only comes out to play when the current context is parsing Parameter.
				//The extension parser. It has to be branching, has to be a param type, and it has to have 

				Record result{};

				bool is_extension = parser->IsType(TokenType::Keyword, "this");

				if (is_extension)
				{
					if (!first){
						parser->croak("Only first parameter can denote an extension function.");
					}

					//I can do other things here, like make sure that a static qualifier exists.

					parser->next();//discard the this.
				}
					
				result = ParseModule::ExecuteModule<ParameterParser>(parser, target);

				//May want to do something where I don't have to specify type name here, but maybe only have it work for
				if (is_extension)
				{
					//result should have a type, so we'll want to take that, and make a record called
					// extends on the target.

					target->EmplaceChildren(Record{ "extends", result.FindChild("type") });
				}
					

				first = false;

				return result;

			};
			#endif

			//I realize all this ain't super needed. To account for extension, all I need to do is remove the first entry if it's named this, HERE,
			// and copy type, change it's name to extends, place it on the target. and Pop it out. Before that, maybe check for other thingy mabobs.
			// I also note that the parser module is VERY hands on, so I should use that to sort out which is using this or not.
			// I can just have the lambda check for each thing named this after the first entry, and then cull it.
			target->EmplaceChildren(Record{ "params", ExpressionType::Total, parser->Delimited("(", ")", ",", ParseModule::TryModule<ParameterParser>) });
			//target->EmplaceChildren(Record{ "params", ExpressionType::Total, parser->Delimited("(", ")", ",", _delegate) });

			//Ensures that the proper token is there.
			parser->SkipType(TokenType::Punctuation, "{");

			target->EmplaceChildren(Record{ "code", ExpressionType::Total, parser->ParseExpression() });

			parser->SkipType(TokenType::Punctuation, "}");

			return *target;
		}

		//Context as a concept hits a snag here, because this would need 2.
	};


	//A bunch parser, this doesn't deal with this (as its hard to detect in situations other than "iden iden"
	//[[deprecated("Handled by variable basically.")]]
	struct TypeParser : public ParseSingleton<TypeParser>
	{
		//I may use this at some other point though.

		bool CanHandle(Parser* parser, Record* target) const override
		{
			return parser->IsType(TokenType::Identifier);
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			//This isn
			if (!target || target->EXPRESSION().type != ExpressionType::Variable) {
				parser->croak("Placeholder: No variable detected");
			}
				
			parser->SkipType(TokenType::Punctuation, "::");

			return parser->CreateExpression(target->GetData(), ExpressionType::Typename);
		}
	};



	//Wont handle for right now.
	struct UnaryParser : public ParseSingleton<UnaryParser, true>
	{

		int operator= (int rhs)
		{
			return 0;
		}

		//TODO:Operators need to split, as it's pretty hard without.
		// precedence can handle this I think, instead of just raw numbers what I can do is use
		//

		bool CanHandle(Parser* parser, Record* target) const override
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
				Record body = parser->ParseExpression();

				unary = parser->CreateExpression(op, ExpressionType::Unary, body);
			}
			else
			{
				parser->croak("Fuck you");

			}

			return unary;
		}

	};

	struct ParenthesisParser : public ParseSingleton<ParenthesisParser>
	{
		//This is basically used in most situations where a lose level of control wishes to be
		// applied 


		uint32_t GetPriority() const override
		{
			return ModulePriority::High;
		}


		bool CanHandle(Parser* parser, Record* target) const override
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
	};

		

	#ifdef Blackout
	struct AssignParser : public ParseSingleton<AssignParser>
	{
		//This shouldn't expressly handle all assignments, just '='. But if it has '=' in it, it will do a jump directly to this?
		// This doesn't work because assign will need to consume '=', which won't exist or will be consumed by binary.

		//I got it, it only consumes if its nested, but if it's atomic it will only generate the left statement (without ; either).

			
		//ok so last problem is that this fucks with what binary does.
		// So maybe, I'll just do what binary does. The idea of a single line
	};

	struct QualifierParser : public ParseSingleton<QualifierParser>
	{

	};


	struct AttributeParser : public ParseSingleton<AttributeParser>
	{

	};
	#endif
	struct CallParser : public ParseSingleton<CallParser>
	{

		bool CanHandle(Parser* parser, Record* target) const override
		{
			//VarUsage basically means no type allowed.
			return parser->IsType(TokenType::Punctuation, "(") && target && target->EXPRESSION().type == ExpressionType::VarUsage;
		}

		Record HandleToken(Parser* parser, Record* target) override
		{
			target->EXPRESSION().type = ExpressionType::Call;

			target->EmplaceChildren(Record{ "args", ExpressionType::Total, parser->Delimited("(", ")", ",", [=](auto, auto) { return parser->ParseExpression(); }) });

			return *target;
		}
	};


	//*/
}