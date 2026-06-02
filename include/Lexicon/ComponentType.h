#pragma once

namespace LEX
{
    ENUM(ComponentType, uint16_t)
    {
        Invalid = static_cast<uint16_t>(-1),

        Component = 0,
        Element,
        Environment,
        Directory,
        Repository,
        Project,
        Subdirectory,
        Script,
        IFunction,
        Function,
        ITypeInfo,
        TypeInfo,
        IGlobal,
        Global,
        IFormula,
        kScriptedMax,

                
        IComponent,
        IElement,
        IDirectory,
        IRepository,
        IEnvironment,
        IScript,
        IProject,
        ISubdirectory,
        
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
        BasicFormula,

        Utilities = 10000,
        Constructor,
        Operator,

        kTotal,
    };

}