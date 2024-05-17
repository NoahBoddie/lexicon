#pragma once

#include "MemberInfo.h"
#include "FunctionBase.h"
#include "MemberPointer.h"

namespace LEX
{

	struct FunctionInfo : public MemberInfo//Needs to be able to use overloads, so these can possibly be a bit bigger.
	{
		using FunctionType = FunctionBase;//


		union {
			std::array<size_t, 2> _raw{ 0 , 0 };


			FunctionType* function;

			struct
			{
				MemberPointer method;//prefered, works with the other.
				FunctionData* signature;
			};
		};

		bool IsVirtual() const
		{
			return specifiers & Specifier::Virtual;
		}

		FunctionType* Get() const
		{
			if (IsVirtual() == false)
				return function;

			return nullptr;
		}

		FieldType GetFieldType() const override { return FieldType::Function; }

		//For now, this is true, there is no way to handle a function's type.
		ITypePolicy* GetType() const override { return nullptr; }


		Qualifier GetQualifiers() const override { return Qualifier::Const; }	//Functions do not have qualifiers.
		Specifier GetSpecifiers() const override { return specifiers; }

		std::string GetFieldName() const override { return Get()->GetName(); }

		operator bool() const override
		{
			return _raw[0] || _raw[1];
		}


	};

}
