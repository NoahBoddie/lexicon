#pragma once 


namespace LEX
{
    class Variable; 

    struct Type;
    /*
    class Array
    {
        //It's possible I opt to make my own container for this, just to cut on space
        //Either way, to get what type ID array is using, one need only (but only within source
        std::vector<Variable> data;

        //Arrays themselves will need to be stocked with a type to submit to the variable when called upon.
        Type* type;


        static Type* GetVariableType(const Array*)
        {
            return nullptr;
        }

    public:
        constexpr std::strong_ordering operator <=> (const Array& rhs) const
        {
            //ideally I'd wish to compare it's variables until its unequal.
            return std::strong_ordering::equal;
        }


        constexpr bool operator==(const Array& a_rhs) const
        {
            return operator<=>(a_rhs) == std::strong_ordering::equal;
        }

    };

    //*/

}