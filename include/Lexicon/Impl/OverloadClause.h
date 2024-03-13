#pragma once

#include "Overload.h"
#include "OverloadKey.h"

namespace LEX
{

	//2 clauses with identical keys may co-exist as long as their options are differing, as well as their use
	// the caveat is that it will produce a warning
	struct OverloadClause : public OverloadKey
	{

		//All functions and all generic entities (So stuff that comes with ISomething) should derived from overload clause in some way.
		// The interface types can merely make it a pure function to be implemented.

	public:
		OverloadClause* AsClause() override { return this; }


		Overload MatchKey(OverloadKey* key)
		{
			//This should work on a nullptr too.


			
			Overload result;

			if (!this)
				return result;


			//Note, this function is largely a work in progress, even if largely it's not needed.



			auto key_flags = key->GetBasicFlags();

			switch (GetVolatility())
			{
			case BasicQualifier::Modable:
				if (key->IsModable() == false)//May possibly make mutable prefered, unsure.
					result += 1;
				break;

			case BasicQualifier::Mutable:
				if (key->IsModable() == true)
					result.Unmatch();
				else if (key->IsConst() == true)
					result += 1;
				break;

			case BasicQualifier::Const:
				if (key->IsConst() == false)
					result.Unmatch();

				break;
			default: //Should throw				
				report::apply::warn("Shouldn't even happen, but no volatility");
				result.Unmatch();
				break;
			}

			if (!result)
				return result;

			auto key_args = key->GetParamInput();

			auto clause_params = GetParamInput();

			if (key_args.size() != clause_params.size()) {
				result.Unmatch();
				return result;
			}

			return result;

			std::vector<uint64_t> hash_list;
			hash_list.resize(key_args.size());

			//For now I will call it here. Whatever happens after, is what it is. The only matching that can
			// happen will be having the right amount of arguments.
		}
	};

}