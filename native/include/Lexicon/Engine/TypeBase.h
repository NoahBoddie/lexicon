#pragma once


#include "Lexicon/Engine/AbstractType.h"
#include "Lexicon/Engine/Environment.h"
#include "Lexicon/Engine/PolicyData.h"
#include "Lexicon/Engine/OverloadClause.h"

namespace LEX
{
	
	class TypeBase : public SecondaryEnvironment, public OverloadClause, public PolicyData
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
		// a specialization ending up in there (Seeing as they must be kept as IType)
		
	public:

		//TODO: I would like have more information here telling the compiler what it can and can't do with it.

		

		virtual AbstractType* AsAbstract() = 0;
		virtual const AbstractType* AsAbstract() const = 0;
		

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

		bool CanMatch(QualifiedType, size_t suggested, size_t optional, OverloadFlag flag) override
		{
			return false;
		}

		std::vector<OverloadEntry> ResolveEntries(Overload& entries, AbstractType* scope, OverloadFlag& flags) override
		{
			return {};
		}


		OverloadEntry MatchSuggestedEntry(QualifiedType type, AbstractType* scope, size_t offset, size_t index, OverloadFlag& flags) override
		{
			flags |= OverloadFlag::Failure;
			return {};
		}
		OverloadEntry MatchDefaultEntry(QualifiedType type, AbstractType* scope, std::string name, OverloadFlag& flags) override
		{
			flags |= OverloadFlag::Failure;
			return {};
		}
		//*/
		//~

		AbstractType* GetHierarchyType() override
		{
			return AsAbstract();
		}

		
		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<AbstractType>::value):
			case Hash(TypeName<IType>::value):
				return AsAbstract();
			
			case Hash(TypeName<TypeBase>::value):
				return this;
			}
			return __super::Cast(name);
		}


		


		//This needs some form of conversion result.
		ConvertResult GetConvertTo(const AbstractType* other, const AbstractType* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const
		{
			if (AsAbstract() == other) {
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

		void CheckDeriveFrom(AbstractType* other) override;

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
		// seem like it needs to stay virtual, due to no longer needing to be from IType. So maybe change that
		// and we're in the clear?
		// Also replace IType's use in IdentityManager, and move it over to this thing (Meaning more source files. Yay).
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


		ConvertResult GetConvertTo(const AbstractType* other, const AbstractType* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const override
		{
			return TypeBase::GetConvertTo(other, scope, out, flags);
		}


		AbstractType* AsAbstract() override { return this; }
		const AbstractType* AsAbstract() const override { return this; }

	};

	using GenericTypeBase = PivotTypeBase<AbstractType>;
	using ConcreteTypeBase = PivotTypeBase<Type>;
}
