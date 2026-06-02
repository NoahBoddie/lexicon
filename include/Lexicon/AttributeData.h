#pragma once

#include "Lexicon/IAttribute.h"

//*src

#include "Lexicon/AttributeBase.h"
namespace LEX
{
    struct AttributeBase;
    struct AttributeData;

    using AttrDataBuilder = std::unique_ptr<AttributeData>(*)();
    using AttributeBuilder = std::unique_ptr<AttributeData>(*)();


    struct AttributeData : public IAttribute
    {
        //what data would this at base need to own? I'm thinking data of its own parentage. Probably
        // just its real self, so I can ask questions like, what's next, what's my parent.
    private:

        friend struct NativeAttribute;

    public:

        template <std::derived_from<AttributeData> T>
        std::unique_ptr<AttributeData> Create() requires(std::is_default_constructible_v<T>)
        {
            return std::unique_ptr<AttributeData>{ new T};
        }



        TypeInfo* GetType() override final
        {
            return _self->GetType();
        }

        AttributeBase* self() noexcept
        {
            return _self;
        }


    INTERNAL:
        //This basically only gets set 
        AttributeBase* _self = nullptr;
    };

}