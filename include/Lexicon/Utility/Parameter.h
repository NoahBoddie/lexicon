#pragma once

#include "Lexicon/TypeInfo.h"
#include "Lexicon/Variable.h"
#include "Lexicon/Interfaces/DirectoryManager.h"

namespace LEX
{
    namespace detail
    {
        template <StringLiteral Name>
        struct ParameterBase
        {
            static constexpr std::string_view type_name = Name;
            inline static TypeInfo* type = nullptr;


            static TypeInfo* GetStaticType()
            {
                if (!type) {
                    //Use project manager to get the type
                    type = DirectoryManager::instance->GetComponentFromPath(nullptr, type_name, ComponentType::TypeInfo)->As<TypeInfo>();
                }
                return type;
            }

            virtual TypeInfo* GetType() const = 0;


            static TypeInfo* GetVariableType(const ParameterBase* a_this)
            {
                TypeInfo* result = nullptr;

                if (a_this) {
                    result = a_this->GetType();
                }

                TypeInfo* static_type = GetStaticType();

#ifndef NDEBUG
                if (result) {
                    assert(static_type->CanConvert(result));
                }
#endif

                if (!result) {
                    result = static_type;
                }

                return result;
            }

        };
    }

    template <StringLiteral Name, typename T = Variable>
    struct Parameter : public detail::ParameterBase<Name>
    {
        using ParameterType = Parameter<Name, T>;

        template <typename... Args>
        Parameter(Args&&... args) requires (std::is_constructible_v<T, Args&...>)
            : value{ args... }
        {
        }


        operator T& ()
        {
            return value;
        }


        auto operator->()
        {
            //Couldn't think of an easier way to do this.
            if constexpr (!std::is_same_v<std::pointer_traits<T>::element_type, T>) {
                return std::to_address(value);
            }
            else {
                return std::addressof(value);
            }
        }

        TypeInfo* GetType() const override
        {
            return GetVariableType(value);
        }

        T value;

    };

    namespace Util
    {
        template <StringLiteral Name>
        using Attribute = Parameter<Name, AttributeBase*>;

        template <StringLiteral Name>
        using Struct = Parameter<Name, ScriptObject*>;

        template <StringLiteral Name>
        using Class = Parameter<Name, ScriptObject*>;

        template <StringLiteral Name>
        using Interface = Parameter<Name, Variable>;
    }

}