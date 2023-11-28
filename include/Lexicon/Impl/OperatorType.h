#pragma once


namespace LEX
{

	//This isn't used anymore but could be useful at defining operators at a later point. if so, just make it so these operators match the operators of
	// Instruct type exactly, making sure the total value of this is the same as the estimated total value of the other.

	ENUM(OperatorType)//Was enum macro
	{
		//The unknowns
		//This is not called target access, look up operator name. It's name is member access. I think I'll continue with target access, as later 
		// -> will be used to indicate access to a struct.
		TargetAccess,	//Don't know where this goes, but it's high. Perhaps just under parenthesis, but not lower than single operator. (This is correct, same as call)
		Call,	//Call is now an operator.
		//NullInstruct,//No need this for this.
		ArrayAccess,//Array access is called subscript

		//Parentheses
		Parenthesis,

		//Single Operators
		BitwiseNOT,
		LogicalNOT,
		UnaryPlus,	//
		UnaryMinus,

		//3D
		Exponent,

		//2D
		Multiply,
		Division,
		Modulo,

		//1D
		Addition,
		Subtract,

		//Shift
		RightShift,
		LeftShift,

		//Relation
		GreaterThan,
		LesserThan,
		GreaterOrEqual,
		LesserOrEqual,

		//Equality
		EqualTo,
		NotEqualTo,
		AbsEqualTo,
		AbsNotEqualTo,

		BitwiseAND,

		BitwiseXOR,

		BitwiseOR,

		LogicalAND,

		LogicalOR,

		BinaryLimit,//Passing this should mean that things after this are no longer binary. organize later.

		//These jump values make ++ and -- easier
		Jump_Parenthesis = OperatorType::Parenthesis,//This will not exist going forward, rather this is moved into a place where "Operators" are interpreted.
		Jump_Not = OperatorType::BitwiseNOT,//This is actually the jump for single operators. This allows us to ignore these if needed.
		Jump_3D = OperatorType::Exponent,
		Jump_2D = OperatorType::Multiply,
		Jump_1D = OperatorType::Addition,
		Jump_Shift = OperatorType::RightShift,
		Jump_Relation = OperatorType::GreaterThan,
		Jump_Equality = OperatorType::EqualTo,
		Jump_BitAND = OperatorType::BitwiseAND,
		Jump_BitXOR = OperatorType::BitwiseXOR,
		Jump_BitOR = OperatorType::BitwiseOR,
		Jump_LogAND = OperatorType::LogicalAND,
		Jump_LogOR = OperatorType::LogicalOR,

		Jump_Highest = OperatorType::Jump_Parenthesis,
		Jump_Lowest = OperatorType::Jump_LogOR,



		Total,
		Invalid = OperatorType::Total
	};

}