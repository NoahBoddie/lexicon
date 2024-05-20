#pragma once


#include "ITypePolicy.h"
#include "Environment.h"
#include "PolicyData.h"
#include "OverloadClause.h"

namespace LEX
{


	class PolicyBase : public virtual ITypePolicy, public SecondaryEnvironment, public OverloadClause, public PolicyData
	{//PolicyBase Might not even use clauses directly. We shall see.


		//This is a pivot for Policies, generic or otherwise to exist, without possibly something like
		// a specialization ending up in there (Seeing as they must be kept as ITypePolicy)

	public:


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

		std::string GetName() override
		{
			return _name;
		}

		OverloadClause* GetClause() override { return this; }


		bool PreEvaluate(size_t suggested, size_t optional, OverloadFlag flag) override
		{
			return false;
		}

		std::vector<OverloadEntry> GetRemainingEvals(Overload& entries, ITypePolicy* scope, OverloadFlag& flags) override
		{
			return {};
		}


		//Fuck it, these return non-booleans and use something else to denote their failures.

		OverloadEntry EvaluateEntry2(QualifiedType type, ITypePolicy* scope, size_t offset, size_t index, OverloadFlag& flags) override
		{
			flags |= OverloadFlag::Failure;
			return {};
		}
		OverloadEntry EvaluateDefault2(QualifiedType type, ITypePolicy* scope, std::string name, OverloadFlag& flags) override
		{
			flags |= OverloadFlag::Failure;
			return {};
		}

		//~



		InheritData* _InheritData(InheritData& data)
		{
			if (data.IsInternal() == true) {
				return nullptr;
			}

			//Should search with an inherit data with the same type.
			auto end = inheritance.end();

			auto it = std::find_if(inheritance.begin(), end, [&](InheritData& other) {return data.type == other.type && !other.IsInternal(); });

			if (it != end)
				return &*it;

			return nullptr;
		}


		const InheritData* GetInheritData(const ITypePolicy* type) const override
		{
			//This ends up pulling intera

			//Should search with an inherit data with the same type.

			const InheritData* result = nullptr;

			//By default, it will exit automatically once when it finds something unless has internal is switched.
			bool exit = !hasInternal;

			for (const InheritData& other : inheritance)
			{
				//Ignore internal check if something doesn't personally have an internal.

				if (type != other.type)
					continue;


				if (other.IsInternal())
				{
					if (!exit) {
						if (_IsInternalParent(&other) == false) {
							continue;
						}

						//Otherwise we're exiting here
						exit = true;
					}
					else {
						//Cant allow an internal to end up here.
						continue;
					}
				}

				result = &other;

				if (exit)
					break;
			}

			return result;
		}

		InheritData* GetInheritData(ITypePolicy* type) override
		{
			const auto* _this = this;

			return const_cast<InheritData*>(_this->GetInheritData(type));
		}


		void FinalizeAndSort()
		{
			//std::sort(inheritance.begin(), inheritance.end(), std::less<InheritData>());return;

			for (int x = 0; x < inheritance.size(); x++)
			{
				InheritData& left = inheritance[x];

				for (int y = x + 1; y < inheritance.size(); y++)
				{
					InheritData& right = inheritance[y];

					if (right < left) {

						InheritData buffer = right;

						right = std::move(left);
						left = std::move(buffer);

						//Do a check of each here, switching 
						for (int z = x; z < inheritance.size(); z++)
						{
							auto& index = inheritance[z].ownerIndex;

							if (index - 1 == x) {
								index = y + 1;
							}
							else if (index - 1 == y) {
								index = x + 1;
							}
						}
					}
				}
			}

			uint32_t count = members.size();

			for (int x = 0; x < inheritance.size(); x++)
			{
				auto& entry = inheritance[x];
				logger::info("before {} {} {}", entry.memberIndex, count, entry.type->GetMemberCount());

				entry.memberIndex = count;
				count += (uint32_t)entry.type->GetMemberCount();

				logger::info("after {} {} {}", entry.memberIndex, count, entry.type->GetMemberCount());
			}

		}

		std::vector<InheritData> GetInheritFrom(uint32_t hashMin, uint32_t idxInc) override
		{
			//This should include access type, and who it's from. This way internal inheritance can be better handled.

			std::vector<InheritData> result;

			result.reserve(inheritance.size() + 1);

			InheritData& self = result.emplace_back();

			//uint32_t largestHash = hashMin;

			self.type = this;

			self.hash[0] = hashMin;
			self.hash[1] = hashRange + hashMin;

			self.access = Access::Public;

			//self.memberIndex = members

			self.distance = 1;

			//We'll want to make a copy, because we're gonna rehash it by the index.
			for (InheritData data : inheritance)
			{

				data.hash[0] += hashMin;
				data.hash[1] += hashMin;
				data.ownerIndex += idxInc;
				//I still have to check against this due to virtual inheritance.
				//if (largestHash < data.hash[1])
				//	largestHash = data.hash[1];


				switch (data.access)
				{
				case Access::Private:
					data.access = Access::None; break;
				case Access::PrivateInternal:
					data.access = Access::NoneInternal; break;
				}


				data.distance++;
				result.push_back(data);
			}

			//self.hash[1] = largestHash;

			return result;
		}


