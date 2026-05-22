#pragma once

namespace LEX
{
    struct Function;
    struct ITypeInfo;
    struct MemberData;
    struct InstanceID;
    struct InheritNode;
    struct OverloadCode;
    struct MemberPointer;
    struct ITemplateBody;
    struct IHierarchyTree
    {
        virtual std::string_view GetName() const = 0;

        virtual ITypeInfo* GetHierarchyType() = 0;

        //No longer needs to be virtual
        


        virtual InheritNode GetInheritNode(const IHierarchyTree* tree) const = 0;
        
        virtual size_t GetInheritIndex(const IHierarchyTree* tree) const = 0;
       
        virtual bool IsAbstract() const noexcept { return true; }


        virtual std::vector<InheritNode> GetInheritFrom(uint32_t hashMin, uint32_t idxInc) const = 0;

        virtual OverloadCode CreateCode(const IHierarchyTree* target) = 0;

        virtual InstanceID GetInstanceID() const = 0;

        virtual size_t GetHashRange() const = 0;
        virtual size_t GetFieldRange() const = 0;//TODO: should be FieldRange

        virtual MemberData* GetMembers() = 0;

        virtual Function* GetMethod(MemberPointer ptr, ITemplateBody* body) = 0;
        virtual size_t GetField(MemberPointer ptr, ITemplateBody* body) = 0;


        size_t GetFieldCount();

        bool DerivesFrom(const IHierarchyTree* other);
    };

}