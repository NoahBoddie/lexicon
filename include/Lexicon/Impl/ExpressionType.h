#pragma once

namespace LEX::Impl
{
	//Still not feeling the name. maybe statement? More broad.
	enum struct SyntaxType
	{
		Script,
		Project,
		Directive,
		Number,
		Integer,
		String,
		Object,
		Boolean,
		Field,
		Variable, 
		
		//Scriptname,		//Reintroducing these, basically do nothing but can be used to specify "Just get property from the environment."
		Scopename,//Basically is Scriptname, but static class functions might be introduced.
		Call,
		Function,		//Completely different than a call, this is more focused toward the declaration/definition of a function
		Return,
		Block,			//The declaration of a code block, a manually denoted one. Basically scopes off shit.
		Typename,		//The type name of something. This encapsulates similar to how call will, but not on the same hierarchy of operators.
		TypeDeclare,
		Format,			//A change in language format. Can be select from TOML, JSON, etc
		//Operators
		Assign,
		Unary,
		Binary,
		Cast,	//X?		//Basically an operator I guess. Might be binary. Basically only for down casting
		//Statements
		If,
		While,
		For,//Should compile like a while statement

		//Individual operators will not be used, I can just have something hash into an
		//I mean, I feel like I could just have something hash an operator into a literal integer, and that would work well enough into getting it's operator code.
		// So maybe not needed.

		//How to parse out the intent of a type name, if an identifier is found and an identifier is next? or following an "as" statement.
		Total,

		//Aliases- Short hand that makes my job a bit easier.
		Invalid = Total,
		//TODO: Separate header.
		Header = Total,//Within the context of an expression class. 
		VarDeclare = Variable,		//Used if something might be a variable, but could be a call.
		VarUsage = Field,		//should handle constants and variables, and when assign is detected, the difference is handled at compile time. Otherwise for comparisons don't matter.
	};
	

	//The above will be ordered in such a way that I can piece out if it's an expression, statement, or belongs in a function
	//TODO: Correction, the pivot will be called syntax. Syntax will be broken down into 3 different types. 
	// Declarations things that create,
	// Statements things that do
	// and expressions things that return or are expected.

	/*
	namespace fmt
	{
		//Really want this to work

		template <>
		struct formatter<ExpressionType>
		{
			template <class ParseContext>
			constexpr auto parse(ParseContext& a_ctx)
			{
				return a_ctx.begin();
			}

			template <class FormatContext>
			auto format(const ExpressionType& a_enum, FormatContext& a_ctx)
			{
				return "";
				//auto* info = RE::ActorValueList::GetSingleton()->GetActorValue(a_actorValue);
				//return fmt::format_to(a_ctx.out(), "{}", info ? info->enumName : "None");
			}
		};
	}
	//*/

	//Unsure if I wish to inline this.
	inline std::string_view ExpressionToString(SyntaxType t, bool cap = true)
	{
		//deprecate this shit and use Magic enum PLEASE I'M BEGGING.

		switch (t)
		{
			case SyntaxType::Script:
				return cap ? "Script" : "Script";
			case SyntaxType::Project:
				return cap ? "Project" : "Project";
			case SyntaxType::Number:
				return cap ? "Number" : "Number";
			case SyntaxType::Integer:
				return cap ? "Integer" : "Integer";
			case SyntaxType::String:
				return cap ? "String" : "String";
			case SyntaxType::Object:
				return cap ? "Object" : "Object";
			case SyntaxType::Boolean:
				return cap ? "Boolean" : "Boolean";
			case SyntaxType::VarUsage:
				return cap ? "Field" : "Field";
			case SyntaxType::VarDeclare:
				return cap ? "Variable" : "Variable";
			//case SyntaxType::Scriptname:
			//	return cap ? "Scriptname" : "Scriptname";
			case SyntaxType::Call:
				return cap ? "Call" : "Call";
			case SyntaxType::Function:
				return cap ? "Function" : "Function";
			case SyntaxType::Block:
				return cap ? "Block" : "Block";
			case SyntaxType::Scopename:
				return cap ? "Scopename" : "Scopename";
			case SyntaxType::Typename:
				return cap ? "Typename" : "Typename";
			case SyntaxType::TypeDeclare:
				return cap ? "TypeDeclare" : "TypeDeclare";
			case SyntaxType::Format:
				return cap ? "Format" : "format";
			case SyntaxType::Assign:
				return cap ? "Assign" : "Assign";
			case SyntaxType::Unary:
				return cap ? "Unary" : "Unary";
			case SyntaxType::Binary:
				return cap ? "Binary" : "Binary";
			case SyntaxType::Cast:
				return cap ? "Cast" : "Cast";
			case SyntaxType::If:
				return cap ? "If" : "If";
			case SyntaxType::While:
				return cap ? "While" : "While";
			case SyntaxType::For:
				return cap ? "For" : "For";
			case SyntaxType::Return:
				return cap ? "Return" : "return";

			case SyntaxType::Invalid:
				return cap ? "Invalid" : "invalid";
		}
		return "UNNAMED";
	}
}