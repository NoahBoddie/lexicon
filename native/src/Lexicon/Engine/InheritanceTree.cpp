#include "Lexicon/Engine/InheritanceTree.h"

#include "Lexicon/Function.h"
#include "Lexicon/Engine/TypeBase.h"
#include "Lexicon/Engine/MemberData.h"
#include "Lexicon/Engine/OverloadEntry.h"
#include "Lexicon/Interfaces/IdentityManager.h"
namespace LEX
{
    Function* InheritanceTree::GetMethod(MemberPointer ptr, ITemplateBody* body)
    {
        ptr = ptr.Resolve(body);

        Function* result = nullptr;

        VisitTrees([&](IHierarchyTree* tree) -> bool
            {
                if (auto members = tree->GetMembers())
                {
                    auto it = members->methods.find(ptr);
                    auto end = members->methods.end();

                    if (it != end) {
                        return result = it->second->GetFunction(body);
                    }
                }


                return false;
            });

        return result;
    }

    size_t InheritanceTree::GetField(MemberPointer ptr, ITemplateBody* body)
    {
        ptr = ptr.Resolve(body);

        size_t index = ptr.index;

        if (GetInstanceID() != ptr.instanceID) {
            bool found = VisitLowerTrees([&](InheritNode& inherit) -> bool
                {
                    auto tree = inherit.tree;

                    if (tree->GetInstanceID() == ptr.instanceID) {
                        //This has issues in that it's going to be missing all the indices
                        // previous 
                        if (auto members = tree->GetMembers(); members && members->fields.size() > index) {
                            index += inherit.memberIndex;
                            return true;
                        }
                    }

                    return false;
                });

            if (!found)
                return -1;
        }

        return index;
    }



    const InheritNode* InheritanceTree::FindInheritNode(const IHierarchyTree* tree) const
    {

        //Should search with an inherit data with the same type.
        auto it = inheritance.begin();
        auto end = inheritance.end();

        it = std::find_if(it, end, [&](const InheritNode& other) { return tree == other.tree; });

        if (it != end)
            return &*it;

        return nullptr;
    }



