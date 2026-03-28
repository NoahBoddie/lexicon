#pragma once

#include "Lexicon/ComponentType.h"
#include "Lexicon/Impl/ComponentDetails.h"
#include "Lexicon/Interfaces/IdentityManager.h"
namespace LEX
{
    struct TypeInfo;
    struct Component;


    namespace Version
    {
        namespace _1
        {
            struct M_INTERFACE_VERSION(IComponentBase)
            {
            protected:
                virtual const Component* AsComponent() const = 0;
                virtual const void* Cast(const void* self, ComponentType from, ComponentType to) const = 0;

            public:

                virtual uint16_t GetComponentOffset() const = 0;
                
                virtual ComponentType GetComponentType() const = 0;
            };
        
            #define DEF_FUNC_IMPL_COMPONENT_1 \
            MAP_UD(DEF_USING_IMPL,Component,AsComponent,GetComponentOffset,GetComponentType)
        }

        CURRENT_VERSION(IComponentBase, 1);
    }


    //The component bases are really never looked at, so to remedy this, I'll do 2 things.
    // Redeclare all the functions and simply refer back to a previous version
    struct IMPL_VERSION(IComponentBase)
    {
    public:
        static constexpr auto COMPONENT_TYPE = ComponentType::Invalid;

        virtual const Component* AsComponent() const = 0;
        Component* AsComponent() { return unconst(make_const(this)->AsComponent()); }

    private:
        //The way this would work is it would link to an engine file that handles the traits. 
        // This would make it so we wouldn't need to actually need to have the include the files to be
        // able to cast to it (or rather not ALL files at once, just the one we're casting to, so the normal rules)
        const void* Cast(const void* self, ComponentType from, ComponentType to) const override final;

        void* Cast(const void* self, ComponentType from, ComponentType to) 
        {
            return unconst(make_const(this)->Cast(self, from, to));
        }
    public:


        //TODO: need to confirm this set up actually works on pointers.

        template<typename T, typename Self, typename = std::enable_if_t<
            std::negation_v<
            std::disjunction<
            std::is_pointer<T>,
            std::is_reference<T>,
            std::is_const<T>,
            std::is_volatile<T>>>>>
        copy_cv_t<Self, T>* As(this Self& a_this, ComponentType type)
        {
            using To = copy_cv_t<Self, T>;
            if (Self* ptr = std::addressof(a_this)) {

                if constexpr (std::is_convertible_v<const std::remove_cvref_t<Self>*, const T*>)
                {
                    return static_cast<To*>(ptr);
                }
                else {


                    if constexpr (requires() { sizeof(T) > 0; }&& requires() { { T::COMPONENT_TYPE } -> std::convertible_to<ComponentType>; })
                    {
                        if (T::COMPONENT_TYPE != type) {
                            report::fault::critical("Incorrect type submitted for casting, expected {}, recieved {}",
                                magic_enum::enum_name(T::COMPONENT_TYPE), magic_enum::enum_name(type));
                        }
                    }


                    return reinterpret_cast<To*>(a_this.Cast(ptr, std::remove_cvref_t<Self>::COMPONENT_TYPE, type));
                }
            }

            return nullptr;
        }

        template<typename T, typename Self, typename = std::enable_if_t<
            std::negation_v<
            std::disjunction<
            std::is_pointer<T>,
            std::is_reference<T>,
            std::is_const<T>,
            std::is_volatile<T>>>>>
        T* As(this Self& a_this) requires(requires() { { T::COMPONENT_TYPE } -> std::convertible_to<ComponentType>; })
        {
            return a_this.As<T>(T::COMPONENT_TYPE);
        }


        //Defined in Impl/ComponentDetails.cpp
        static TypeInfo* GetTypeFromOffset(uint16_t offset);


        template<std::derived_from<IComponentBase> Comp>
        static TypeInfo* GetVariableType(const Comp* comp)
        {
            //TODO: I would like to have the rest of this done elsewhere.
            uint16_t offset = comp ? comp->GetComponentOffset() : Comp::SCRIPT_TYPE;
            //ITypeInfo* type = IdentityManager::instance->GetTypeByOffset("REFLECT", offset);
            return GetTypeFromOffset(offset);
        }



    };



    #define DEF_FUNC_IMPL_COMPONENT_MAIN \
    MAP_UD(DEF_USING_IMPL,Component,GetVariableType,As)



#define DECL_IMPL_FUNC_COMPONENT DEF_FUNC_IMPL_COMPONENT_MAIN DEF_FUNC_IMPL_COMPONENT_1


}

