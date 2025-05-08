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
	struct ITypeInfo;
	struct TargetObject;


	class OverloadInput : public OverloadArgument
	{
	public:


		OverloadBias Match(OverloadParameter* param, ITypeInfo* scope, Overload& out, Overload* prev) override
		{
			OverloadFlag flags{};

			//...

			if (specialStated.empty() == false) {
				flags |= OverloadFlag::StatesTemplate;
			}
			if (stated.empty() == false) {
				flags |= OverloadFlag::StatesArgument;
			}

			if (param->CanMatch(nullptr, implied.size(), specialImplied.size(), flags) == false) {
				logger::info("pre-eval fail");
				return OverloadBias::kFailure;
			}

			param->QualifyOverload(out);

#pragma region TemplateParts
			flags |= OverloadFlag::IsTemplate;

			//IMPORTANT TO REMEMBER PART, any use of a template type is a loss to a real type.

			for (int i = 0; i < specialImplied.size(); i++)
			{
				auto& pair = specialImplied[i];

				QualifiedType type{ pair.first };
				auto offset = pair.second;


				OverloadEntry entry;

				if (param->MatchImpliedEntry(entry, type, scope, out, i, offset, flags) == false) {
					//Needs to send the given issue object
					return OverloadBias::kFailure;
				}



				if (out.EmplaceTemplate(entry, type, scope) == false) {
					return OverloadBias::kFailure;
				}
			}


#pragma endregion

			flags &= ~OverloadFlag::Clearable;

#pragma region CallParts

			QualifiedType this_type;

			if (object->IsValid() == true)
			{

				if (object->IsImplicit() == true)
					flags |= OverloadFlag::TargetOpt;

				this_type = *object->target;

			}

			OverloadEntry tar;
			tar.index = -1;

			if (param->MatchImpliedEntry(tar, this_type, scope, out, -1, -1, flags) == false) {
				return OverloadBias::kFailure;
			}

			//out.target = tar.entry.type;
			if (tar.type && out.EmplaceEntry(tar, this_type, scope) == false) {
				return OverloadBias::kFailure;
			}

			out.target = tar.type;
			out.param = param;


			int winner = 0;


			for (int i = 0; i < implied.size(); i++)
			{
				auto& pair = implied[i];

				QualifiedType type = pair.first;
				auto offset = pair.second;


				OverloadEntry entry;

				if (param->MatchImpliedEntry(entry, type, scope, out, i, offset, flags) == false) {
					//Needs to send the given issue object
					return OverloadBias::kFailure;
				}

				//entry.funcs = conversion;
				//entry.type = input;
				//entry.index = index;

				//Compare should this input-> prev vs entry
				auto new_winner = prev->SafeCompare(entry, type);

				if (!winner)
					winner = new_winner;

				else if (winner < 0 && new_winner > 1 || winner > 0 && new_winner < 1) {// shit isn't valid anymore.
					return OverloadBias::kAmbiguous;
				}

				//out.implied.push_back(temp.entry);

				if (out.EmplaceEntry(entry, type, scope) == false) {
					return OverloadBias::kFailure;
				}
			}


			for (auto& [name, sol] : stated)
			{
				OverloadEntry entry;
				//Some new flags should be used here I believe.
				if (param->MatchStatedEntry(entry, sol, scope, out, name, flags) == false) {
					//Needs to send the given issue object
					return OverloadBias::kFailure;
				}

				auto new_winner = prev->SafeCompare(name, entry, sol);

				if (!winner)
					winner = new_winner;

				else if (winner < 0 && new_winner > 1 || winner > 0 && new_winner < 1) {// shit isn't valid anymore.
					return OverloadBias::kAmbiguous;
				}

				if (out.EmplaceEntry(entry, sol, scope, name) == false) {
					return OverloadBias::kFailure;
				}
			}

#pragma endregion

			if (out.IsValid() == false) {
				return OverloadBias::kFailure;
			}


			if (!winner) {
				winner = prev->SafeCompare(tar, this_type);
			}


			if (winner > 0) {
				logger::trace("right winner");
				//This is just failure tbh, doing so means I don't need to use move or set the pointer.
				return OverloadBias::kPrevious;
			}
			else if (winner < 0) {
				logger::trace("left winner");
				return OverloadBias::kCurrent;
			}
			else {
				logger::trace("no winner {}", prev != nullptr);
				return OverloadBias::kAmbiguous;
			}



		}


		//Note to self, maintain using using Solutions. Later passed friendship is a thing that I use.
		// Additionally, if I go that way I want to find a way that inputting types in generic spaces that private inheritance from
		// telling it no when temporary friendship has been given.

		TargetObject* object = nullptr;

		//I'd like to order all these vectors in a much more friendly way.
		std::vector<std::pair<ITypeInfo*, size_t>>			specialImplied;
		std::unordered_map<std::string, std::vector<ITypeInfo*>> specialStated;


		std::vector<std::pair<Solution, size_t>> implied;
		std::map<std::string, Solution> stated;//The stated can possibly declare an array/tuple. For this


	};


}