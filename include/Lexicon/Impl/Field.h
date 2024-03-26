#pragma once 

#include "AccessModifier.h"
//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"

#include "Qualifier.h"

namespace LEX
{
	//Include questions for qualifiers
	struct ITypePolicy;
	struct Solution;

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

		//TODO: Make Field::AsSolution virtual
		Solution AsSolution();


		virtual Qualifier GetQualifiers() const = 0;


		bool IsLocal() const { return GetFieldType() == FieldType::Local; }
	};

}