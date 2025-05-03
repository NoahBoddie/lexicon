#pragma once

namespace LEX
{
	//Still not feeling the name. maybe statement? More broad.
	enum struct SyntaxType : uint16_t
	{
		//Things called UNHANDLE should be under total, as they should never make it to the compiler. Anything under that should be removed by compiling time.

		Script,
		Project,
		Number,
		Integer,
		String,
		Object,
		Boolean,
		Field,
		Variable, 
		

		
		Scriptname,		//Reintroducing these, basically do nothing but can be used to specify "Just get property from the environment."
		Scopename,//Basically is Scriptname, but static class functions might be introduced.
		Call,
		Function,		//Completely different than a call, this is more focused toward the declaration/definition of a function
		Return,
		StateBlock,				//The declaration of a code block, a manually denoted one. express for expression, state for statement
		ExpressBlock,			//The declaration of a code block, a manually denoted one. express for expression, state for statement
		Typename,		//The type name of something. This encapsulates similar to how call will, but not on the same hierarchy of operators.
		Type,
		Format,			//A change in language format. Can be select from TOML, JSON, etc
		//Operators
		//Assign,
		Unary,
		Binary,
		Cast,	//X?		//Basically an operator I guess. Might be binary. Basically only for down casting
		//Statements
		
		Conditional,
		If = Conditional,
		While = Conditional,
		For = Conditional,
		
		Typeof,



		//Individual operators will not be used, I can just have something hash into an
		//I mean, I feel like I could just have something hash an operator into a literal integer, and that would work well enough into getting it's operator code.
		// So maybe not needed.

		//How to parse out the intent of a type name, if an identifier is found and an identifier is next? or following an "as" statement.

		//THE NEW SYNTAX

		ProjectName,
		

		Total,//Any syntax greater or equal to this isn't able to be used within compiling functions.
		
		Invalid = Total,



		Path,			//Used to tell if something is a pathing record used as a header for a scope deduction.
		SpecifyGlobal,
		SpecifyProject,
		SpecifyShared,
		SpecifyScript,
		SpecifyCommons,
		SpecifyType,




		Header,
		Disposable,
		Identifier,
		Declare,
		Requirement,
		Relationship,
		
		Directive,

		Prefunc,

		//Aliases- Short hand that makes my job a bit easier.
		None = Total,  //No type. Purely name.
		Declaration = Declare,  //used as a pivot between functions, variables, properties, etc, deciding which it is based on what comes next.
	};
}