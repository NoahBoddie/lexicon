#pragma once 

#include "AccessModifier.h"
//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"

#include "Qualifier.h"
#include "DeclareSpecifier.h"
#include "QualifiedType.h"

namespace LEX
{
	//Include questions for qualifiers
	struct ITypePolicy;
	struct Solution;
	struct QualifiedType;


	enum struct FieldType
	{
		Invalid,
		Local,
		Parameter,
		Variable,
		Function,//Doesn't differentiate between method or function
	};

	


	struct Field
	{
		//The interface for fields
		virtual ~Field() = default;

		virtual String GetFieldName() const = 0;

		virtual FieldType GetFieldType() const = 0;

		virtual uint32_t GetFieldIndex() const = 0;


		virtual Qualifier GetQualifiers() const = 0;

		virtual Specifier GetSpecifiers() const = 0;

		virtual ITypePolicy* GetType() const = 0;

		//TODO: Make Field::AsSolution virtual
		Solution AsSolution();

		QualifiedType GetQualifiedType() const;
		QualifiedType FetchQualifiedType() const;
		
		bool IsLocal() const { return GetFieldType() == FieldType::Local; }
	};
}