#pragma once


#include "MemberInfo.h"
#include "Lexicon/MemberPointer.h"
//*src
#include "Lexicon/Global.h"
#include "GlobalData.h"
#include "GlobalVariable.h"

namespace LEX
{
	struct VariableInfo : public MemberInfo
	{
		//no qualifiers for now.
		//VariableInfo(ITypeInfo* t, size_t i, Qualifier q) : _index{ i }, _type{ t }
		//{
		//	DataAs<Settings>().flags = q;
		//}


		FieldType GetFieldType() const override { return IsStatic() ? FieldType::Global : FieldType::Member; }

		ITypeInfo* GetType() const override { return nullptr; }

		Qualifier GetQualifiers() const override { return qualifiers; }
		Specifier GetSpecifiers() const override { return specifiers; }

		std::string GetFieldName() const override { return std::string{ Get()->GetName() }; }


		union {
			std::array<size_t, 2> _raw{ 0 , 0 };

			//This would be a GlobalBase, rather than a global.
			Global* variable;

			struct
			{
				//These is used for member variables, since they don't exist until instantiated.
				MemberPointer method;//prefered, works with the other.
				GlobalData* signature;
			};
		};

		Global* Get() const
		{
			if (IsStatic() == true)
				return variable;

			return nullptr;
		}
	};



}
