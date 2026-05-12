#pragma once

#include "Lexicon/Engine/InheritNode.h"
#include "Lexicon/Engine/HierarchyTree.h"

//*src
#include "Lexicon/Engine/MemberData.h"


namespace LEX
{
    struct Function;
	struct ITypeInfo;
	struct QualifiedType;
	struct OverloadCode;


    struct InheritanceTree : public HierarchyTree
    {
        ~InheritanceTree()
        {
            if (_members)
                delete _members;
        }

        MemberData* _members = nullptr;


        uint32_t hashRange = 0;//Range is equal to zero to this number.

        uint32_t fieldRange = 0;//Should bind classes increase this value any? Nah, probably handle in post.


        size_t GetHashRange() const override
        {
            return hashRange;
        }
        size_t GetFieldRange() const override
        {
            return fieldRange;
        }

        MemberData* GetMembers() override
        {
            return _members;
        }




        bool VisitLowerTrees(auto func)
        {
            for (auto& inherit : inheritance)
            {
                if (func(inherit) == true)
                    return true;
            }


            //for (auto& inherit : inheritance)
            //{
            //    InheritIterator it{ inherit, prev };
            //    if (inherit.tree->VisitLowerTrees(func, &it) == true)
            //        return true;
            //}

            return false;
        }




        bool VisitTrees(auto func)
        {
            if (func(this) == true)
                return true;

            return VisitLowerTrees(func);
        }


        Function* GetMethod(MemberPointer ptr, ITemplateBody* body) override;

        size_t GetField(MemberPointer ptr, ITemplateBody* body) override;


        /////////////////////////////////////////////



        //TODO: This should come back at a later point I think.
        virtual void HandleInheritance() {}



        InheritNode GetInheritNode(const IHierarchyTree* tree) const override
        {

            const InheritNode* result = FindInheritNode(tree);

            if (result)
                return *result;

            return {};
        }


        const InheritNode* FindInheritNode(const IHierarchyTree* tree) const;

        InheritNode* FindInheritNode(const IHierarchyTree* tree)
        {
            return unconst(make_const(this)->FindInheritNode(tree));
        }

        //Should be hierarcy data when the change
        virtual std::vector<IHierarchyTree*> GetPostAffixedTypes() const;

        void FinalizeAndSort();

        std::vector<InheritNode> GetInheritFrom(uint32_t hashMin, uint32_t idxInc) const override;


        void SetInheritFrom(IHierarchyTree* other, Access a_access = Access::Public, bool post_affixed = false);

        //One of the sole functions that will remain the same, only triggered when inheritance
        virtual void CheckDeriveFrom(IHierarchyTree* other) {};



        void SetDerivesTo(IHierarchyTree* other, Access a_access = Access::Public);



        OverloadCode CreateCode(const IHierarchyTree* target) override;



        void PrintInheritance();



        /////////////////////


    //This should be moved to qualified type, and the main thing of desire here should be the overload code.
        int CompareType(OverloadCode& left, OverloadCode& right, QualifiedType&& left_type, QualifiedType&& right_type);

        int CompareType(ITypeInfo* a_lhs, ITypeInfo* a_rhs);
    };


}