#pragma once

namespace LEX
{
    struct Info;
    struct TypeInfo;
    struct IComponent;
    struct ScriptObject;
    struct AttributeData;
    struct AttributeOwner;
    struct AttributeBase;
    struct Variable;
    struct RuntimeVariable;
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
                virtual bool GetValue(const std::string_view & name, RuntimeVariable& value) =0;
                virtual bool SetValue(const std::string_view & name, const RuntimeVariable& value) = 0;
            INTERNAL:

                virtual bool CanAllowAttach(AttributeOwner* owner) = 0;
                virtual bool CanAttachedAllow(AttributeOwner* owner, AttributeBase* attribute) = 0;

                virtual void OnTargetValidated() = 0;

            };
        }

        CURRENT_VERSION(IAttribute, 1);
    }

    struct IMPL_VERSION(IAttribute)
    {
        bool GetValue(const std::string_view& name, RuntimeVariable& value) override { return false; }
        bool SetValue(const std::string_view& name, const RuntimeVariable& value) override { return false; }
        bool CanAllowAttach(AttributeOwner* owner) override { return true; }
        bool CanAttachedAllow(AttributeOwner* owner, AttributeBase* attribute) override { return true; }
    };
}