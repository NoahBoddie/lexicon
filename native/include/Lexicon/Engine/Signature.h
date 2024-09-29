#pragma once

#include "Overload.h"
#include "OverloadKey.h"
#include "OverloadFlag.h"
#include "OverloadClause.h"

#include "Lexicon/SignatureBase.h"

namespace LEX
{

	struct Signature : public OverloadKey, public SignatureBase
	{
		//The match for this should be aimed to be as very little in terms of ambiguity as possible.
		//Thus, the stated project, script, and of course, the path.
		//The path for generics will probably simply be controlled by the amount of generic parameters it has, rather than the entires.
		// So no generic, no generic notation. Is generic, has generic notation. If it's the only one you can

		//Make these 2 a single function.
		Overload MatchFailure(OverloadFlag& flag)
		{
			logger::critical("Force failure");

			flag |= OverloadFlag::Failure;

			return {};
		}

		Overload MatchAmbiguous(OverloadFlag& flag)
		{
			logger::critical("Force ambiguous");

			flag |= OverloadFlag::Ambiguous;

			return {};
		}

		//Please note, this kind of match is what a clause should be doing.

		//This boolean needs to say if this failed to match, failed to be better, or resulted in ambiguity.
		Overload Match(OverloadClause* clause, ITypePolicy*, Overload*, OverloadFlag& a_flag) override
		{
			a_flag |= OverloadFlag::AllAccess;

			if (clause->CanMatch(result, parameters.size(), 0, a_flag) == false) {
				return MatchFailure(a_flag);
			}

			//I want to phase out of function. Maybe combine it with prev in some way.

			//Make a copy as to not completely mutate this.
			OverloadFlag flag = a_flag;

			Overload overload;

			OverloadEntry tar = clause->MatchSuggestedEntry(target, nullptr, -1, -1, flag);

			if (flag & OverloadFlag::Failure || tar.convertType != ConvertResult::Exact)
				return MatchFailure(a_flag);

			overload.target = tar.type;
			overload.clause = clause;

			constexpr auto offset_placeholder = 0;

			int winner = 0;

			for (int i = 0; i < parameters.size(); i++) {
				QualifiedType input = parameters[i];

				OverloadEntry entry = clause->MatchSuggestedEntry(input, nullptr, offset_placeholder, i, flag);

				if (flag & OverloadFlag::Failure || tar.convertType != ConvertResult::Exact)
					return MatchFailure(a_flag);

				//entry.funcs = conversion;
				//entry.type = input;
				//entry.index = index;

				overload.given.push_back(entry);
			}

			return overload;
		}


		void MoveSignature(SignatureBase& other)
		{
			result = std::move(other.result);
			target = std::move(other.target);
			parameters = std::move(other.parameters);
		}


		void MoveSignature(ISignature& other)
		{
			result = other.result();
			target = other.target();
			parameters = other.parameters();
		}

		constexpr Signature() noexcept = default;
		//May not need.
		Signature(SignatureBase& other) { MoveSignature(other); }
		Signature(ISignature& other) { MoveSignature(other); }

	};

}