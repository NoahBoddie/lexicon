#pragma once

#include "Overload.h"
#include "OverloadKey.h"
#include "OverloadFlag.h"
#include "OverloadClause.h"

#include "Lexicon/SignatureBase.h"

namespace LEX
{

	struct Signature : public OverloadArgument, public SignatureBase
	{
		//The match for this should be aimed to be as very little in terms of ambiguity as possible.
		//Thus, the stated project, script, and of course, the path.
		//The path for generics will probably simply be controlled by the amount of generic parameters it has, rather than the entires.
		// So no generic, no generic notation. Is generic, has generic notation. If it's the only one you can




		OverloadBias Match(OverloadParameter*clause, ITypeInfo* scope, Overload& out, Overload* prev) override
		{

			OverloadFlag flag = OverloadFlag::AllAccess;

			if (clause->CanMatch(result, parameters.size(), 0, flag) == false) {
				return OverloadBias::kFailure;
			}

			{
				OverloadEntry tar;
				
				if (clause->MatchImpliedEntry(tar, target, nullptr, out, -1, -1, flag) == false ||  tar.convertType != ConversionEnum::Exact)
					return OverloadBias::kFailure;

				if (out.EmplaceEntry(tar, target, nullptr) == false)
					return OverloadBias::kFailure;

				out.param = clause;
			}


			constexpr auto offset_placeholder = 0;

			int winner = 0;

			for (int i = 0; i < parameters.size(); i++) {
				QualifiedType input = parameters[i];
				OverloadEntry entry;
				if (clause->MatchImpliedEntry(entry, input, nullptr, out, i, offset_placeholder, flag) == false || entry.convertType != ConversionEnum::Exact)
					return OverloadBias::kFailure;
				
				
				if (out.EmplaceEntry(entry, target, nullptr) == false)
					return OverloadBias::kFailure;
					

			}

			return Overload::kSuccess;
		}

		OverloadBias Match(OverloadParameter* clause)
		{
			Overload out;
			return Match(clause, nullptr, out, nullptr);
		}


		void MoveSignature(const SignatureBase& other)
		{
			result = std::move(other.result);
			target = std::move(other.target);
			parameters = std::move(other.parameters);
		}


		void MoveSignature(const ISignature& other)
		{
			result = other.result();
			target = other.target();
			parameters = other.parameters();
		}



		constexpr Signature() noexcept = default;
		//May not need.
		Signature(const SignatureBase& other) { MoveSignature(other); }
		Signature(const ISignature& other) { MoveSignature(other); }

	};

}