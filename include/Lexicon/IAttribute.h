#pragma once

namespace LEX
{
    struct Info;
    struct TypeInfo;
    struct IComponent;
    struct ScriptObject;
    struct AttributeData;
    struct AttributeOwner;
    namespace Version
    {
        namespace _1
        {
            struct INTERFACE_VERSION(IAttribute)
            {
                virtual std::string_view GetName() const = 0;
                virtual TypeInfo* GetType() = 0;
                virtual AttributeOwner* GetParent() = 0;
                virtual ScriptObject* GetScriptObject() = 0;
                virtual AttributeData* GetNativeData() = 0;

            INTERNAL:
                virtual void OnTargetValidated() = 0;

            };
        }

        CURRENT_VERSION(IAttribute, 1);
    }

    struct IMPL_VERSION(IAttribute)
    {

    };
}