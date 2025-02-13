#pragma once

namespace LEX
{
	enum struct ScopeType : uint8_t
	{
		Required,		//It's a given that the parent scope will have to run the following when encountered
		Conditional,	//The parent scope may not run the contents of this scope when encountered
		Depedent,		//Whether the parent scope runs the contents of this scope are dependent on another scope.
		Header,			//The header scope, the only things that go here would be parameters and such. Cannot have a parent.
		Temporary,

		//Header is the default scope,
		//Required is any scope that will be encountered (say {..})
		//Condition is stuff like ifs and while loops.
		//Dependent means it's reliant on a conditional to trigger. 
		// I'm considering something like trivial, say if something's set within an if statement.
	};

}