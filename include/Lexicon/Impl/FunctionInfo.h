#pragma once

#include "MemberInfo.h"
#include "FunctionBase.h"
#include "MemberPointer.h"

namespace LEX
{

	struct FunctionInfo : public MemberInfo
	{

		struct Settings : public MemberInfo::Settings
		{
			bool isVirtual = false;
		};

		using FunctionType = FunctionBase;//


		union {
			FunctionType* function = nullptr;
			MemberPointer method;//prefered, works with the other.
		};

		bool IsVirtual() const
		{
			return GetData<Settings>().isVirtual;
		}

		FunctionType* Get()
		{
			if (IsVirtual() == false)
				return function;

			return nullptr;
		}

		FunctionInfo() = default;
	};

}
