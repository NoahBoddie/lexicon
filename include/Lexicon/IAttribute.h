#pragma once

namespace LEX
{
    struct Info;
    struct TypeInfo;
    struct IComponent;
    struct ScriptObject;

    namespace Version
    {
        namespace _1
        {
            struct INTERFACE_VERSION(IAttribute)
            {
                virtual TypeInfo* GetType() = 0;
                //I'd prefer to make this the IComponentBase that owns it.
                virtual Info* GetInfoParent() = 0;
                virtual IComponent* GetCompParent() = 0;
                virtual ScriptObject* GetScriptObject() = 0;

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