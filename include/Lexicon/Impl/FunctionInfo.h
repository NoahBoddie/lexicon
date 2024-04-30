#pragma once

#include "MemberInfo.h"
#include "FunctionBase.h"
#include "MemberPointer.h"

namespace LEX
{

	struct FunctionInfo : public MemberInfo
	{

		//TODO: This needs to be able to handle overloads some how, via abstract functions maybe?


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

		FieldType GetFieldType() const override { return FieldType::Function; }


		FunctionInfo() = default;
	};

}
