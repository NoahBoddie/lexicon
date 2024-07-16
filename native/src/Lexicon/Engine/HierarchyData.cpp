#pragma once

#include "Lexicon/Engine/HierarchyData.h"

#include "Lexicon/ITypePolicy.h"
#include "Lexicon/Engine/PolicyBase.h"
#include "Lexicon/Engine/OverloadEntry.h"
#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{

	InheritData* HierarchyData::_InheritData(InheritData& data)
	{
		if (data.IsInternal() == true) {
			return nullptr;
		}

		//Should search with an inherit data with the same type.
		auto end = inheritance.end();

		auto it = std::find_if(inheritance.begin(), end, [&](InheritData& other) { return data.type == other.type && !other.IsInternal(); });

		if (it != end)
			return &*it;

		return nullptr;
	}

	const InheritData* HierarchyData::GetInheritData(const ITypePolicy* type) const
	{
		//This ends up pulling intera

		//Should search with an inherit data with the same type.

		const InheritData* result = nullptr;

		//By default, it will exit automatically once when it finds something unless has internal is switched.
		bool exit = !hasInternal;

		for (const InheritData& other : inheritance) {
			//Ignore internal check if something doesn't personally have an internal.

			if (type != other.type)
				continue;

			if (other.IsInternal()) {
				if (!exit) {
					if (_IsInternalParent(&other) == false) {
						continue;
					}

					//Otherwise we're exiting here
					exit = true;
				} else {
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

	InheritData* HierarchyData::GetInheritData(ITypePolicy* type)
	{
		const auto* _this = this;

		return const_cast<InheritData*>(_this->GetInheritData(type));
	}

	void HierarchyData::FinalizeAndSort()
	{
		//std::sort(inheritance.begin(), inheritance.end(), std::less<InheritData>());return;

		for (auto& affix : GetPostAffixedTypes()){
			SetInheritFrom(affix, Access::Public, true);
		}

		if (inheritance.size() == 0)
			return;

		for (int x = 0; x < inheritance.size(); x++) {
			InheritData& left = inheritance[x];

			for (int y = x + 1; y < inheritance.size(); y++) {
				InheritData& right = inheritance[y];

				if (right < left) {
					InheritData buffer = right;

					right = std::move(left);
					left = std::move(buffer);

					//Do a check of each here, switching
					for (int z = x; z < inheritance.size(); z++) {
						auto& index = inheritance[z].ownerIndex;

						if (index - 1 == x) {
							index = y + 1;
						} else if (index - 1 == y) {
							index = x + 1;
						}
					}
				}
			}
		}

		uint32_t count = members.size();

		for (int x = 0; x < inheritance.size(); x++) {
			auto& entry = inheritance[x];
			//logger::info("before {} {} {}", entry.memberIndex, count, entry.type->GetHierarchyData()->members.size());

			entry.memberIndex = count;
			count += (uint32_t)entry.type->GetHierarchyData()->members.size();

			//logger::info("after {} {} {}", entry.memberIndex, count, entry.type->GetHierarchyData()->members.size());
		}
	}

	std::vector<InheritData> HierarchyData::GetInheritFrom(uint32_t hashMin, uint32_t idxInc)
	{
		//This should include access type, and who it's from. This way internal inheritance can be better handled.

		std::vector<InheritData> result;

		result.reserve(inheritance.size() + 1);

		InheritData& self = result.emplace_back();

		//uint32_t largestHash = hashMin;

		self.type = GetHierarchyType();

		self.hash[0] = hashMin;
		self.hash[1] = hashRange + hashMin;

		self.access = Access::Public;

		//self.memberIndex = members

		self.distance = 1;// +(self.type->GetName() == "Object");

		//We'll want to make a copy, because we're gonna rehash it by the index.
		for (InheritData data : inheritance) {
			data.hash[0] += hashMin;
			data.hash[1] += hashMin;
			data.ownerIndex += idxInc;
			//I still have to check against this due to virtual inheritance.
			//if (largestHash < data.hash[1])
			//	largestHash = data.hash[1];

			switch (data.access) {
			case Access::Private:
				data.access = Access::None;
				break;
			case Access::PrivateInternal:
				data.access = Access::NoneInternal;
				break;
			}

			data.distance++;
			result.push_back(data);
		}

		//self.hash[1] = largestHash;

		return result;
	}


	
	void HierarchyData::SetInheritFrom(ITypePolicy* other, Access a_access, bool post_affixed)
	{
		//I need to carry around a set of types not allowed to be submitted for this when things need to handle their own inheritance.
		//Basically, for every item that SetDerivesTo is used on, that's another entry that SetDerives shouldn't be used on.
		// The nature of this means there's really no good time to be able to use set derives to, that being said, I need to handle it like a function
		// that need only run once. And if it relies on things that haven't run once, then it needs to run it once. For now, direct line of action.

		//Additionally, you cannot directly add stuff that you already have in, only when inherited from something else.

		//thread lock this, and remember a change of everything that set derived is processing.

		HierarchyData* heir = other->GetHierarchyData();

		uint32_t idxInc = (uint32_t)inheritance.size() + 1;

		std::vector<InheritData> inherits = heir->GetInheritFrom(hashRange + 1, idxInc);

		bool second = false;

		//Later
		Access access = a_access & ~Access::Internal;

		bool intern_ = a_access & Access::Internal;

		if (!hasInternal)
			hasInternal = intern_;

		ITypePolicy* self = GetHierarchyType();


		for (auto& data : inherits) {
			bool _second = second;
			second = true;

			if (data.type == self) {
				report::compile::critical("Type '{}' cannot inherit from itself", self->GetName());
			}

			InheritData* prev_data = _InheritData(data);

			if (prev_data && prev_data->distance == 1 && prev_data->ownerIndex == 1 && data.distance == 1 && data.ownerIndex == 0) {
				report::compile::critical("Type '{}' inherits {} directly multiple times.", self->GetName(), prev_data->type->GetName());
			}


			

			if (data.GetAccess() > access) {
				data.SetAccess(access);
			}

			if (intern_) {
				data.SetInternal(true);
			}


			if (post_affixed) {
				//if (_second)
				//	data.distance++;

				data.SetAffixed(true);
			}

			if (data.IsInternal() == false) {
				if (prev_data) {
					//We'll want to pretend like it's a new object without disturbing it. Making it 0 is fine, as it needs
					// to pull results from somewhere else to be valid and if it can't either something else is better or it's ambiguous.

					//having these use the hash of 0 makes it valid for comparison for nothing else (if not corrected by it's comparison) other than
					// the derived class.

					bool both_affixed = prev_data->IsAffixed() && data.IsAffixed();



					prev_data->hash[0] = 0;
					prev_data->hash[1] = both_affixed ? -1 : 0;
					//^This will ensure literally anything will count toward this, only until it's manually inherited. This may fuck
					// with entries that aren't compatible with it, but this shit is bottom of the barrel in terms of coversion, it's the least prefered.
					
					prev_data->ownerIndex = 0;
					prev_data->virtInherited = true;

					//Inherits whichever access from here is greater.
					if (auto data_access = data.GetAccess(); data_access > prev_data->GetAccess()) {
						prev_data->SetAccess(data_access);
					}


					if (both_affixed) {
						if (prev_data->distance < data.distance)
							prev_data->distance = data.distance;
					}
					else
						//I think the distance will actually be 2 if it's not the first.
						prev_data->distance = 1 + _second;

					continue;
				}
			}

			

			if (data.hash[1] > hashRange)
				hashRange = data.hash[1];

			auto old = memberCount;

			data.memberIndex = old;  //This is the place where this starts.
			memberCount += (uint32_t)data.type->GetHierarchyData()->members.size();
			//logger::info("add {} {} {}", old, data.memberIndex, memberCount);
			//This doesn't get hit at times. I need to inspect why.

			inheritance.push_back(data);
		}
	}


	void HierarchyData::SetDerivesTo(ITypePolicy* other, Access a_access)
	{
		
		HierarchyData* heir = other->GetHierarchyData();

		ITypePolicy* self = GetHierarchyType();

		CheckDeriveFrom(other);

		if (self == other)
			report::compile::critical("Type '{}' cannot inherit from itself", self->GetName());

		heir->HandleInheritance();
		
		return SetInheritFrom(other, a_access);
	}

	
	OverloadCode HierarchyData::CreateCode(ITypePolicy* target)
	{  //Change name to generate code probably more like.
		ITypePolicy* type = GetHierarchyType();

		auto data = !target || target == type ? nullptr : GetInheritData(target);

		if (data)
			return { data->hash, data->distance, !data->virtInherited };
		else
			return { { 0, hashRange }, 0, true };
	}


	void HierarchyData::PrintInheritance()
	{
		std::string name;

		if (auto type = GetHierarchyType()) {
			name = type->GetName();
		}

		RGL_LOG(info, "Class: {}: Ranged: (0/{}), Members: ({}/{})", name, hashRange, memberCount, members.size());

		for (auto& basis : inheritance) {
			std::string access;

			switch (basis.access) {
			case Access::Public:
				access = "public";
				break;
			case Access::Protected:
				access = "protected";
				break;
			case Access::Private:
				access = "private";
				break;
			case Access::None:
				access = "no access";
				break;

			case Access::PublicInternal:
				access = "public internal";
				break;
			case Access::ProtectedInternal:
				access = "protected internal";
				break;
			case Access::PrivateInternal:
				access = "private internal";
				break;
			case Access::NoneInternal:
				access = "internal";
				break;
			}

			//I'm going to leave this because it needs a source file
			logger::info("|	Name: {}, Hash: {}/{}, Dist: {}, Ownr: {}, intern: {}, access: {}, Mbrs: ({}+{}), postfix: {}",
				basis.type->GetName(), basis.hash[0], basis.hash[1], basis.distance, basis.ownerIndex, basis.IsInternal(), 
				access, basis.memberIndex, basis.type->GetHierarchyData()->members.size(), basis.IsAffixed());
		}
	}

	//This should be moved to qualified type, and the main thing of desire here should be the overload code.
	int HierarchyData::CompareType(OverloadCode& left, OverloadCode& right, QualifiedType&& left_type, QualifiedType&& right_type)
	{
		//This should be using OverloadCode, but i'd need a given type to do it.

		if (!left.initialized && !right.initialized) {
			left = left.FinalizeOld(left_type->GetHierarchyData(), right_type->GetHierarchyData());
			right = right.FinalizeOld(right_type->GetHierarchyData(), left_type->GetHierarchyData());

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

	int HierarchyData::CompareType(ITypePolicy* a_lhs, ITypePolicy* a_rhs)
	{
		OverloadCode left = CreateCode(a_lhs);
		OverloadCode right = CreateCode(a_rhs);

		return CompareType(left, right, QualifiedType{ a_lhs }, QualifiedType{ a_rhs });
	}

	bool HierarchyData::_IsInternalParent(const InheritData* intern) const
	{
		if (!intern || intern->IsInternal() == false)
			return false;

		bool cont = intern->ownerIndex && intern->IsInternal();

		while (cont) {
			intern = &inheritance[intern->ownerIndex - 1];

			cont = intern->ownerIndex && intern->IsInternal();
		}

		return intern->IsInternal() && intern->ownerIndex == 0;
	}

	std::vector<ITypePolicy*> HierarchyData::GetPostAffixedTypes() const
	{
		return { IdentityManager::instance->GetBaseByOffset("CORE", 0) };
	}

}