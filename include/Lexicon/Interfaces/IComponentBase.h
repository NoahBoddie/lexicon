#pragma once

#include "Lexicon/ComponentType.h"
#include "Lexicon/Impl/ComponentDetails.h"
#include "Lexicon/Interfaces/IdentityManager.h"
#include "Lexicon/Interfaces/AttributeOwner.h"
namespace LEX
{
    struct TypeInfo;
    struct Component;
    struct IComponent;


    namespace Version
    {
        namespace _1
        {
            struct INTERFACE_VERSION_DERIVES(IComponentBase, AttributeOwner)
            {
            protected:
                virtual const Component* GetComponent() const = 0;
                virtual const IComponent* GetComponentBase() const = 0;
                virtual const void* CastComponent(const void* self, ComponentType from, ComponentType to) const = 0;

            public:
                virtual std::string_view GetName() const = 0;

                virtual uint16_t GetComponentOffset() const = 0;
                
                virtual ComponentType GetComponentType() const = 0;
            };
        
            #define DEF_FUNC_IMPL_COMPONENT_1 \
            MAP_UD(DEF_USING_IMPL,Component,GetComponent,GetComponentOffset,GetComponentType,GetName)
        }

        CURRENT_VERSION(IComponentBase, 1);
    }


    //The component bases are really never looked at, so to remedy this, I'll do 2 things.
    // Redeclare all the functions and simply refer back to a previous version
    struct IMPL_VERSION(IComponentBase)
    {
    public:
        static constexpr auto COMPONENT_TYPE = ComponentType::Invalid;

        //TODO: Change name to GetComponent, as this object may not actually be the component.
        virtual const Component* GetComponent() const = 0;
        Component* GetComponent() { return unconst(make_const(this)->GetComponent()); }

        const IComponent* GetComponentBase() const override;
        IComponent* GetComponentBase() { return unconst(make_const(this)->GetComponentBase()); }

    private:
        //The way this would work is it would link to an engine file that handles the traits. 
        // This would make it so we wouldn't need to actually need to have the include the files to be
        // able to cast to it (or rather not ALL files at once, just the one we're casting to, so the normal rules)
        const void* CastComponent(const void* self, ComponentType from, ComponentType to) const override INTERFACE_FUNCTION;
        
        void* CastComponent(const void* self, ComponentType from, ComponentType to) { return unconst(make_const(this)->CastComponent(self, from, to)); }
        
        //Defined in Impl/ComponentDetails.cpp
        static TypeInfo* GetTypeFromOffset(uint16_t offset);


        bool IsType(Type type) const noexcept override;

    public:
        std::string GetFullName() const;

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
#define AS_COMPONENT(mc_component) As<::LEX::mc_component>(::LEX::ComponentType::mc_component)

            using To = copy_cv_t<Self, T>;
            if (Self* ptr = std::addressof(a_this)) {

                if constexpr (std::is_convertible_v<const std::remove_cvref_t<Self>*, const T*>)
                {
                    return static_cast<To*>(ptr);
                }
                else {


                    if constexpr (requires() { sizeof(T) > 0; } && requires() { { T::COMPONENT_TYPE } -> std::convertible_to<ComponentType>; })
                    {
                        if (T::COMPONENT_TYPE != type) {
                            report::fault::critical("Incorrect type submitted for casting, expected {}, recieved {}",
                                magic_enum::enum_name(T::COMPONENT_TYPE), magic_enum::enum_name(type));
                        }
                    }


                    return reinterpret_cast<To*>(ptr->CastComponent(ptr, std::remove_cvref_t<Self>::COMPONENT_TYPE, type));
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
        copy_cv_t<Self, T>* As(this Self& a_this) requires(requires() { { T::COMPONENT_TYPE } -> std::convertible_to<ComponentType>; })
        {
            return a_this.As<T>(T::COMPONENT_TYPE);
        }

        template<typename T, typename Self, typename = std::enable_if_t<
            std::negation_v<
            std::disjunction<
            std::is_pointer<T>,
            std::is_reference<T>,
            std::is_const<T>,
            std::is_volatile<T>>>>>
            copy_cv_t<Self, T>* GetAs(this Self& a_this, ComponentType type)
        {
#define GET_AS_COMPONENT(mc_component) GetAs<::LEX::mc_component>(::LEX::ComponentType::mc_component)            
            if (Self* ptr = std::addressof(a_this)) {
                return ptr->GetComponentBase()->As<T>(type);
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
            copy_cv_t<Self, T>* GetAs(this Self& a_this) requires(requires() { { T::COMPONENT_TYPE } -> std::convertible_to<ComponentType>; })
        {
            return a_this.GetAs<T>(T::COMPONENT_TYPE);
        }
        template<typename T, typename Self, typename = std::enable_if_t<
            std::negation_v<
            std::disjunction<
            std::is_pointer<T>,
            std::is_reference<T>,
            std::is_const<T>,
            std::is_volatile<T>>>>>
        bool Is(this Self& a_this) requires(requires() { { T::COMPONENT_TYPE } -> std::convertible_to<ComponentType>; })
        {
            return a_this.As<T>();
        }


        template<typename Self>
        bool Is(this Self& a_this, ComponentType type)
        {
            if (Self* ptr = std::addressof(a_this)) {
                return ptr->CastComponent(ptr, std::remove_cvref_t<Self>::COMPONENT_TYPE, type);
            }
            return false;
        }

       

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
    MAP_UD(DEF_USING_IMPL,Component,GetVariableType,As,GetAs,Is,GetFullName)



#define DECL_IMPL_FUNC_COMPONENT DEF_FUNC_IMPL_COMPONENT_MAIN DEF_FUNC_IMPL_COMPONENT_1



}

