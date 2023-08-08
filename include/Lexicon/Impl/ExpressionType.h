#pragma once

namespace LEX::Impl
{
	//Still not feeling the name. maybe statement? More broad.
	enum struct ExpressionType
	{
		Script,
		Project,
		Number,
		Integer,
		String,
		Object,
		Boolean,
		VarName,		//Used if something might be a variable, but could be a call.
		Variable,		//should handle constants and variables, and when assign is detected, the difference is handled at compile time. Otherwise for comparisons don't matter.
		Scriptname,		//Reintroducing these, basically do nothing but can be used to specify "Just get property from the environment."
		Call,
		Function,		//Completely different than a call, this is more focused toward the declaration/definition of a function
		Block,			//The declaration of a code block, a manually denoted one. Basically scopes off shit.
		Typename,		//The type name of something. This encapsulates similar to how call will, but not on the same hierarchy of operators.
		Inline,			//Inlined language. Can be select from TOML, JSON, etc
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
		Header = Total,//Within the context of an expression class
		VarDeclare = VarName,
		VarUsage = Variable,
	};
	
	
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
	inline std::string_view ExpressionToString(ExpressionType t, bool cap = true)
	{
		switch (t)
		{
			case ExpressionType::Script:
				return cap ? "Script" : "Script";
			case ExpressionType::Project:
				return cap ? "Project" : "Project";
			case ExpressionType::Number:
				return cap ? "Number" : "Number";
			case ExpressionType::Integer:
				return cap ? "Integer" : "Integer";
			case ExpressionType::String:
				return cap ? "String" : "String";
			case ExpressionType::Object:
				return cap ? "Object" : "Object";
			case ExpressionType::Boolean:
				return cap ? "Boolean" : "Boolean";
			case ExpressionType::VarName:
				return cap ? "VarName" : "VarName";
			case ExpressionType::Variable:
				return cap ? "Variable" : "Variable";
			case ExpressionType::Scriptname:
				return cap ? "Scriptname" : "Scriptname";
			case ExpressionType::Call:
				return cap ? "Call" : "Call";
			case ExpressionType::Function:
				return cap ? "Function" : "Function";
			case ExpressionType::Block:
				return cap ? "Block" : "Block";
			case ExpressionType::Typename:
				return cap ? "Typename" : "Typename";
			case ExpressionType::Inline:
				return cap ? "Inline" : "Inline";
			case ExpressionType::Assign:
				return cap ? "Assign" : "Assign";
			case ExpressionType::Unary:
				return cap ? "Unary" : "Unary";
			case ExpressionType::Binary:
				return cap ? "Binary" : "Binary";
			case ExpressionType::Cast:
				return cap ? "Cast" : "Cast";
			case ExpressionType::If:
				return cap ? "If" : "If";
			case ExpressionType::While:
				return cap ? "While" : "While";
			case ExpressionType::For:
				return cap ? "For" : "For";
		}
		return "Invalid";
	}
}