#pragma once

namespace LEX
{
    ENUM(ComponentType, uint8_t)
    {
        IComponent,
            IElement,
            IDirectory,
            IEnvironment,
            //IRepository,  //I'm unsure of the necessity of this type.

            IScript,

            IProject,





            //The harder stuff, specializables
            IFunction,
            Function,


            ITypeInfo,
            TypeInfo,


            IGlobal,
            Global,


#ifdef LEX_SOURCE//Size manually adjusted to not cause overlap when id count grows.
                        
            Component = (255 / 2),
            Project,
            Directory,
            Script,
            Environment,
            Element,



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
#endif
    };

}