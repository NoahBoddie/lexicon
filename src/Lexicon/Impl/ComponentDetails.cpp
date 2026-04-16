#include "Lexicon/Impl/ComponentDetails.h"



#include "Lexicon/Interfaces/IComponentBase.h"
//This is where I'll move the functions that aren't interface in the other 2.
#include "Lexicon/ITypeInfo.h"

#include "Lexicon/Interfaces/IDirectory.h"
#include "Lexicon/Interfaces/IComponent.h"
#include "Lexicon/Interfaces/IProject.h"
namespace LEX
{
    TypeInfo* IComponentBase::GetTypeFromOffset(uint16_t offset)
    {
        ITypeInfo* type = IdentityManager::instance->GetTypeByOffset("REFLECT", offset);
        return NULL_OP(NULL_Q(type)->GetTypeInfo(nullptr));
    }

    std::string IComponentBase::GetFullName() const
    {
        auto a_this = unconst(GetAs<IComponent>());

        //*
        std::string result = std::string{ GetName() };

        if (IElement* as_elem = a_this->As<IElement>())
        {
            auto* directory = as_elem->GetParentInfc();

            while (directory)
            {
                result = std::format("{}::{}", directory->GetName(), result);

                if (directory->Is<IProject>() == false)
                    directory = directory->GetParentInfc();
                else
                    directory = nullptr;
            }
        }

        return result;
        //*/
    }

}