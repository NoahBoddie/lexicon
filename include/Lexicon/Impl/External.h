#pragma once


//src
#include "ExternalManager.h"

namespace LEX
{

    template <typename T>
    //using ArgContainer = std::span<T, std::dynamic_extent>;
    using ArgContainer = std::vector<T>;

    //using ExternID = std::pair<const std::type_info*, const std::type_info*>;

    struct ExternInfo
    {
        const std::type_info* const variable = nullptr;
        const std::type_info* const constant = nullptr;


        

        constexpr ExternInfo(const std::type_info& var, std::type_info& con) : variable{ &var }, constant{ &con }
        {}
    };

    struct ScriptInterface;

    //TODO: Very similar to its interface, this will need to have different versions, as well as IMPLEMENTING A SOURCE so this is safely includable.
    struct External {
        virtual ~External() = default;

        //This is the core function of the object type. In it's implementation,
        virtual void Invoke(std::string_view func_name, Variable& ret, ArgContainer<Variable>& args)
        {
            //The above should be const.

            //Extra point should go toward notifying one that this literaly is unimplemented.
            // A specific function should be the thing that does this btw.
            "No implementation of [Put type name here]::Invoke exists. ";
            throw nullptr;

        }

        virtual constexpr const std::type_info& GetTypeInfo() const
        {
            return typeid(*this);
        }

        virtual constexpr std::string_view GetName()
        {
            //This cannot work properly for the const version. So I may make 2 versions
            
            //Returns the top level name of this polymorphic type. Can be changed to get the current name
            // in order to have a central version be the registered type.
            return typeid(*this).name();
        }

        //Need to make sure they're of the same type.
        virtual External& operator=(const External& a_rhs) = 0;

        virtual std::strong_ordering operator<=>(const External& a_rhs) = 0;




        template <typename Self>
        constexpr std::string_view GetCurrentName(this Self&& self)
        {
            //The name of the type this was called at, regardless of what it inherits.
            return typeid(Self).name();
        }

        template <std::derived_from<External> Self>
        constexpr Self& MatchType(this Self&& self, const External& a_rhs)
        {
            //Also maybe use that type name implementation if you can. Just so different versions don't get caught on each other.

            if (_registry != a_rhs._registry)
            {
                //Types were not matched. Reporting failure.
                //Complete and utter failure.
            }

            return static_cast<Self&>(a_rhs);
        }


        External()
        {
            
            _registry = ExternalManager::GetInterface(std::string{GetName()});

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

        ExternalInterface* _registry = nullptr;
    };

}