#pragma once


#include "Lexicon/Engine/ITypeInfoImpl.h"
#include "Lexicon/Engine/Environment.h"
#include "Lexicon/Engine/PolicyData.h"
#include "Lexicon/Engine/OverloadParameter.h"
#include "Lexicon/Engine/AttributeHandler.h"

//*src
#include "Lexicon/ITemplatePart.h"
#include "Lexicon/Engine/Convert.h"
namespace LEX
{
	class TypeBase;

	struct TypeNode
	{
		TypeNode() = default;
		TypeNode(TypeBase* b, ITypeInfo* i) : base{ b }, info{ i } {}

		TypeBase* base = nullptr;
		ITypeInfo* info = nullptr;
	};

	class TypeBase : public SecondaryEnvironment, public OverloadParameter, public PolicyData, public AttributeHandler
	{//TypeBase Might not even use clauses directly. We shall see.
	public:
		DEFINE_COMPONENT_TYPE(ComponentType::TypeBase)

	protected:



		ELEM_ENUM
		{
			ELEM_FLAG(External, 0),
			ELEM_FLAG(InheritHandled, 1),

			ELEM_NEXT,
		};

		//This is a pivot for Policies, generic or otherwise to exist, without possibly something like
		// a specialization ending up in there (Seeing as they must be kept as ITypeInfo)

	public:


		
		

		TypeNode CreateNode(ITemplatePart* part)
		{
			auto type = AsType();
			return TypeNode{ this, part->GetSize() ? type->CheckTypeInfo(part) : type };
		}

		//TODO: I would like have more information here telling the compiler what it can and can't do with it.



		virtual ITypeInfo* AsType() = 0;
		virtual const ITypeInfo* AsType() const = 0;


		//Rename to ForceTypeID, and then make set type ID the public one.
		void SetTypeID(TypeID id)
		{
			_id = id;
		}



		IHierarchyTree* GetHierarchyTree() const
		{
			return unconst(this);
		}

		TypeID GetTypeID() const { return _id; }
		DataType GetDataType() const { return _dataType; }


		std::string_view GetName() const override { return _name; }

		void HandleAttributes() override
		{
			SyntaxRecord* syntax = GetSyntaxTree();
			if (syntax) {
				if (SyntaxRecord* attribute_record = syntax->FindChild(parse_strings::attributes))

				return LoadAttributes(this, GetParent(), *attribute_record);
			}
		}

		/*

		OverloadClause* GetClause() override { return nullptr; }

		/*/



		bool CanMatch(const QualifiedType& target, size_t callArgs, size_t tempArgs, OverloadFlag) override
		{
			return false;
		}
		bool MatchImpliedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags) override
		{
			return false;
		}
		bool MatchStatedEntry(OverloadEntry& out, const QualifiedType&, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags)override
		{
			return false;
		}
		void QualifyOverload(Overload& overload) override {}
		bool ResolveOverload(Overload& entries, OverloadFlag& flags) override { return true; }
		//*/
		//~


		void CheckDeriveFrom(IHierarchyTree* other, SyntaxRecord& record) override;

		void OnInherit(ITypeInfo* other, SyntaxRecord& record);

		bool IsExternal() const
		{
			return GetFlags() & Flag::External;
		}


		bool IsInheritHandled() const
		{
			return GetFlags() & Flag::InheritHandled;
		}



		virtual std::vector<Directory*> GetAssociates(RelateType type) override
		{

			if (type != RelateType::Nested)
				return {};

			//auto data = GetHierarchyData();

			//data->GetInheritData(nullptr);

			return {};
		}



		IFunction* FindConstructor(OverloadArgument& key, Overload& out);

	protected:
		void MarkInheritHandled() const
		{
			GetFlags() |= Flag::InheritHandled;
		}

		
		void MarkExternal() const
		{
			GetFlags() |= Flag::External;
		}

	////////////////////////
	//TypeInfo Virtuals   //
	////////////////////////
	protected:
		ObjectPolicy* GetObjectPolicyImpl() const noexcept
		{
			return GetPolicy();
		}





	public:

		void HandleInheritance() override;

		virtual TypeRuleset GetRuleset() const
		{
			return TypeRuleset::None;
		}


		//These doing this is kinda ill advised, but since the function is defined here, it's safe to do. Specially since
		// this is the final version of these functions.

		//Still, this is terrible practice and this likely needs to get changed. SetTypeID doesn't really
		// seem like it needs to stay virtual, due to no longer needing to be from ITypeInfo. So maybe change that
		// and we're in the clear?
		// Also replace ITypeInfo's use in IdentityManager, and move it over to this thing (Meaning more source files. Yay).
		//For now, this works
		TypeBase();
		
		
		TypeBase(uint32_t i);

		TypeBase(std::string_view name, TypeOffset offset);
	
	protected:
		TypeBase(InherentType type)
		{
			_id = (uint32_t)type;
		}
	};

	//For what it's worth, I really fucking loathe this system all together.
	template <typename T>
	struct PivotTypeBase : public TypeBase, public TypeInstance<T>
	{
		using TypeBase::TypeBase;

		std::string_view GetName() const 
		{ 
			return TypeBase::GetName();
		}

		InstanceID GetInstanceID() const override
		{
			return TypeInstance<T>::GetInstanceID();

		}
		IFunction* FindConstructor(OverloadArgument& key, Overload& out) override
		{
			return TypeBase::FindConstructor(key, out);
		}


		IHierarchyTree* GetHierarchyTree() const override
		{
			return TypeBase::GetHierarchyTree();
		}

		std::span<AttributeBase*> GetAttributes() override
		{ 
			return AttributeHandler::GetAttributes();
		}

		ITypeInfo* GetHierarchyType() override
		{
			return this;
		}

		TypeID GetTypeID() const override
		{
			return TypeBase::GetTypeID();
		}

		DataType GetDataType() const override { return TypeBase::GetDataType(); }


		ObjectPolicy* GetObjectPolicy() const override
		{
			return GetObjectPolicyImpl();
		}


		void SetSelfQualifiers(Qualifier& qualifiers) const override
		{
			//TODO: If a struct is the only one of it's kind, it shouldn't have to make itself readonly.
			// This is the reason C# structs are the way they are, and if I can make that promise that there is nothing else and will be nothing else it's good
			// Also, if the struct is final, it's also good.
			if (!this->IsValueType() || IsExternal() == true)
				qualifiers.MakeReadonly(true);
		}




		ITypeInfo* AsType() override { return this; }
		const ITypeInfo* AsType() const override { return this; }

	};

	using GenericTypeBase = PivotTypeBase<ITypeInfo>;
	using ConcreteTypeBase = PivotTypeBase<TypeInfo>;
}
