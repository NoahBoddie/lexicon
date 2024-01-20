#pragma once

#include "Field.h"

//Need to change name now.

//else
#include "MemberPointer.h"

namespace LEX
{
	struct ITypePolicy;
	struct IFunction;//deprecated here?
	struct FunctionBase;


	enum struct InfoType
	{
		Variable,
		Parameter,
		Field,
	};

	//This is something I don't know how or if I want to do yet. At least, private protected and public.
	enum struct AccessModifier
	{//The base struct that handles this might be later named "ClassicInfos". Maybe. Will decide if it sounds dumb.

		Private,	//Only a given scope can observe it
		Protected,	//Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
		Internal,	//Only the script can observe it.
		ProtectedInternal,//extends internal to derived scripts. In a class functions much like internal.
		Public


		//Protected		Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
		//				script block usage.
	};


	enum struct QualifierFlag
	{
		None = 0,
		Const = 1 << 0,
		Refr = 1 << 1,
	};

	//I need this because the above qualifier flags
	enum struct QualifierFlag2
	{
		None = 0,
		Const = 1 << 0,
	};


	struct InfoBase : public Field
	{
		//I actually believe this should probably not exist. Field is enough of a pivot
	public:
		InfoBase() = default;
		//no qualifiers for now.
		InfoBase(ITypePolicy* t, size_t i) : type { t }, index { i }
		{

		}

		//Move this.
		ITypePolicy* type = nullptr;//I would actually like to use TypeID or something. ITypePolicy
		
		//Mainly used for members and function variables. But since I've yet to see anything that remotely 
		// outside these 2 uses, I think this suits just fine.
		size_t index = max_value<size_t>;
		
		size_t GetFieldIndex() const override
		{
			return index;
		}


		virtual operator bool() const
		{
			//Should this be and?
			return index != max_value<size_t> || type;
		}
	};

	struct NamedInfo : public InfoBase
	{
		std::string name;

		//This version is never used I don't think
		
		//So the concept of this is it should include an inlinable formula that exists to be able to include
		// into wherever it's used.
		//Formula base;

		std::string GetName()
		{
			return name;
		}
		NamedInfo() = default;

		NamedInfo(ITypePolicy* t, size_t i, std::string n) : InfoBase{ t, i }, name { n }
		{

		}
	};

	

	struct ParamInfo : public NamedInfo
	{
		//Similar to VarInfo, but has areas for qualifers specific to parameters.

		bool hasDefault = false;//Make a formula and represent by using a unique_ptr

		QualifierFlag qualifiers{};

		FieldType GetFieldType() const override
		{
			return FieldType::Param;
		}

		ParamInfo(ITypePolicy* t, size_t i, std::string n) : NamedInfo{ t, i, n }
		{

		}
	};

	struct ClassUnitInfo : public NamedInfo
	{
		//The pivot between members and methods.


		//like param info but holds a place where a subroutine can go.
		bool isStatic = false;//Not exploring right now.
		AccessModifier access = AccessModifier::Public;


		FieldType GetFieldType() const override
		{
			return FieldType::Member;
		}


		ClassUnitInfo() = default;

	};

	
	struct FunctionInfo : public ClassUnitInfo
	{
		using FunctionType = FunctionBase;//

		bool isVirtual = false;//I wish to move this into a flag below me or something.
		union{
			FunctionType* function = nullptr;
			MemberPointer method;//prefered, works with the other.
		};

		FunctionType* Get()
		{
			if (!isVirtual)
				return function;

			return nullptr;
		}

		FunctionInfo() = default;
	};

	struct VariableInfo : public InfoBase
	{
		VariableInfo() = default;

		//no qualifiers for now.
		VariableInfo(ITypePolicy* t, size_t i) : InfoBase {t, i}
		{

		}

		QualifierFlag qualifiers{};
		FieldType GetFieldType() const override
		{
			return FieldType::Param;
		}

	};

}