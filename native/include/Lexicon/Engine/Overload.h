#pragma once

#include "Lexicon/Engine/QualifiedType.h"
#include "Lexicon/Engine/OverloadEntry.h"
#include "Lexicon/Engine/OverloadFlag.h"
#include "Lexicon/ITemplateBodyPart.h"
//*src
#include "Lexicon/Engine/HierarchyData.h"
#include "Lexicon/Engine/TemplateType.h"
namespace LEX
{

	struct OverloadParameter;


	struct Overload : public ITemplateBodyPart
	{
		//Making this an ITemplateBodyPart and then using it is the ideal

		enum Bias
		{
			kPrevious = -1,
			kAmbiguous = 0,
			kCurrent = 1,


			//These double time as the result for overload matching

			kFailure = kPrevious,
			kUnsucessful = kAmbiguous,
			kSuccess = kCurrent,

		};


		//This is now a container that shows the given options for an overload.

		OverloadParameter* param = nullptr;

		QualifiedType target;


		//This should include TemplateTuple
		std::vector<std::pair<ITypeInfo*, bool>> specialImplied;



		//These align perfectly with arguments given.
		// This has to be loaded with a properly sorted default by this point.
		// Defaults have to go here because 
		std::vector<OverloadEntry> implied;

		std::map<std::string, size_t> statedEntries;



		//The purpose of states is to have it unloaded into implied I think?


		//I think I'll make it prefer the last Overload if this is not here.
		OverloadEntry* failure = nullptr;//If there's a failure this will show where it is.


		Bias bias = kCurrent;

		size_t requiredTemplate = 0;

		bool isResolved = true;



		bool IsResolved() const override
		{
			return isResolved;
		}


		ITypeInfo* GetPartArgument(size_t i) const override
		{
			return specialImplied[i].first;
		}

		TypeInfo* GetBodyArgument(size_t i) const override
		{
			if (!isResolved)
				return nullptr;

			return specialImplied[i].first->GetTypePolicy(nullptr);

		}

		size_t GetSize() const
		{
			return specialImplied.size();
		}




		//Returns if there are any missing elements unfulfilled. Templates in particular.
		bool IsValid() const
		{
			if (specialImplied.size() < requiredTemplate)
				return false;

			for (auto i = 0; i < requiredTemplate && i < specialImplied.size(); i++)
			{
				if (!specialImplied[i].first)
					return false;
			}

			return true;
		}

		ITypeInfo* GetManualTemplateType(size_t index)
		{
			if (specialImplied.size() <= index) {
				return nullptr;
			}

			if (auto entry = specialImplied[index]; entry.second)
				return entry.first;

			return nullptr;
		}


		//Type is the actual type used. Entry is the optional entry to be filled by said type.
		std::optional<bool> EmplaceTemplate(OverloadEntry& entry, ITypeInfo* type, ITypeInfo* scope, bool manual = true)
		{
			std::optional<bool> result = std::nullopt;

			if (entry.type)
			{
				//if (auto temp = entry.type->AsTemplate())
				for (auto temp : entry.type->GetTemplateInputs())
				{
					//TODO: 

					auto index = temp->index;
					//Please note, more needs to be done if it's a TupleType.

					if (specialImplied.size() <= index) {
						specialImplied.resize(index + 1);
					}

					if (auto& slot = specialImplied[index]; !slot.first) {
						if (isResolved && type->IsResolved() == false) {
							isResolved = false;
						}

						slot.first = type;
						slot.second = manual;
						//Move this outside
						if (type == temp)
							entry.type.policy = type;

						//if (result.has_value() == false);
					}
					else {
						//If something already exists, see if we can convert it to that. Then if we can, supplant that new conversion.
						// if not, remove and go next.


						//TODO:This entire bit here needs to change. 

						if (slot.second) {
							Conversion convert;
							auto result = type->IsConvertibleTo(slot.first, scope, convert);

							if (result <= ConversionEnum::Failure) {
								return false;
							}

							entry.convert = convert;
							entry.convertType = result;
						}
						else {
							logger::debug("Confliction");
							return false;
						}


						if constexpr (0)
						{
							//TODO: I think this might need conversion flags.
							Conversion convert;
							auto result = type->IsConvertibleTo(slot.first, scope, convert);

							if (result <= ConversionEnum::Failure) {
								return false;
							}

							entry.convert = convert;
							entry.convertType = result;
						}
					}

					//if (result.has_value() == false);
					result = true;

				}
			}
			return result;
		}

		bool EmplaceEntry(OverloadEntry& entry, ITypeInfo* type, ITypeInfo* scope, std::string_view name = {})
		{
			//EmplaceEntry needs to adapt the type to what's loaded.

			//I believe this can handle the entire bit itself.

			auto result = EmplaceTemplate(entry, type, scope, false).value_or(true);


			auto index = entry.index;


			if (index == -1) {
				target = entry.type;
			}
			else {
				if (implied.size() <= index) {
					implied.resize(index + 1);
				}
				auto& slot = implied[index];

				if (slot.type) {
					return false;
				}

				slot = entry;
			}


			if (name.empty() == false) {
				auto& slot = statedEntries[std::string{ name }];

				bool filled = slot;

				slot = index;

				//This bit doesn't seem needed
				if (filled) {
					return false;
				}
			}


			//This function needs to be able to handle resolutions between param generic types. So like "params Type[]" should handle
			// each new type, SOMEHOW. That may be a section all on it's own I believe, just about anything that's multi input. I believe this
			// will only apply to to the call args

			//CORRECTION... I realize now that what I'm talking about is like making an initializer list that can be understood without context.
			// So, instead the only thing that can be done when some initialization happens like that is to resolve it. Perhaps I'll make something
			// that does it for me, and assumes based on what types can convert into the others. For now, it's not a worry.



			//Work needs to be done if it's a grouped type

			return result;
		}


