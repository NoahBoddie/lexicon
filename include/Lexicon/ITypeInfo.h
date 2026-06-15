#pragma once

#include "Lexicon/ISpecial.h"
#include "Lexicon/TypeID.h"


#include "Lexicon/DataType.h"

#include "Lexicon/Interfaces/IEnvironment.h"

namespace LEX
{

	struct TypeID;
	class Variable;
	struct ICallableUnit;//No longer needed
	struct IFunction;
	class RuntimeVariable;
	struct TypeBase;

	enum struct DataType : uint8_t;

	struct OverloadCode;


	struct GenericBase;
	struct ITemplatePart;
	struct ITemplateBody;

	struct ITypeInfo;
	struct ObjectPolicy;
	

	enum struct ConvertType
	{
		Type,
		Implicit,
		Explicit,
	};

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION_DERIVES(ITypeInfo, ISpecial)
			{
			public:

				virtual TypeInfo* GetTypeInfo(ITemplateBody* args) = 0;


				virtual TypeID GetTypeID() const = 0;

				virtual InstanceID GetInstanceID() const = 0;

				//Should this be higher?
				virtual DataType GetDataType() const = 0;


				//*
				virtual bool IsReferenceType() const = 0;

				virtual bool IsScriptObject() const = 0;

				virtual bool IsAbstract() const noexcept = 0;

				virtual size_t GetFieldCount() const = 0;
				//*/

				
				virtual bool CanConvert(const ITypeInfo* other, ConvertType convert) const = 0;


				virtual std::span<ITypeInfo*> GetTemplate() = 0;

				virtual ObjectPolicy* GetObjectPolicy() const = 0;

				virtual ITypeInfo* CheckTypeInfo(ITemplatePart* args) = 0;
			};
		}

		CURRENT_VERSION(ITypeInfo, 1);
	}


	struct __declspec(novtable) IMPL_VERSION_DERIVES(ITypeInfoAbstract, ITypeInfo, IEnvironment)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::ITypeInfo);

		std::span<ITypeInfo*> GetTemplate() override { return {}; }

		bool IsValueType() const
		{
			return !IsReferenceType();
		}

		bool IsScriptObject() const override
		{
			return false;
		}

		bool IsAbstract() const noexcept override
		{
			return true;
		}


		bool IsAttribute() const
		{
			return GetDataType() == DataType::Attribute;
		}




		
		bool IsDerivedFrom(const ITypeInfo* other) const
		{
			return CanConvert(other, ConvertType::Type);
		}
	

		bool CanConvertTo(const ITypeInfo* other) const
		{
			return CanConvert(other, ConvertType::Implicit);
		}

		bool CanCastTo(const ITypeInfo* other) const
		{
			return CanConvert(other, ConvertType::Explicit);
		}
	};


#ifndef LEX_SOURCE
	//Only accessible outside of the source.
	struct ITypeInfo : public ITypeInfoAbstract {};
#endif

	using IType = ITypeInfo;

}

#ifdef LEX_SOURCE
#include "Lexicon/Engine/ITypeInfoImpl.h"
#endif