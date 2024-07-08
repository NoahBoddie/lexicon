#pragma once 


#include "Lexicon/Engine/ConcretePolicy.h"

namespace LEX
{
	//TODO: Due to the existence of VoidPolicy I will need a "IsPolicyValid" function.
	// or something like that to say it's an instantiatible type.

	//Also additionally, this shit should be a singleton.
	class VoidPolicy : public ConcretePolicy
	{
	public:

		//I don't think I'd even want to do this.
		//using ConcretePolicy::ConcretePolicy;

		//Should do nothing.
		VoidPolicy();
	};
}