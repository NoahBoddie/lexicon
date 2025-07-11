#pragma once


#include "Lexicon/Engine/ITypeInfoImpl.h"
#include "Lexicon/Engine/Environment.h"
#include "Lexicon/Engine/PolicyData.h"
#include "Lexicon/Engine/OverloadClause.h"

//*src
#include "Lexicon/ITemplatePart.h"

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

	class TypeBase : public SecondaryEnvironment, public OverloadParameter, public PolicyData
	{//TypeBase Might not even use clauses directly. We shall see.

	private:


		using Prev = Flag;
		enum Flag
		{
			None = 0,
			LinkExtern = 1 << (Prev::_next + 0),
			InheritHandled = 1 << (Prev::_next + 1),


			_next = 1 << (Prev::_next + 2),

		};

		//This is a pivot for Policies, generic or otherwise to exist, without possibly something like
		// a specialization ending up in there (Seeing as they must be kept as ITypeInfo)
		
	public:

		TypeNode CreateNode(ITemplatePart* part) 
		{
			auto type = AsType();
			return TypeNode{ this, part->GetSize() ? type->CheckTypePolicy(part) : type };
		}

		//TODO: I would like have more information here telling the compiler what it can and can't do with it.

		

		virtual ITypeInfo* AsType() = 0;
		virtual const ITypeInfo* AsType() const = 0;
		

		//Rename to ForceTypeID, and then make set type ID the public one.
		void SetTypeID(TypeID id)
		{
			_id = id;
		}



		HierarchyData* GetHierarchyData() const
		{
			const HierarchyData* out = this;
			return const_cast<HierarchyData*>(out);
		}

		TypeID GetTypeID() const { return _id; }
		DataType GetDataType() const { return _dataType; }


		std::string_view GetName() const { return std::string_view{ _name }; }

		/*

		OverloadClause* GetClause() override { return nullptr; }

		/*/



		bool CanMatch(const QualifiedType& target, size_t callArgs, size_t tempArgs, OverloadFlag) override
		{return false;}
		bool MatchImpliedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags) override
		{return false;}
		bool MatchStatedEntry(OverloadEntry& out, const QualifiedType&, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags)override
		{return false;}
		void QualifyOverload(Overload& overload) override {}
		bool ResolveOverload(Overload& entries, OverloadFlag& flags) override { return true; }
		//*/
		//~

		ITypeInfo* GetHierarchyType() override
		{
			return AsType();
		}

		
		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<ITypeInfo>::value):
				return AsType();
			
			case Hash(TypeName<TypeBase>::value):
				return this;
			}
			return __super::Cast(name);
		}


		


		//This needs some form of conversion result.
		ConvertResult GetConvertTo(const ITypeInfo* other, const ITypeInfo* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const
		{
			if (AsType() == other) {
				return ConversionEnum::Exact;
			}


			//First we check if the entry exists. Based on what its access is, what we do changes.
			//Public: Conversion is allowed.
			//Protected: scope type must other must derive from
			//Private: scope type must have other's distance be 1.

			//Internal: The trail of internals within scope must lead to an internal that has an owner index of 0. Otherwise, it is too seperated to be converted.
			//Will need to make a collection to measure this.
			// Actually, simplier way (though the above is still required) this and scope need to be the same. Actually this isn't right either, internal is about
			// this, not who's scope.


			//Whether it can convert should basically be solved here, this should never return an internal it does not own.
			const InheritData* convert_data = GetInheritData(other);


			//Not gonna worry about function conversions for a while.
			if (!convert_data) {
				//TODO: If this is internal, I might perform some saving throw with the scope type.
				return ConversionResult::Ineligible;
			}

			//To do this saving throw, we'd need to get if scope has that inheritdata from other first (check for internal first)
			// then, if it does, ask if scope is viewable this is convertible to scope.



			Access access = convert_data->access & ~Access::Internal;
			//Should this ever actually be internal?
			bool is_internal = convert_data->IsInternal();//convert_data->access & Access::Internal;

			if (is_internal) {
				logger::info("Owner should be 0 => {}", convert_data->ownerIndex);
			}


			switch (access)
			{
				//No access is given, thus conversion is not possible from here.
				//*This has to be tested later.
				//case Access::None:
				//	return false;//No access between types

			case Access::Public:
				return ConversionEnum::TypeDefined;//Type converted
			}

			if (!scope) {
				return ConversionResult::Inaccessible;//No access from here
			}

			if (scope == other) {
				return ConversionEnum::TypeDefined;
			}

			//By this point, please note that internal should not even be a thought here.
			//Being able to get someone's inheritdata that belongs to a specific class might be valuable.
			//*Might need to recant this.
			const InheritData* access_data = scope->GetHierarchyData()->GetInheritData(other);



			if (!access_data || access_data->GetAccess() == Access::None) {
				return ConversionResult::Inaccessible;//No access from here
			}

			//if (access_data->GetAccess())



			return ConversionEnum::TypeDefined;//Should have access
		}

		void CheckDeriveFrom(ITypeInfo* other) override;

		Flag& GetFlags() const
		{
			return GetComponentData<Flag>();
		}

		bool IsLinkExtern() const
		{
			return GetFlags() & Flag::LinkExtern;
		}

		
		bool IsInheritHandled() const
		{
			return GetFlags() & Flag::InheritHandled;
		}

	protected:
		void MarkInheritHandled() const
		{
			GetFlags() |= Flag::InheritHandled;
		}

		
		void MarkLinkExtern() const
		{
			GetFlags() |= Flag::LinkExtern;
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

	};

	//For what it's worth, I really fucking loathe this system all together.
	template <typename T>
	struct PivotTypeBase : public TypeBase, public T
	{
		using TypeBase::TypeBase;

		std::string_view GetName() const 
		{ 
			return TypeBase::GetName();
		}


		HierarchyData* GetHierarchyData() const override
		{
			return TypeBase::GetHierarchyData();
		}

		TypeID GetTypeID() const override
		{
			return TypeBase::GetTypeID();
		}
		DataType GetDataType() const override { return TypeBase::GetDataType(); }


		ConvertResult GetConvertTo(const ITypeInfo* other, const ITypeInfo* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const override
		{
			return TypeBase::GetConvertTo(other, scope, out, flags);
		}



		void SetSelfQualifiers(Qualifier& qualifiers) const override
		{
			//TODO: If a struct is the only one of it's kind, it shouldn't have to make itself readonly.
			// This is the reason C# structs are the way they are, and if I can make that promise that there is nothing else and will be nothing else it's good
			// Also, if the struct is final, it's also good.
			if (!this->IsValueType() || IsLinkExtern() == true)
				qualifiers.MakeReadonly(true);
		}




		ITypeInfo* AsType() override { return this; }
		const ITypeInfo* AsType() const override { return this; }

	};

	using GenericTypeBase = PivotTypeBase<ITypeInfo>;
	using ConcreteTypeBase = PivotTypeBase<TypeInfo>;
}
