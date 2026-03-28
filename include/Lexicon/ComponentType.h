#pragma once

namespace LEX
{
    ENUM(ComponentType, uint16_t)
    {
        Invalid = -1,
        
        Component,
        Element,
        Environment,
        Directory,
        Project,
        Script,
        IFunction,
        Function,
        ITypeInfo,
        TypeInfo,
        IGlobal,
        Global,

        kScriptedMax,

                
        IComponent,
        IElement,
        IDirectory,
        IEnvironment,
        IScript,
        IProject,
        //IRepository,  //I'm unsure of the necessity of this type.

        FunctionBase,
        ConcreteFunction,
        GenericFunction,
        SpecialFunction,

        TypeBase,
        ConcreteType,
        GenericType,
        SpecialType,

        GlobalBase,
        ConcreteGlobal,
        GenericGlobal,
        SpecialGlobal,



       kTotal,
    };

}