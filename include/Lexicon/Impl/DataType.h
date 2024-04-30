#pragma once

namespace LEX
{

	enum struct DataType : uint8_t
	{
		Invalid,
		Class,
		Struct,
		Interface,
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