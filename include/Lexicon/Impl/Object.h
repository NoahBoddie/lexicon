#pragma once


//src
#include "ObjectManager.h"

namespace LEX
{
    struct ObjectInterface;

    template <typename T>
    //using ArgContainer = std::span<T, std::dynamic_extent>;
    using ArgContainer = std::vector<T>;


    struct ScriptInterface;

    //TODO: Rename Object to ScriptInterface. Object is confusing with the general type of thing called any/object.
    struct Object {
        virtual ~Object() = default;

        //This is the core function of the object type. In it's implementation,
        virtual void Invoke(std::string_view func_name, Variable& ret, ArgContainer<Variable>& args)
        {
            //The above should be const.

            //Extra point should go toward notifying one that this literaly is unimplemented.
            // A specific function should be the thing that does this btw.
            "No implementation of [Put type name here]::Invoke exists. ";
            throw nullptr;

        }

        virtual constexpr std::string_view GetName()
        {
            //Returns the top level name of this polymorphic type. Can be changed to get the current name
            // in order to have a central version be the registered type.
            return typeid(std::remove_reference_t<decltype(*this)>).name();
        }

        //Need to make sure they're of the same type.
        virtual Object& operator=(const Object& a_rhs) = 0;

        virtual std::strong_ordering operator<=>(const Object& a_rhs) = 0;




        template <typename Self>
        constexpr std::string_view GetCurrentName(this Self&& self)
        {
            //The name of the type this was called at, regardless of what it inherits.
            return typeid(Self).name();
        }

        template <std::derived_from<Object> Self>
        constexpr Self& MatchType(this Self&& self, const Object& a_rhs)
        {
            //Also maybe use that type name implementation if you can. Just so different versions don't get caught on each other.

            if (_registry != a_rhs._registry)
            {
                //Types were not matched. Reporting failure.
                //Complete and utter failure.
            }

            return static_cast<Self&>(a_rhs);
        }


        Object()
        {
            
            _registry = ObjectManager::GetInterface(std::string{GetName()});

            //I'mma do this differently else where. Rather, I want to return an API result.
            if (!_registry) {
                "Object Type is unregistered or malformed. Confirm registration before use.";
                throw nullptr;
            }
        }

        //Basically, the object's job is to find the type.
        virtual AbstractTypePolicy* GetType() = 0;



        //This would be a virtual function that is required to be implemented by the derived class.
        //virtual TypeCode TypeCodeOf() = 0;

        ObjectInterface* _registry = nullptr;
    };

}