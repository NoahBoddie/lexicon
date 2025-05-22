#pragma once

#include "Lexicon/ISpecial.h"
#include "Lexicon/TypeID.h"


#include "Lexicon/DataType.h"

#include "Lexicon/Conversion.h"

#include "Lexicon/Reflection.h"


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

	

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION_DERIVES(ITypeInfo, ISpecial), public Reflection
			{
			private:
				enum Offset
				{
					kBasic,
					kGeneric,
					kConcrete,
				};

			public:
				constexpr static uint32_t NonGenericIndex = -1;

				Reflect GetReflect() const override
				{
					return Reflect::Type;
				}

				size_t GetReflectOffset() const override
				{
					if (IsResolved() == true) {
						return kConcrete;
					}
					else if (auto spec = GetSpecializable())
					{
						return kGeneric;
					}
					else
					{
						//This shouldn't actually be possible, but I'm just putting it here
						return kBasic;
					}
				}
				bool IsValidOffset(size_t offset) const override
				{
					auto self = GetReflectOffset();

					if (offset == (size_t)kBasic)
						return true;

					return __super::IsValidOffset(offset);
				}




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

				TypeInfo* FetchTypePolicy(ITemplateBody* args)
				{
					return this ? GetTypePolicy(args) : nullptr;
				}


				virtual TypeID GetTypeID() const = 0;

				//Should this be higher?
				virtual DataType GetDataType() const = 0;


				DataType FetchDataType() const
				{
					return this ? GetDataType() : DataType::Invalid;
				}
				//*
				virtual bool IsReferType() const = 0;

				bool IsValueType() const
				{
					return !IsReferType();
				}
				//*/

				
				TypeID FetchTypeID() const
				{
					return this ? GetTypeID() : TypeID{ 0 };
				}

				virtual bool CanConvert(const ITypeInfo* other) const = 0;

				virtual bool Convert(const Variable& from, Variable& to, const ITypeInfo* type) const = 0;

				//Make a type that produces an error
				//virtual bool Convert(const Variable& from, Variable& to, const ITypeInfo* type) = 0;


				//Make some safe functions for these.


			public:
				
				virtual std::string_view GetName() const = 0;

				virtual std::span<ITypeInfo*> GetTemplate() { return {}; }


			};
		}

		CURRENT_VERSION(ITypeInfo, 1);
	}

#ifndef LEX_SOURCE
	//Only accessible outside of the source.
	struct IMPL_VERSION(ITypeInfo) {};
#endif
	
}