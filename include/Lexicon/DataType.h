#pragma once

namespace LEX
{

	enum struct DataType : uint8_t
	{
		Invalid,
		//Typename,//Reserved for template types, and forward declared
		Class,
		Struct,
		Interface,
		//Enum,
		//Concept
	};


	inline bool IsDataTypeRef(DataType type)
	{
		switch (type)
		{
		case DataType::Class:
		case DataType::Interface:
			return true;

		default:
			false;
		}
	}

	inline bool IsDataTypeVal(DataType type)
	{
		switch (type)
		{
		default:
			return !IsDataTypeRef(type);

		case DataType::Invalid:
			false;
		}
	}
}