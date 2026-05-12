#pragma once

#include "Lexicon/Specifier.h"

namespace LEX
{
    struct ITypeInfo;
    struct IHierarchyTree;

    ENUM(InheritFlag, uint8_t)
    {
        None = 0,
        IsGeneric = 1 << 0,
        IsPostAffixed = 1 << 1,
        AllowsVirtual = 1 << 2,
        NilHash = 1 << 3,
    };




    struct InheritNode
    {
        static constexpr uint32_t virtual_pos = -1;
        //Allow this to be converted to InheritanceTree. This way I can visit both inherit nodes and trees

        //The hash is a value that represents memory wise, where said object can be considered within memory.

        IHierarchyTree* tree = nullptr;


        //This can possibly be unionized, with the sign bit being able to tell if one or the other.
        uint32_t ownerIndex = 0;

        uint32_t memberIndex = 0;

        //New hash concept, hash will either be represented with the start hash plus the range the tree has
        // or 0 and -1 if that's what the start has is.
        uint32_t startHash{};

        //Distance is -1 if virtually inherited
        uint16_t distance = 0;

        Access access;//

        InheritFlag flags = InheritFlag::None;



        //2 free bytes left.



        constexpr bool operator<(const InheritNode& other) const
        {
            auto g_self = IsGeneric();
            auto g_other = other.IsGeneric();

            if (g_self != g_other)
                return !g_self < !g_other;

            return distance < other.distance;
        }

        //Need to figure out how to move these.

        //These are deprecated, as methods
        //uint32_t memberRange [2];//This should be where this entries stuff starts, and then where it ends. same deal with the other.
        //uint32_t methodRange [2];




        std::array<uint32_t, 2> hash();

        ITypeInfo* type() const;

        uint32_t hash_range()
        {
            return hash()[1];
        }


        operator IHierarchyTree* ()
        {
            return tree;
        }

        void SetFlag(InheritFlag flag, bool v)
        {
            if (v) {
                flags |= flag;
            }
            else {
                flags &= ~flag;
            }
        }

        constexpr bool HasFlag(InheritFlag flag) const noexcept
        {
            return flags & flag;
        }


        constexpr bool IsGeneric() const noexcept
        {
            return flags & InheritFlag::IsGeneric;
        }

        Access GetAccess() const
        {
            return access & ~Access::Internal;
        }

        void SetAccess(Access a_acc)
        {
            auto pop = access & Access::Internal;
            a_acc &= ~Access::Internal;
            access = a_acc;
            access |= pop;
        }

        //I want to merge a bunch of flags into where access is. Stuff like is generic or is internal, virtual inherited all can probably be shoved in there.

        void SetInternal(bool v)
        {
            if (v) {
                access |= Access::Internal;
            }
            else {
                access &= ~Access::Internal;
            }
        }

        void SetAffixed(bool v)
        {
            return SetFlag(InheritFlag::IsPostAffixed, v);
        }

        bool IsAffixed() const
        {
            return HasFlag(InheritFlag::IsPostAffixed);
        }


        void SetVirtuallyInherited(bool nil_hash) {
            startHash = virtual_pos;
            SetNilHash(nil_hash);
        }

        void SetNilHash(bool v)
        {
            return SetFlag(InheritFlag::NilHash, v);
        }

        bool HasNilHash() const
        {
            return HasFlag(InheritFlag::NilHash);
        }

        bool IsInternal() const
        {
            return access & Access::Internal;
        }

        bool IsVirtualInherited() const
        {
            //This is not how this works.
            return startHash == virtual_pos;
        }

    };


}