		static int CompareType(OverloadCode& left, OverloadCode& right, QualifiedType& left_type, QualifiedType& right_type)
		{
			//This should be using OverloadCode, but i'd need a given type to do it.

			if (!left.initialized && !right.initialized)
			{
				left = left.FinalizeOld(left_type->GetHierarchyData(), right_type->GetHierarchyData());
				right = right.FinalizeOld(right_type->GetHierarchyData(), left_type->GetHierarchyData());

				//OverloadEntry left = a_lhs.FinalizeOld(a_rhs.type);
				//OverloadEntry right = a_rhs.FinalizeOld(a_lhs.type);
			}

			logger::info("left({}/{}), right({}/{})", left.hash[0], left.hash[1], right.hash[0], right.hash[1]);


			if (left.hash[0] <= right.hash[1] && left.hash[1] >= right.hash[0]) {
				return left.distance - right.distance;
			}
			if (left.hash[0] >= right.hash[1] && left.hash[1] <= right.hash[0]) {
				return left.distance - right.distance;
			}

			//We do reference comparison here

			return 0;
		}

		static int CompareType(QualifiedType& a_this, QualifiedType& a_lhs, QualifiedType& a_rhs)
		{
			OverloadCode left = a_this.policy->GetHierarchyData()->CreateCode(a_lhs);
			OverloadCode right = a_this.policy->GetHierarchyData()->CreateCode(a_rhs);

			return CompareType(left, right, a_lhs, a_rhs);
		}



		int SafeCompare(const std::string& name, OverloadEntry& entry, QualifiedType& arg)
		{
			if (!this) {
				return -1;
			}

			auto it = statedEntries.find(name);

			if (statedEntries.end() == it) {
				return -1;
			}

			//auto& tar = it->second;
			auto& other = implied[it->second];

			return _ConvertComp(other, entry, arg);

		}



		//TODO: the safe compares of Overload are not yet completely resolved. The main issue being it doesn't account for conversions.
		// to expand on the above, detecting a conversion is grounds for prefering one over the other, then it depends which conversion has the value closer to zero.
		// conversions don't really happen right now so it's a big ol fuck it. But this WILL be important.

		static int _ConvertComp(OverloadEntry& tar, OverloadEntry& entry, QualifiedType& arg)
		{
			//Arg is unused

		//These should remove const conversions and stuff like that
		//

		//I want temp match to lose to everything but a conversion.

			bool prev_pure = tar.convertType >= ConversionEnum::Exact && tar.convertType < ConversionEnum::Transformative;
			bool curr_pure = entry.convertType >= ConversionEnum::Exact && entry.convertType < ConversionEnum::Transformative;


			if (!prev_pure && curr_pure)
				return -1;
			else if (prev_pure && !curr_pure)
				return 1;
			else if (!prev_pure && !curr_pure)
				//return arg.CompareType(tar.code, entry.code, tar.type, entry.type);
				return CompareType(tar.code, entry.code, tar.type, entry.type);
			else
				return (entry.convertType <=> tar.convertType)._Value;
		}

		int SafeCompare(QualifiedType& other, QualifiedType& arg)
		{
			//This version is for types.


			if (!this)
				return -1;

			if (target && !other)
				return 1;
			else if (!target && other)
				return -1;
			else if (!target && !other)//Is convert qualified is not desired here.
				return 0;

			//No conversions exist, so direct is good.

			//return arg.CompareType(target, other);
			return CompareType(arg, target, other);

			return 0;

			//Qualified conversions have already played out by this point.
			//if (target.IsConvertToQualified(other, scope) > ConversionEnum::Failure)
			//	return -1;
			//else if (other.IsConvertToQualified(target, scope) > ConversionEnum::Failure)
			//	return 1;

			return 0;
		}


		int SafeCompare(OverloadEntry& entry, QualifiedType& arg)
		{
			//TODO:Right here, safe compare should be viewing this through the qualified type through the lens of what the overload expects
			// So it should

			//The idex can just use the entry btw

			auto i = entry.index;

			if (!this)
				return -1;

			if (i >= implied.size())
				return -1;

			auto& tar = implied[i];

			//I don't think I should be assuming a lack of type. here that's an explicit error
			if (tar.type && !entry.type)
				return 1;
			else if (!tar.type && entry.type)
				return -1;
			else if (!tar.type && !entry.type)
				return 0;

			return _ConvertComp(tar, entry, arg);


			//return arg.CompareType(tar.code, entry.code, tar.type, entry.type);
			return CompareType(tar.code, entry.code, tar.type, entry.type);

			return 0;

			//Currently there is no actually way to compare. it is or it isn't Implement properly in the merger.

			//auto left = tar.type.IsConvertToQualified(entry.type, arg);
			//auto right = entry.type.IsConvertToQualified(tar.type, arg);

			//if (left == true)
			//	return -1;
			//else if (right == true)
			//	return 1;

			return 0;
		}
	};



	using OverloadBias = Overload::Bias;


}
