#pragma once

#include "Field.h"

//Need to change name now.

namespace LEX
{
	struct TypePolicy;

	enum struct InfoType
	{
		Variable,
		Parameter,
		Field,
	};

	//This is something I don't know how or if I want to do yet. At least, private protected and public.
	enum struct AccessModifier
	{
		Private,	//Only a given scope can observe it
		Protected,	//Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
		Internal,	//Only the script can observe it.
		ProtectedInternal,//extends internal to derived scripts. In a class functions much like internal.
		Public


		//Protected		Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
		//				script block usage.
	};

	struct InfoBase : public Field
	{

	public:
		
		TypePolicy* type;//I would actually like to use TypeID or something. ITypePolicy
		
		//Mainly used for members and function variables. But since I've yet to see anything that remotely 
		// outside these 2 uses, I think this suits just fine.
		uint32_t index = max_value<uint32_t>;
		
		size_t GetFieldIndex() const override
		{
			return index;
		}
	};

	struct NamedInfo : public InfoBase
	{
		std::string name;

		//This version is never used I don't think


	};

	

	struct ParamInfo : public NamedInfo
	{
		//Similar to VarInfo, but has areas for qualifers specific to parameters.

		bool hasDefault = false;


		FieldType GetFieldType() const override
		{
			return FieldType::Param;
		}

	};

	struct MemberInfo : public NamedInfo
	{
		//like param info but holds a place where a subroutine can go.
		bool isStatic = false;//Not exploring right now.
		AccessModifier access = AccessModifier::Public;


		FieldType GetFieldType() const override
		{
			return FieldType::Member;
		}

	};


	enum struct QualifierFlag
	{
		None = 0,
		Const = 1 << 0,

	};


	struct VariableInfo : public InfoBase
	{

		QualifierFlag qualifiers{};
		FieldType GetFieldType() const override
		{
			return FieldType::Param;
		}

	};

}