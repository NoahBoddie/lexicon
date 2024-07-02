#pragma once

#include "QualifiedType.h"
#include "OverloadEntry.h"

namespace LEX
{

	struct OverloadClause;

	enum struct OverloadPreference
	{
		Current		= -1,
		Ambiguous	=  0,
		Previous	=  1,

	};

	struct Overload
	{
		//This is now a container that shows the given options for an overload.

		OverloadClause* clause;

		QualifiedType target;//

		//These align perfectly with arguments given.
		// This has to be loaded with a properly sorted default by this point.
		// Defaults have to go here because 
		std::vector<OverloadEntry> given;


		std::unordered_map<std::string, OverloadEntry> defaults;

		


		//TODO: the safe compares of Overload are not yet completely resolved. The main issue being it doesn't account for conversions.
		// to expand on the above, detecting a conversion is grounds for prefering one over the other, then it depends which conversion has the value closer to zero.
		// conversions don't really happen right now so it's a big ol fuck it. But this WILL be important.

		static int _ConvertComp(OverloadEntry& tar, OverloadEntry& entry, QualifiedType& arg)
		{
			//These should remove const conversions and stuff like that
			//
			bool prev_pure = tar.convertType < ConvertResult::Transformative;
			bool curr_pure = entry.convertType < ConvertResult::Transformative;

			if (!prev_pure && curr_pure)
				return -1;
			else if (prev_pure && !curr_pure)
				return 1;
			else if (!prev_pure && !curr_pure)
				return arg.CompareType(tar.code, entry.code, tar.type, entry.type);
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

			return arg.CompareType(target, other);

			return 0;

			//Qualified conversions have already played out by this point.
			//if (target.IsConvertToQualified(other, scope) > ConvertResult::Failure)
			//	return -1;
			//else if (other.IsConvertToQualified(target, scope) > ConvertResult::Failure)
			//	return 1;

			return 0;
		}


		int SafeCompare(std::string name, OverloadEntry& entry, QualifiedType& arg)
		{
			if (!this) {
				return -1;
			}

			auto it = defaults.find(name);

			if (defaults.end() == it) {
				return -1;
			}

			//auto& tar = it->second;

			
			return _ConvertComp(it->second, entry, arg);
		
		}

		int SafeCompare(size_t i, OverloadEntry& entry, QualifiedType& arg)
		{
			if (!this)
				return -1;

			if (i >= given.size())
				return -1;

			auto& tar = given[i];

			//I don't think I should be assuming a lack of type. here that's an explicit error
			if (tar.type && !entry.type)
				return 1;
			else if (!tar.type && entry.type)
				return -1;
			else if (!tar.type && !entry.type)
				return 0;

			return _ConvertComp(tar, entry, arg);


			return arg.CompareType(tar.code, entry.code, tar.type, entry.type);

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

}
