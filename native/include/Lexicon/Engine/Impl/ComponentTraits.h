#pragma once

#include "Lexicon/ComponentType.h"

#include "Lexicon/Interfaces/IComponent.h"
#include "Lexicon/Interfaces/IElement.h"
#include "Lexicon/Interfaces/IDirectory.h"
#include "Lexicon/Interfaces/IEnvironment.h"
#include "Lexicon/Interfaces/IScript.h"
#include "Lexicon/Interfaces/IProject.h"
#include "Lexicon/ITypeInfo.h"
#include "Lexicon/IFunction.h"
#include "Lexicon/IGlobal.h"
#include "Lexicon/TypeInfo.h"
#include "Lexicon/Function.h"
#include "Lexicon/Global.h"

#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/Component.h"
#include "Lexicon/Engine/Directory.h"
#include "Lexicon/Engine/Environment.h"
#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"

#include "Lexicon/Engine/TypeBase.h"
#include "Lexicon/Engine/ConcreteType.h"
#include "Lexicon/Engine/GenericType.h"
#include "Lexicon/Engine/SpecialType.h"

#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/Engine/ConcreteFunction.h"
#include "Lexicon/Engine/GenericFunction.h"
#include "Lexicon/Engine/SpecialFunction.h"

#include "Lexicon/Engine/GlobalBase.h"
#include "Lexicon/Engine/ConcreteGlobal.h"
//#include "Lexicon/Engine/GenericGlobal.h"
//#include "Lexicon/Engine/SpecialGlobal.h"


//Give it everything.

namespace LEX
{
#define LHS_COMPONENT_TRAITS(mc_type) \
    case mc_type::COMPONENT_TYPE:{\
        return LeftComponentCast<mc_type>(self, to);\
    }

#define RHS_COMPONENT_TRAITS(mc_type) \
    case mc_type::COMPONENT_TYPE:{\
        return RightComponentCast<T, mc_type>(self);\
    }

#define COMPONENT_TRAITS(mc_type) \
    case mc_type::COMPONENT_TYPE:\
        if constexpr (std::is_convertible_v<const mc_type*, const From*> &&\
            std::is_convertible_v<const mc_type*, const To*>){\
            return static_cast<const mc_type*>(self);\
        }\
        break;
 
    
#define COMPONENT_NAMES IComponent, Component, IElement, Element, IDirectory, Directory, IEnvironment, Environment, IScript, Script, IProject, Project, ITypeInfo, TypeInfo, TypeBase, ConcreteType, GenericType, SpecialType, IFunction, Function, FunctionBase, ConcreteFunction, GenericFunction, SpecialFunction, IGlobal, Global, GlobalBase, ConcreteGlobal 

/*GenericGlobal, SpecialGlobal*/



    template <typename From, typename To>
    inline const To* RightComponentCast(const From* self)
    {
        ComponentType type;

        if constexpr (requires(const From* it) { { it->GetComponentType() } ->std::convertible_to<ComponentType>; })
        {
            type = self->GetComponentType();
        }
        else
        {
            //This roughly should never actually happe I think, and if it's reached it should scream.
            type = From::COMPONENT_TYPE;
        }

        switch (type)
        {
            //MAP(COMPONENT_TRAITS, COMPONENT_NAMES);
            MAP(COMPONENT_TRAITS, COMPONENT_NAMES);

        default:
            report::fault::critical("Unknown ComponentType {} detected", magic_enum::enum_name(type));
            break;
        }

        return nullptr;
    }

    template<typename T>
    inline const void* LeftComponentCast(const void* ptr, ComponentType to)
    {
        const T* self = reinterpret_cast<const T*>(ptr);

        switch (to)
        {
            MAP(RHS_COMPONENT_TRAITS, COMPONENT_NAMES);

        default:
            report::fault::critical("Unknown ComponentType {} detected", magic_enum::enum_name(to));
            break;
        }

        return nullptr;
    }

    //From in this case is the pointer it's being percieved as, to is the goal
    const void* ComponentCast(const void* self, ComponentType from, ComponentType to)
    {
        if (self)
        {
            switch (from)
            {
                MAP(LHS_COMPONENT_TRAITS, COMPONENT_NAMES);

            default:
                report::fault::critical("Unknown ComponentType {} detected", magic_enum::enum_name(from));
                break;
            }
        }
        return nullptr;
    }
}

#undef LHS_COMPONENT_TRAITS
#undef RHS_COMPONENT_TRAITS
#undef COMPONENT_TRAITS
#undef COMPONENT_NAMES
