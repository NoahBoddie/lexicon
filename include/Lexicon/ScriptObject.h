#pragma once


//*src
#include "Lexicon/RuntimeVariable.h"

namespace LEX
{


    struct FieldData
    {
        using T = RuntimeVariable;

        T* data;

        ~FieldData()
        {
            Destroy();
        }

        bool TryPreserve()
        {
            //Check if any of the data has references, and if so move them to garbage collection
            return false;
        }

        void Destroy()
        {
            if (data) {
                if (TryPreserve() == false) {
                    delete[] data;
                    data = nullptr;
                }
            }
        }

        void Create(uint32_t size)
        {
            Destroy();

            data = new T[size];
        }

        void Transfer(T* other, uint32_t size, bool move = false)
        {
            for (int i = 0; i < size; i)
            {
                if (move) {
                    data[i] = std::move(other[i]);
                }
                else {
                    data[i] = other[i];
                }
            }
        }

        void Transfer(const T* other, uint32_t size)
        {
            return Transfer(unconst(other), size);
        }


        void Transfer(const void* other, uint32_t size)
        {
            //Shouldn't happen, this is just a dump overload.
        }
    };


    struct ScriptObject
    {
    public:
        enum Flag : uint8_t
        {
            kNone,
            kInitialized = 1 << 0,
            kDestructed = 1 << 1,
            kHasBindData = 1 << 2,    //
        };

    protected:
    public:

        union
        {
            //BindEntry basically holds onto the type, as well as the bind data. This moves the burden of the extra pointer,
            // and for non-bound objects it will decrease the size (of which there will be more of them).
            uintptr_t _raw{};
            BindEntry* _entry;
            TypeInfo* _type;//Static cast to AttributeType if attribute. or use as actually. Easier.

        };

        FieldData fieldList;

        //I will only store RuntimeVariables on these, I believe the extra cost is worth it,
        // primarily to simplify access and 


        Flag flags = kNone;
        //Move access memory to be a thread local system
        uint8_t bytes[3]{};
        StateID stateID{};//If the state ID is invalid, this means it will use the main bind


    public:

        ScriptObject(TypeInfo* type) : _type{ type }
        {
            fieldList.Create(type->GetFieldCount());
        }


        ~ScriptObject()
        {
            if (HasFlag(Flag::kDestructed) == false) {
                //Call destruct
            }
        }







        constexpr bool HasFlag(Flag flag) const noexcept
        {
            return flags & flag;
        }


        constexpr TypeInfo* type() const noexcept
        {

            if (HasFlag(Flag::kHasBindData) == true)
                return _entry->type;
            else
                return _type;
        }


        size_t size()
        {
            if (auto a_type = type()) {
                return a_type->GetFieldCount();
            }

            return 0;
        }

        void Instantiate(TypeInfo* self)
        {

        }



        bool GetMethod(const std::string_view& name, IFunction*& out)
        {
            return {};
        }

        bool GetMethod(MemberPointer member, IFunction*& out)
        {
            return {};
        }



        bool GetField(const std::string_view& name, RuntimeVariable& out)
        {
            return {};
        }

        bool GetField(MemberPointer member, RuntimeVariable& out)
        {
            return {};
        }

        bool IsRuntimeType() const
        {
            return false;
        }

        void Revert()
        {
            fieldList.Destroy();
            _type = nullptr;
            //size = 0;
        }

        //This gets complicated with bind objects
        void Transfer(const ScriptObject& other)
        {
            Revert();
            _type = other._type;
            //size = other.size;
            //lhs.Create(other.size);
            //lhs.Transfer(rhs.data, other.size);


        }


        //IDEA
        //Instead of the bind id being on everything, bind id will be used for stateIDs. Nah. 
        // this is a bad idea. I really would like to make some use out of this space though.





        ///I might use some extra flags for this, allowing it to easy denote things like having a bind class, or having a state at a later point.


        //I'm thinking this is how I'm going to handle this. A union that helps contro it being a variable pointer and a runtime pointer. I can then 
        // switch what type it's percieved as.

        //This might make it a pain however.


    };

}