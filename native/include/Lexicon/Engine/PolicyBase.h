#pragma once


#include "Lexicon/ITypePolicy.h"
#include "Lexicon/Engine/Environment.h"
#include "Lexicon/Engine/PolicyData.h"
#include "Lexicon/Engine/OverloadClause.h"

namespace LEX
{
	
	class PolicyBase : public virtual ITypePolicy, public SecondaryEnvironment, public OverloadClause, public PolicyData
	{//PolicyBase Might not even use clauses directly. We shall see.
	public:
		ITypePolicy* AsType() override { return this; }


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
		// a specialization ending up in there (Seeing as they must be kept as ITypePolicy)
		
	public:

		//TODO: I would like have more information here telling the compiler what it can and can't do with it.

		
		TypeID GetTypeID() const override
		{
			return _id;
		}

		//Rename to ForceTypeID, and then make set type ID the public one.
		void SetTypeID(TypeID id)
		{
			_id = id;
		}

		DataType GetDataType() const override { return _dataType; }

		std::string_view GetName() const override
		{
			return std::string_view{ _name };
		}

		/*

		OverloadClause* GetClause() override { return nullptr; }

		/*/

		bool CanMatch(QualifiedType, size_t suggested, size_t optional, OverloadFlag flag) override
		{
			return false;
		}

		std::vector<OverloadEntry> ResolveEntries(Overload& entries, ITypePolicy* scope, OverloadFlag& flags) override
		{
			return {};
		}


		OverloadEntry MatchSuggestedEntry(QualifiedType type, ITypePolicy* scope, size_t offset, size_t index, OverloadFlag& flags) override
		{
			flags |= OverloadFlag::Failure;
			return {};
		}
		OverloadEntry MatchDefaultEntry(QualifiedType type, ITypePolicy* scope, std::string name, OverloadFlag& flags) override
		{
			flags |= OverloadFlag::Failure;
			return {};
		}
		//*/
		//~

		HierarchyData* GetHierarchyData() const override
		{
			const HierarchyData* out = this;
			return const_cast<HierarchyData*>(out);
		}

		ITypePolicy* GetHierarchyType() override
		{
			return this;
		}




		


		//This needs some form of conversion result.
		ConvertResult IsConvertibleTo(const ITypePolicy* other, const ITypePolicy* scope, Conversion* out = nullptr, ConversionType type = ConversionType::Implicit) const override
		{
			if (this == other) {
				return ConvertResult::Exact;
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

			logger::info("running {} -> {}", GetName(), other->GetName());


			//Not gonna worry about function conversions for a while.
			if (!convert_data) {
				//TODO: If this is internal, I might perform some saving throw with the scope type.
				return ConvertResult::Ineligible;
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
				return ConvertResult::TypeDefined;//Type converted
			}

			if (!scope) {
				return ConvertResult::Inaccessible;//No access from here
			}

			if (scope == other) {
				return ConvertResult::TypeDefined;
			}

			//By this point, please note that internal should not even be a thought here.
			//Being able to get someone's inheritdata that belongs to a specific class might be valuable.
			//*Might need to recant this.
			const InheritData* access_data = scope->GetHierarchyData()->GetInheritData(other);



			if (!access_data || access_data->GetAccess() == Access::None) {
				return ConvertResult::Inaccessible;//No access from here
			}

			//if (access_data->GetAccess())



			return ConvertResult::TypeDefined;//Should have access
		}

		void CheckDeriveFrom(ITypePolicy* other) override;

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
		// seem like it needs to stay virtual, due to no longer needing to be from ITypePolicy. So maybe change that
		// and we're in the clear?
		// Also replace ITypePolicy's use in IdentityManager, and move it over to this thing (Meaning more source files. Yay).
		//For now, this works
		PolicyBase();

		PolicyBase(uint32_t i);

		PolicyBase(std::string_view name, TypeOffset offset);

	};

}