    void InheritanceTree::FinalizeAndSort()
    {
        //std::sort(inheritance.begin(), inheritance.end(), std::less<InheritData>());return;

        for (auto& affix : GetPostAffixedTypes()) {
            //if (GetInheritData(affix) == nullptr) 
            //GetInheritIndex(affix) == -1 && 
            if (affix != this)
            {
                SetInheritFrom(affix, Access::Public, true);
            }
        }

        if (inheritance.size() == 0)
            return;

        for (int x = 0; x < inheritance.size(); x++) {
            InheritNode& left = inheritance[x];

            for (int y = x + 1; y < inheritance.size(); y++) {
                InheritNode& right = inheritance[y];

                if (right < left) {
                    //InheritData buffer = right;
                    //right = std::move(left);
                    //left = std::move(buffer);
                    std::swap(left, right);

                    //Do a check of each here, switching
                    for (int z = x; z < inheritance.size(); z++) {
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

        uint32_t count = GetFieldCount();

        for (int x = 0; x < inheritance.size(); x++) {
            auto& entry = inheritance[x];
            //logger::info("before {} {} {}", entry.memberIndex, count, entry.type->GetHierarchyData()->members.size());

            entry.memberIndex = count;
            count += (uint32_t)entry.tree->GetFieldCount();

            //logger::info("after {} {} {}", entry.memberIndex, count, entry.type->GetHierarchyData()->members.size());
        }
    }

    std::vector<InheritNode> InheritanceTree::GetInheritFrom(uint32_t hashMin, uint32_t idxInc) const
    {
        //This should include access type, and who it's from. This way internal inheritance can be better handled.

        std::vector<InheritNode> result;

        result.reserve(inheritance.size() + 1);

        InheritNode& self = result.emplace_back();

        //uint32_t largestHash = hashMin;

        self.tree = unconst(this);

        self.startHash = hashMin;

        self.access = Access::Public;

        //self.memberIndex = members

        self.distance = 1;// +(self.type->GetName() == "Object");

        //We'll want to make a copy, because we're gonna rehash it by the index.
        for (InheritNode data : inheritance) {
            
            if (!data.IsVirtualInherited() || data.HasZeroWidthHash())
                data.startHash += hashMin;
            
            data.ownerIndex += idxInc;
            //I still have to check against this due to virtual inheritance.
            //if (largestHash < data.hash[1])
            //	largestHash = data.hash[1];

            switch (data.access) {
            case Access::Private:
                data.access = Access::None;
                break;
            case Access::InternalPrivate:
                data.access = Access::Internal;
                break;
            }

            data.distance++;
            result.push_back(data);
        }

        //self.hash[1] = largestHash;

        return result;
    }


    void InheritanceTree::SetInheritFrom(IHierarchyTree* other, Access a_access, bool post_affixed)
    {
        uint32_t idxInc = (uint32_t)inheritance.size() + 1;

        std::vector<InheritNode> inherits = other->GetInheritFrom(hashRange + 1, idxInc);

        bool second = false;

        //Later
        Access access = a_access & ~Access::Internal;


        for (auto& data : inherits) {
            bool _second = second;
            second = true;

            if (data.tree == this) {
                report::compile::error("Type '{}' cannot inherit from itself", GetName());
            }

            InheritNode* prev_data = FindInheritNode(data.tree);

            if (prev_data && prev_data->distance == 1 && prev_data->ownerIndex == 1 && data.distance == 1 && data.ownerIndex == 0) {
                report::compile::error("Type '{}' inherits {} directly multiple times.", GetName(), prev_data->tree->GetName());
            }




            if (data.GetAccess() > access) {
                data.SetAccess(access);
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

                    //This might only check if one is inherited as such.
                    prev_data->SetVirtuallyInherited(!both_affixed);
                    //^This will ensure literally anything will count toward this, only until it's manually inherited. This may fuck
                    // with entries that aren't compatible with it, but this shit is bottom of the barrel in terms of coversion, it's the least prefered.

                    prev_data->ownerIndex = 0;

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



            if (auto range = data.hash_range(); range > hashRange)
                hashRange = range;

            auto old = fieldRange;

            data.memberIndex = old;  //This is the place where this starts.
            fieldRange += (uint32_t)data.tree->GetFieldCount();


            inheritance.push_back(data);
        }
    }



    void InheritanceTree::SetDerivesTo(IHierarchyTree* other, SyntaxRecord& record, Access a_access)
    {
        CheckDeriveFrom(other, record);

        if (this == other)
            record.critical<IssueType::Fault>("Type '{}' cannot inherit from itself", GetName());

        HandleInheritance();

        return SetInheritFrom(other, a_access);
    }



    OverloadCode InheritanceTree::CreateCode(const IHierarchyTree* target)
    {  //Change name to generate code probably more like.

        auto data = !target || target == this ? nullptr : FindInheritNode(target);

        if (data)
            return { data->hash(), data->distance, !data->IsVirtualInherited() };
        else
            return { { 0, hashRange }, 0, true };
    }



    void InheritanceTree::PrintInheritance()
    {
        std::string name;

        if (auto type = GetHierarchyType()) {
            name = type->GetName();
        }

        logger::trace("Class: {}: Ranged: (0/{}), Members: ({}/{})", name, hashRange, fieldRange, GetFieldCount());

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

            case Access::InternalPublic:
                access = "public internal";
                break;
            case Access::InternalProtected:
                access = "protected internal";
                break;
            case Access::InternalPrivate:
                access = "private internal";
                break;
            case Access::Internal:
                access = "internal";
                break;
            }

            auto hash = basis.hash();

            //I'm going to leave this because it needs a source file
            logger::trace("|	Name: {}, Hash: {}/{}, true: {}, Dist: {}, Ownr: {}, intern: {}, access: {}, Mbrs: ({}+{}), postfix: {}, nil: {}",
                basis.tree->GetName(), hash[0], hash[1], basis.startHash, basis.distance, basis.ownerIndex, basis.IsInternal(),
                access, basis.memberIndex, basis.tree->GetFieldCount(), basis.IsAffixed(), basis.HasZeroWidthHash());
        }
    }



    /////////////////////


//This should be moved to qualified type, and the main thing of desire here should be the overload code.
    int InheritanceTree::CompareType(OverloadCode& left, OverloadCode& right, QualifiedType&& left_type, QualifiedType&& right_type)
    {
        //This should be using OverloadCode, but i'd need a given type to do it.

        if (!left.initialized && !right.initialized) {
            left = left.FinalizeOld(left_type->GetHierarchyTree(), right_type->GetHierarchyTree());
            right = right.FinalizeOld(right_type->GetHierarchyTree(), left_type->GetHierarchyTree());

            //OverloadEntry left = a_lhs.FinalizeOld(a_rhs.type);
            //OverloadEntry right = a_rhs.FinalizeOld(a_lhs.type);
        }

        logger::trace("left({}/{}), right({}/{})", left.hash[0], left.hash[1], right.hash[0], right.hash[1]);

        if (left.hash[0] <= right.hash[1] && left.hash[1] >= right.hash[0]) {
            return left.distance - right.distance;
        }
        if (left.hash[0] >= right.hash[1] && left.hash[1] <= right.hash[0]) {
            return left.distance - right.distance;
        }

        return 0;
    }

    int InheritanceTree::CompareType(ITypeInfo* a_lhs, ITypeInfo* a_rhs)
    {
        OverloadCode left = CreateCode((a_lhs, nullptr));
        OverloadCode right = CreateCode((a_rhs, nullptr));

        return CompareType(left, right, QualifiedType{ a_lhs }, QualifiedType{ a_rhs });
    }


	std::vector<IHierarchyTree*> InheritanceTree::GetPostAffixedTypes() const
	{
		return { common_type::object()->GetHierarchyTree() };
	}

}