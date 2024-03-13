#pragma once 


#include "ConcretePolicy.h"

namespace LEX
{
	//TODO: Due to the existence of VoidPolicy I will need a "IsPolicyValid" function.
	// or something like that to say it's an instantiatible type.

	//Also additionally, this shit should be a singleton.
	class VoidPolicy : public ConcretePolicy
	{
	public:

		using ConcretePolicy::ConcretePolicy;

		//Unlike any other 
		VoidPolicy() 
		{
			//This does nothing basically I guess.
		}
	};
}