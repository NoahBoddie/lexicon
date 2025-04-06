#pragma once 


#include "Lexicon/Qualifier.h"
#include "Lexicon/Specifier.h"
#include "Lexicon/Engine/QualifiedType.h"

namespace LEX
{
	//Include questions for qualifiers
	struct QualifiedType;


	enum struct FieldType
	{
		Invalid,
		Local,
		Parameter,
		Variable,
		Member,
		Function,//Doesn't differentiate between method or function
	};

	

	//TODO: Field does not need to be viewable
	struct Field
	{
		//The interface for fields
		virtual ~Field() = default;
		
		//Field sorta needs to remain a string because of the fact locals don't really have names. Despite this, it's a problem between plugins
		// that string has different sizes. This needs to be solved.
		//I think the info will have a name, but the field name will be basic
		virtual std::string GetFieldName() const = 0;

		virtual FieldType GetFieldType() const = 0;

		virtual uint32_t GetFieldIndex() const = 0;


		virtual Qualifier GetQualifiers() const = 0;

		virtual Specifier GetSpecifiers() const = 0;

		virtual BasicType* GetType() const = 0;

		//TODO: Make Field::AsSolution virtual
		

		QualifiedType GetQualifiedType() const;
		QualifiedType FetchQualifiedType() const;
		
		bool IsLocal() const { return GetFieldType() == FieldType::Local; }
	};
}