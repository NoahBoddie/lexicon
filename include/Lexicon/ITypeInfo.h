#pragma once

#include "Lexicon/ISpecial.h"
#include "Lexicon/TypeID.h"


#include "Lexicon/DataType.h"


#include "Lexicon/Reflection.h"

#include "Lexicon/Interfaces/IEnvironment.h"

namespace LEX
{

	struct TypeID;
	class Variable;
	struct ICallableUnit;//No longer needed
	struct IFunction;
	class RuntimeVariable;
	struct TypeBase;
	struct HierarchyData;

	enum struct DataType : uint8_t;

	struct InheritData;
	struct OverloadCode;


	struct GenericBase;
	struct ITemplatePart;
	struct ITemplateBody;

	struct ITypeInfo;
	struct ObjectPolicy;
	

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION_DERIVES(ITypeInfo, ISpecial)
			{
			public:
				constexpr static uint32_t NonGenericIndex = -1;




				//ITypeInfo();

				//ITypeInfo(uint32_t i);

				//ITypeInfo(std::string_view name, TypeOffset offset);


				//ITypeInfo(ISpecializable* tar) : _target{ tar } {}

				//An interface of a type policy, which may or may not be a policy. Generally, one should use all the functions that this provides in order
				// to use the type policy.


				//Unsure of how this gets one however.


				//Might require an ISpecializable target. Actually, move this shit.
				//ISpecializable* _target = nullptr;

				//This function basically expects a number of things to be submitted, but here, the full specialization of a calling function could be used in order to
				// get a partially specialized function.
				//I may still go with interface because I don't think I want something where I derive a specializaton from TypePolicy, it'd be a waste of space, literal repeat.
				// I KNOW, instead, I could make an implementation policy
				//TODO:Make second interface for "Type" which GetType returns, and what stores all the type policy info.
				virtual TypeInfo* GetTypePolicy(ITemplateBody* args) = 0;

				//This should be hidden.
				virtual ITypeInfo* CheckTypePolicy(ITemplatePart* args) = 0;


				virtual TypeID GetTypeID() const = 0;

				//Should this be higher?
				virtual DataType GetDataType() const = 0;


				//*
				virtual bool IsReferenceType() const = 0;

				//*/

				

				virtual bool CanConvert(const ITypeInfo* other) const = 0;

				virtual bool Convert(const Variable& from, Variable& to) const = 0;

				//Make a type that produces an error
				//virtual bool Convert(const Variable& from, Variable& to, const ITypeInfo* type) = 0;


				//Make some safe functions for these.


			public:
				
				virtual std::span<ITypeInfo*> GetTemplate() { return {}; }



				TypeInfo* FetchTypePolicy(ITemplateBody* args)
				{
					return this ? GetTypePolicy(args) : nullptr;
				}

				DataType FetchDataType() const
				{
					return this ? GetDataType() : DataType::Invalid;
				}

				bool IsValueType() const
				{
					return !IsReferenceType();
				}

				TypeID FetchTypeID() const
				{
					return this ? GetTypeID() : TypeID{};
				}
			};
		}

		namespace _2
		{
			struct INTERFACE_VERSION(ITypeInfo)
			{
				virtual ObjectPolicy* GetObjectPolicy() const = 0;
			};
		}

		CURRENT_VERSION(ITypeInfo, 2);
	}


	struct __declspec(novtable) IMPL_VERSION_DERIVES(ITypeInfoAbstract, ITypeInfo, IEnvironment)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::ITypeInfo)
	};


#ifndef LEX_SOURCE
	//Only accessible outside of the source.
	struct ITypeInfo : public ITypeInfoAbstract {};
#endif


}

#ifdef LEX_SOURCE
#include "Lexicon/Engine/ITypeInfoImpl.h"
#endif