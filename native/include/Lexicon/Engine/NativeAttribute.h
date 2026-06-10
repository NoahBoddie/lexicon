#pragma once

#include "Lexicon/AttributeData.h"
#include "Lexicon/Engine/Attribute.h"

namespace LEX
{
    struct NativeAttribute : public Attribute
    {
        NativeAttribute(TypeInfo* type, std::unique_ptr<AttributeData>&& data) : _data{ std::move(data) }
        {
            _data->_self = this;
            _type = type;
        }



        ScriptObject* GetScriptObject() override { return nullptr; }
        
        AttributeData* GetNativeData() override
        {
            return _data.get();
        }
   
        //I'm thinking that these can only 

        void OnTargetValidated() override { return _data->OnTargetValidated(); }

        std::unique_ptr<AttributeData> _data = nullptr;
    };


}