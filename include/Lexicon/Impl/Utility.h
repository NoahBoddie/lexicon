#pragma once

namespace LEX
{
	struct TypeInfo;
	struct ITypeInfo;

	struct Utility
	{
		static TypeInfo* ResolveTypeInfo(ITypeInfo* type);
	};
}