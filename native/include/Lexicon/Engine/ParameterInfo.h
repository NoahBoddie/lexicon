#pragma once


#include "LocalInfo.h"



namespace LEX
{
	ENUM (ParameterFlag, uint8_t)
	{
		None,
		Default = 1 << 0,
		Params = 1 << 1,
	};

	class ParameterInfo : public LocalInfo
	{
	public:
		


		ParameterInfo(QualifiedType t, std::string n, uint32_t i) : LocalInfo{ t, i }, _name { n }
		{
		}

		//ParameterInfo(ITypePolicy* t, size_t i, std::string n) : LocalInfo{ t, i }
		//{
		//	name = n;
		//}
		//ParameterInfo(QualifiedType t, size_t i, std::string n) : LocalInfo{ t, i }
		//{
		//	name = n;
		//	DataAs<Settings>().flags = t.flags;
		//}


		std::string GetFieldName() const override { return _name; }

		FieldType GetFieldType() const override { return FieldType::Parameter; }

		//Should contain information like default info etc.

		bool IsOptional() const
		{
			return flags & ParameterFlag::Default || flags & ParameterFlag::Params;
		}

	protected:
		std::string _name;
		ParameterFlag flags{};
		RoutineBase* defFunc = nullptr;
	};

}
