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


        bool GetValue(const std::string_view& name, RuntimeVariable& value) override
        { 
            return _data->GetValue(name, value);
        }
        bool SetValue(const std::string_view& name, const RuntimeVariable& value) override
        { 
            return _data->SetValue(name, value);
        }
        bool CanAllowAttach(AttributeOwner* owner) override 
        { 
            return _data->CanAllowAttach(owner);
        }
        bool CanAttachedAllow(AttributeOwner* owner, AttributeBase* attribute) override 
        { 
            return _data->CanAttachedAllow(owner, attribute);
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