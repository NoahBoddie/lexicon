#pragma once


namespace LEX
{
	//These are likely going to be the user operations, but my operations would be closer
	// to being called isntructions.
	ENUM(InstructionType, uint16_t)
	{
		Pass,

		//Operators (Operator and created instructs are one in the same)
		BitwiseXOR,
		BitwiseOR,
		BitwiseAND,
		RightShift,
		LeftShift,
		//These NOT operators are a bit too spicy for this right now, as they're single way operators. That'll need work.
		BitwiseNOT,
		LogicalNOT,
		UnaryPlus,//This is a useless entry, that has more to do with turning something into an integral or something.
		UnaryMinus,


		GreaterThan,
		LesserThan,
		GreaterOrEqual,
		LesserOrEqual,
		EqualTo,
		EqualAbsTo,
		NotEqualAbsTo,
		NotEqualTo,
		LogicalOR,
		LogicalAND,

		//Sorting area
		Subtract,
		Addition,
		Division,
		Multiply,
		Modulo,
		Exponent,
		Access,

		Assign, 

		//Directives (Instructions created don't match 1 to 1 the operator used)	
		Push,
		Move, 
		Forward,
		PushVariable,//Pushes something from the left to a variable spot in the right. Perhaps in the future I could make registers spot just do this.



		IncrementArgStack,//PrepArgs
		IncrementVarStack,
		DefineVariable,		//Defines a variable as it's default. May not fully define. TODO: Remove DefineVarPolicy, it is basically a move instruction.
		DefineParameter,	//Defines a parameter merely by assigning it's storage type.
		// For these I could make them the same function, but make an index what makes them differ. In that I could send it it's index, similar to how
		//  native functions get a string view of their names.
		Test,//General test
		ObjectTest,//Test that only exists for objects.




		Call,
		Convert,				//Handles the call as it would a conversion. This doesn't need to allocate the argument stack to be performed.
		Construct,				//Constructs the default uninitialized object.
		ReturnDefault,
		DropStack,
		DropStackN,//This will drop if positive instead of if negative.
		JumpStack,
		Return,					//
		Total,
		IncArgStack = InstructionType::IncrementArgStack,
		IncVarStack = InstructionType::IncrementVarStack,

		Invalid = InstructionType::Total,
	};

	using InstructType = InstructionType;
}