#pragma once

#include "Lexicon/Variable.h"

namespace LEX
{

    struct RefVariable
    {


    public:


        ~RefVariable() { Unhandle(); }

    protected:
        RefVariable(const Variable* var) : _var{ unconst(var) }
        {
            _var->ModRefCount(true);
        }
    public:

        RefVariable(const std::reference_wrapper<Variable>& var) : RefVariable{ &var.get() } {}

        RefVariable(const Variable& var) : RefVariable{ std::addressof(var) } {}



        RefVariable(const RefVariable& other)
        {
            Transfer(other);
        }



        RefVariable& operator=(const RefVariable& other)
        {
            CheckUnhandle(other);

            Transfer(other);
            return *this;
        }
        constexpr operator bool() const noexcept
        {
            return _var;
        }

        constexpr Variable* get() noexcept
        {
            return _var;
        }

        constexpr const Variable* get() const noexcept
        {
            return _var;
        }

        constexpr Variable& ref() noexcept
        {
            return *get();
        }

        constexpr const Variable& ref() const noexcept
        {
            return *get();
        }


        Variable* operator->() noexcept
        {
            return _var;
        }

        const Variable* operator->() const noexcept
        {
            return _var;
        }

        operator Variable&()
        {
            return ref();
        }

        operator const Variable&() const
        {
            return ref();
        }


        void Clear()
        {
            Unhandle();
            _var = nullptr;
        }
    private:

        void CheckUnhandle(const RefVariable& other)
        {
            if (_var != other._var)
                Unhandle();
        }

        void Transfer(const RefVariable& other)
        {
            if (_var = other._var) {
                _var->ModRefCount(true);
            }
        }

        void Unhandle()
        {
            if (_var) {
                _var->ModRefCount(false);
            }
        }

    protected:

        //This should be created the moment it comes into existence
        mutable Variable* _var = nullptr;
    };
    REQUIRED_SIZE(RefVariable, 0x8);


    inline RefVariable detach(Variable&& var)
    {
        return RefVariable{ *new Variable{ ctrl::detached, std::move(var) } };
    }

    inline RefVariable detach(const Variable& var)
    {
        return RefVariable{ *new Variable{ ctrl::detached, var } };
    }



}