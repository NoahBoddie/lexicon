#pragma once


//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"

#include "OverloadKey.h"
#include "Solution.h"

//*src
#include "Overload.h"
#include "OverloadClause.h"
#include "TargetObject.h"
namespace LEX
{
	struct ITypePolicy;
	struct TargetObject;

	class OverloadInput : public OverloadKey
	{
	public:

		//Make these 2 a single function.
		Overload MatchFailure(OverloadFlag& flag, Overload* prev = nullptr)
		{
			logger::critical("Force failure");

			//This should only be used when it's an ambiguous match I think.
			flag |= OverloadFlag::Failure;
			//This feels very unnecessary and possibly unused.
			if (prev)
				return *prev;

			return {};
		}

		Overload MatchAmbiguous(OverloadFlag& flag)
		{
			logger::critical("Force ambiguous");

			//This should only be used when it's an ambiguous match I think.
			flag |= OverloadFlag::Ambiguous;

			return {};
		}

		QualifiedType GetTarget() const override
		{
			auto tar = object->GetCallTarget()->target;

			return tar ? *tar : QualifiedType{};
		}

		//This boolean needs to say if this failed to match, failed to be better, or resulted in ambiguity.
		virtual Overload Match(OverloadClause* clause, ITypePolicy* scope, Overload* prev, OverloadFlag& a_flag) override
		{

			if (defaultInput.empty() == false) {
				a_flag |= OverloadFlag::UsesDefault;
			}


			if (clause->PreEvaluate(nullptr, paramInput.size(), defaultInput.size(), a_flag) == false) {
				logger::info("pre-eval fail");
				return MatchFailure(a_flag);
			}


			//I want to phase out of function. Maybe combine it with prev in some way.

			//Make a copy as to not completely mutate this.
			OverloadFlag flag = a_flag;


			Overload overload;

			QualifiedType type;

			if (object && object->target)
				type = *object->target;


			OverloadEntry tar = clause->EvaluateEntry2(type, scope, -1, -1, flag);


			if (flag & OverloadFlag::Failure)
				return MatchFailure(a_flag, prev);


			//Also a thought, [this] is a parameter too. Granted a hidden one.

			overload.target = tar.type;
			overload.clause = clause;

			constexpr auto offset_placeholder = 0;

			int winner = 0;

			for (int i = 0; i < paramInput.size(); i++)
			{
				QualifiedType input = paramInput[i];

				OverloadEntry entry = clause->EvaluateEntry2(input, scope, offset_placeholder, i, flag);

				if (flag & OverloadFlag::Failure)
					return MatchFailure(a_flag, prev);



				//entry.funcs = conversion;
				//entry.type = input;
				//entry.index = index;

				//Compare should this input-> prev vs entry
				auto new_winner = prev->SafeCompare(i, entry, input);

				if (!winner)
					winner = new_winner;
				else if (winner < 0 && new_winner > 1 || winner > 0 && new_winner < 1)
				{// shit isn't valid anymore.
					return MatchAmbiguous(a_flag);
				}


				overload.given.push_back(entry);
			}

			auto it = defaultInput.begin();

			for (int i = 0; it != defaultInput.end(); i++, it++)
			{
				QualifiedType input = it->second;

				OverloadEntry entry = clause->EvaluateDefault2(input, scope, it->first, flag);

				if (flag & OverloadFlag::Failure)
					return MatchFailure(a_flag, prev);


				//entry.funcs = conversion;
				//entry.type = input->second;
				//entry.index = index;


				auto new_winner = prev->SafeCompare(it->first, entry, input);

				if (!winner)
					winner = new_winner;
				else if (winner < 0 && new_winner > 1 || winner > 0 && new_winner < 1)
				{// shit isn't valid anymore.
					return MatchAmbiguous(a_flag);
				}


				overload.defaults[it->first] = entry;
			}


			if (!winner){
				winner = prev->SafeCompare(-1, tar, type);
			}



			if (winner > 0) {
				logger::info("right winner");
				//This is just failure tbh, doing so means I don't need to use move or set the pointer.
				return *prev;
			}
			else if (winner < 0) {
				logger::info("left winner");
				return overload;
			}
			else {
				logger::info("no winner {}" , prev != nullptr);
				return MatchAmbiguous(a_flag);
			}

		}





		TargetObject* object = nullptr;
		std::vector<ITypePolicy*>			genericInput;
		
		//On reflection, this doesn't expressly need to be a solution, just a qualified type. This can help with syntax that
		// parses which overload to use by showing which function is being used.
		std::vector<Solution>				paramInput;
		std::map<std::string, Solution>		defaultInput;//rename
	};

}