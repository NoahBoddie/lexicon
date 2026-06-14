#pragma once

namespace LEX
{
    struct Info;
    struct IComponent;
    struct AttributeBase;

    struct AttributeOwner : public Interface
    {
    protected:

        enum Type
        {
            kInfo,
            kIComponent,
            kTotal,
        };

        virtual void* AsType(Type type);

        virtual bool IsType(Type type) const noexcept;

    public:

        virtual std::span<AttributeBase*> GetAttributes() { return {}; }


        template<typename T>
        T* AsOwner()
        {
            return nullptr;
        }

        template<typename T>
        bool IsOwner() const noexcept
        {
            return false;
        }

#define HANDLE_OWNER_TYPE(mc_type)                                                                  \
            template<>                                                                              \
            mc_type* AsOwner<mc_type>()                                                             \
            {                                                                                       \
                return reinterpret_cast<mc_type*>(this ? AsType(CONCAT(k,mc_type)) : nullptr);      \
            }                                                                                       \
            template<>                                                                              \
            bool IsOwner<mc_type>() const noexcept                                                  \
            {                                                                                       \
                return this ? IsType(CONCAT(k,mc_type) ) : false;                                   \
            }   

        HANDLE_OWNER_TYPE(Info)
        HANDLE_OWNER_TYPE(IComponent)

#undef HANDLE_OWNER_TYPE

        template<typename T>
        const T* AsOwner() const
        {
            return unconst(this)->AsOwner<T>();
        }

    };

}