		void SetDerivesTo(ITypePolicy* other, bool intern = false, Access a_access = Access::Public)
		{
			//I need to carry around a set of types not allowed to be submitted for this when things need to handle their own inheritance.
			//Basically, for every item that SetDerivesTo is used on, that's another entry that SetDerives shouldn't be used on.
			// The nature of this means there's really no good time to be able to use set derives to, that being said, I need to handle it like a function
			// that need only run once. And if it relies on things that haven't run once, then it needs to run it once. For now, direct line of action.

			//Additionally, you cannot directly add stuff that you already have in, only when inherited from something else.

			//thread lock this, and remember a change of everything that set derived is processing.
			uint32_t idxInc = (uint32_t)inheritance.size() + 1;

			std::vector<InheritData> inherits = other->GetInheritFrom(hashRange + 1, idxInc);

			bool second = false;

			if (intern)
				a_access |= Access::Internal;

			//Later
			Access access = a_access & ~Access::Internal;

			bool intern_ = a_access & Access::Internal;

			if (!hasInternal)
				hasInternal = intern;


			for (auto& data : inherits)
			{
				bool _second = second;
				second = true;

				if (data.GetAccess() > access) {
					data.SetAccess(access);
				}

				if (intern_) {
					data.SetInternal(true);
				}


				if (data.IsInternal() == false) {
					if (auto* prev_data = _InheritData(data); prev_data)
					{


						//We'll want to pretend like it's a new object without disturbing it. Making it 0 is fine, as it needs
						// to pull results from somewhere else to be valid and if it can't either something else is better or it's ambiguous.


						//having these use the hash of 0 makes it valid for comparison for nothing else (if not corrected by it's comparison) other than
						// the derived class.

						prev_data->hash[0] = 0;
						prev_data->hash[1] = 0;
						prev_data->ownerIndex = 0;
						prev_data->virtInherited = true;

						//Inherits whichever access from here is greater.
						if (auto data_access = data.GetAccess(); data_access > prev_data->GetAccess()) {
							prev_data->SetAccess(data_access);
						}

						//I think the distance will actually be 2 if it's not the first.
						prev_data->distance = 1 + _second;

						continue;
					}
				}


				if (data.hash[1] > hashRange)
					hashRange = data.hash[1];

				auto old = memberCount;

				data.memberIndex = old;//This is the place where this starts.
				memberCount += (uint32_t)data.type->GetMemberCount();
				//logger::info("add {} {} {}", old, data.memberIndex, memberCount);
				//This doesn't get hit at times. I need to inspect why.

				inheritance.push_back(data);
			}
		}

		OverloadCode CreateCode(ITypePolicy* target) override
		{//Change name to generate code probably more like.
			auto data = !target || target == this ? nullptr : GetInheritData(target);


			if (data)
				return {data->hash, data->distance, !data->virtInherited };
			else
				return { {0, hashRange}, 0, true };
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
			const InheritData* access_data = scope->GetInheritData(other);



			if (!access_data || access_data->GetAccess() == Access::None) {
				return ConvertResult::Inaccessible;//No access from here
			}

			//if (access_data->GetAccess())



			return ConvertResult::TypeDefined;//Should have access
		}



		void PrintInheritance()
		{
			RGL_LOG(info, "Class: {}: Ranged: (0/{}), Members: ({}/{})", _name, hashRange, memberCount, members.size());

			for (auto& basis : inheritance)
			{
				std::string access;

				switch (basis.access)
				{
				case Access::Public:
					access = "public"; break;
				case Access::Protected:
					access = "protected"; break;
				case Access::Private:
					access = "private"; break;
				case Access::None:
					access = "no access"; break;

				case Access::PublicInternal:
					access = "public internal"; break;
				case Access::ProtectedInternal:
					access = "protected internal"; break;
				case Access::PrivateInternal:
					access = "private internal"; break;
				case Access::NoneInternal:
					access = "internal"; break;
				}

				logger::info("|	Name: {}, Hash: {}/{}, Dist: {}, Ownr: {}, intern: {}, access: {}, Mbrs: ({}+{})",
					basis.type->GetName(), basis.hash[0], basis.hash[1], basis.distance, basis.ownerIndex, basis.IsInternal(), access, basis.memberIndex, basis.type->GetMemberCount());
			}
		}

		//This should be moved to qualified type, and the main thing of desire here should be the overload code.
		int CompareType(OverloadCode& left, OverloadCode& right, QualifiedType&& left_type, QualifiedType&& right_type)
		{
			//This should be using OverloadCode, but i'd need a given type to do it.

			if (!left.initialized && !right.initialized)
			{
				left = left.FinalizeOld(left_type, right_type);
				right = right.FinalizeOld(right_type, left_type);

				//OverloadEntry left = a_lhs.FinalizeOld(a_rhs.type);
				//OverloadEntry right = a_rhs.FinalizeOld(a_lhs.type);
			}

			logger::info("left({}/{}), right({}/{})", left.hash[0], left.hash[1], right.hash[0], right.hash[1]);


			if (left.hash[0] <= right.hash[1] && left.hash[1] >= right.hash[0]) {
				return left.distance - right.distance;
			}
			if (left.hash[0] >= right.hash[1] && left.hash[1] <= right.hash[0]) {
				return left.distance - right.distance;
			}

			return 0;
		}

		int CompareType(ITypePolicy* a_lhs, ITypePolicy* a_rhs)
		{
			OverloadCode left = CreateCode(a_lhs);
			OverloadCode right = CreateCode(a_rhs);

			return CompareType(left, right, QualifiedType{ a_lhs }, QualifiedType{ a_rhs });
		}

		bool _IsInternalParent(const InheritData* intern) const
		{
			if (!intern || intern->IsInternal() == false)
				return false;

			bool cont = intern->ownerIndex && intern->IsInternal();

			while (cont)
			{
				intern = &inheritance[intern->ownerIndex - 1];

				cont = intern->ownerIndex && intern->IsInternal();
			}

			return intern->IsInternal() && intern->ownerIndex == 0;

		}


		size_t GetMemberCount(bool total = false) const override
		{
			return total ? memberCount : members.size();
		}

		virtual _String GetName() const
		{
			return _name;
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
