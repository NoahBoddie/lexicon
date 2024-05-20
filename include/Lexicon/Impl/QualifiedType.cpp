#pragma once

#include "QualifiedType.h"

#include "OverloadEntry.h"

namespace LEX
{
	int QualifiedType::CompareType(OverloadCode& left, OverloadCode& right, QualifiedType& left_type, QualifiedType& right_type)
	{
		//This should be using OverloadCode, but i'd need a given type to do it.

		if (!left.initialized && !right.initialized)
		{
			left = left.FinalizeOld(left_type, right_type);
			right = right.FinalizeOld(right_type, left_type);

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

		return 0;
	}

	int QualifiedType::CompareType(QualifiedType& a_lhs, QualifiedType& a_rhs)
	{
		OverloadCode left = policy->CreateCode(a_lhs);
		OverloadCode right = policy->CreateCode(a_rhs);

		return CompareType(left, right, a_lhs, a_rhs);
	}
}