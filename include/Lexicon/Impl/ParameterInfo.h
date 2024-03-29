#pragma once


#include "LocalInfo.h"



namespace LEX
{

	class ParameterInfo : public LocalInfo
	{
		struct Settings : public LocalInfo::Settings
		{
			bool isDefault = false;
		};


	public:
		ParameterInfo(ITypePolicy* t, size_t i, std::string n) : LocalInfo{t, i}
		{
			name = n;
		}

		ParameterInfo(QualifiedType t, size_t i, std::string n) : LocalInfo{ t, i }
		{
			name = n;

			DataAs<Settings>().flags = t.flags;
		}
	};
}
