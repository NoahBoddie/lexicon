#pragma once 

#include "AccessModifier.h"
#include "BasicQualifier.h"
#include "RuntimeQualifier.h"

namespace LEX
{
	//Include questions for qualifiers
	struct ITypePolicy;

	enum struct FieldType
	{
		Invalid,
		Local,
		Variable,
		Function,//Doesn't differentiate between method or function
	};

	
	struct Field
	{
	public:
		~Field() = default;

		//These last 2 are sorta not really important. TypePolicy is however.
		virtual FieldType GetFieldType() const = 0;// { return FieldType::Invalid; }
		
		virtual size_t GetFieldIndex() const { return max_value<size_t>; }
		
		virtual ITypePolicy* GetTypePolicy() const { return nullptr; }


		bool IsLocal() const { return GetFieldType() == FieldType::Local; }
	};

}