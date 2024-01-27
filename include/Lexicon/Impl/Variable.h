#pragma once


#include "Number.h"
#include "AbstractTypePolicy.h"
#include "AbstractFunction.h"
#include "ExternalHandle.h"

#include "Array.h"
#include "Delegate.h"
#include "FunctionHandle.h"


#include "TypeID.h"

#include "VariableType.h"

//Some notes about the, the setting that's used should be a result of whichever of the 2 sides has more priority. Need to figure out a way
// to do that, then have a function handle the truncating process.
namespace LEX
{
    //RULE: All superfluous things are moved to the back for now.


    //Number should be a pivot instead.
    using Integer = int32_t;



    using Boolean = bool;
    
    using String = std::string;

    

    struct Variable;

    
    struct VariableRef
    {
        //The idea of this struct is that it takes a reference to a variable.
        // I think the concept is, if it's copy constructing, it will transfer over the reference. If copy assigning it will copy it's target.
        // or something. I'll need to put some thought into this.


        //Also note, ref types will have to exist A, outside of any (not derived from voidable, nor any).
        Variable* _ref;

        
    };

    //TODO: Do the below. Medium priority.
    //Resuming the style from last time, there's a single function that one overloads for each of these to determine how their IDs are recieved.
    // Accompanying that, there should be a handler function that turns a type into a variable. Either by turning it into an object, or transfering it into one of the types.
    using VariableData = std::variant
        <
        //Types that will not need to exist coming soon:
        // prompt, Index,
        Void,						    //Void given form. Invalidates all other types.
        Number,                         //Represents all numeric values. Integers, Floats, and Boolean values.
        String,
        Array,
        FunctionHandle,
        ExternalHandle,
        Delegate
        >;
    

    namespace 
    {
        //Method to find an object or variable.

        template <class T, class U> struct is_one_of;

        template <class T, class... Ts>
        struct is_one_of<T, std::variant<Ts...>>
            : std::bool_constant<(std::derived_from<T, Ts> || ...)>
        { };


        template<typename T>
        concept ObjectClass = is_one_of<T, VariableData>::value;

    }

    
    //TODO:Move get_index to a type_trait in RGL
    template<typename VariantType, typename T, std::size_t index = 0>
    constexpr std::size_t variant_index() {
        static_assert(std::variant_size_v<VariantType> > index, "Type not found in variant");
        if constexpr (index == std::variant_size_v<VariantType>) {
            return index;
        }
        else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
            return index;
        }
        else {
            return variant_index<VariantType, T, index + 1>();
        }
    }


    ENUM (BasicType, uint8_t)
    {
        Void,
        Number,
        Array,
        String,
        Function,
        Delegate,
        Object,
        Total,
    };

    constexpr std::array<size_t, (size_t)BasicType::Total> basicTypeIndices
    {
        variant_index<VariableData, Void>(),
        variant_index<VariableData, Number>(),
        variant_index<VariableData, Array>(),
        variant_index<VariableData, String>(),
        variant_index<VariableData, FunctionHandle>(),
        variant_index<VariableData, Delegate>(),
        variant_index<VariableData, ExternalHandle>()
    };


    //Make a copy of the copy constructors
    struct VariableComponent : public RGL::ClassAlias<VariableData>
    {
        ALIAS_HEADER;

        //I want to make some ease of access functions in this.
        //If I do, this might become the primary class Variable uses


        BasicType GetType() const
        {
            //I want to put this shit within Variable Component and make this use that instead.
            switch (index())
            {
            case basicTypeIndices[BasicType::Void]:     return BasicType::Void;
            case basicTypeIndices[BasicType::Number]:   return BasicType::Number;
            case basicTypeIndices[BasicType::Array]:    return BasicType::Array;
            case basicTypeIndices[BasicType::String]:   return BasicType::Number;
            case basicTypeIndices[BasicType::Function]: return BasicType::Function;
            case basicTypeIndices[BasicType::Delegate]: return BasicType::Delegate;
            case basicTypeIndices[BasicType::Object]:   return BasicType::Object;
            default:                                    return BasicType::Total;
            }
        }
    };

    //The concept for the abstract version of a poly morphic class.
    struct IVariable;
    
    template <typename T1, typename T2>
    concept Assignable = requires(T2 base, T1 insert) {
        { base.operator=(insert) };
    };

    template <typename T1, typename T2>
    concept Constructible = requires(T1 insert) {
        { T2{ insert } };
    };

    template <typename T>
    AbstractTypePolicy* TypeOf()
    {
        //The idea of this function is that it takes the core type and splits how it finds it's type.
        //For objects it should be their object interface that does it. However, part of this might be that the actual object will need to find that itself (if it can).
        // For example, delegates and function pointers will be able to use their real types to get that information
        //For Numbers the setting should do it
        //For Booleans it's obvious
        //For
        return nullptr;
    }


    struct TestUnion
    {
        //This class needs to be it's own thing, not just a part of variable.

        union
        {
            uint64_t raw;
            std::bitset<64> bits;//The idea is this will allow us to extract bits before and after assigning.
            struct
            {
                TypeID id;
                uint32_t spec;
            }; 
        };
    };
    static_assert(sizeof(TestUnion) == 8, "grgihyrbg");
    static_assert(sizeof(std::bitset<61>) == 8, "grgihyrbg");


    struct Variable
    {
        //A thing to think about, if how this interacts between versions is ever a problem, simply including a vtable so the implementations are always the same
        // would be easy.

        //If an object is created with a type and nothing else, this bit will not be on.
        constexpr static size_t k_defineFlag = 1 << 0;
        //If an object has been assigned since 
        constexpr static size_t k_changeFlag = 1 << 1;
        //A bit that represents a larger class at hand. Allows it to be static cast to a version of variable which has a vtable, allowing it to be converted 
        // dynamically
        constexpr static size_t k_polyFlag = 1 << 2;

        constexpr static size_t k_flags = k_defineFlag | k_changeFlag | k_polyFlag;



        //No idea why this delegating ctor crashes, will address when I can actually be arsed
        //template <Constructible<VariableComponent> T>
        //Variable(T&& value) : Variable{ value, _CheckVariableType() }
        //{}




        /*
        X Construction From Default- No define, no change.
        X Construction From Convertible- Construction based on "convertible to data" will  attempt to automatically create the variable based on the type. Define, maybe clear change?
        X Construction From Variable- Construction from variable is viewed as a copy, taking on the values of the right side excluding the polymorhic flag. Define, Clear change.
        Construction from type- Being constructed from a type basically assigns the default value from the type to it. No Define, No change.
        Construction From Function- Using a specific function to create the Variable will lead to the resulting value being not filling the variables policy. Creates the value, not the type. Defined, no change

        Assigning from Convertible- Assigning from convertible will lead to the change flag being edited, as well as the definition flag. In addition, this should edit the stored type. Change flag ticked.

        Assign from variable- seen very similar from how construct from variable is seen, this is a complete reset.
        Assign from Type- reflect ctor

        //*/

        Variable() = default;
        
        template <Constructible<VariableComponent> T>
        Variable(T&& value)//, AbstractTypePolicy* type)
        {
            _data = value;
            _SetPolicy(_CheckVariableType());//REFUTE
            _SetDefined(true);
        }

        //For its
        template <Assignable<VariableComponent> T>
        Variable(T&& value, AbstractTypePolicy* policy)
        {
            _data = value;
            //Might not do it like this no more.
            _SetPolicy(policy);
            _SetDefined(true);
        }



        Variable(const Variable& rhs)
        {
            _data = rhs._data;
            _SetPolicy(rhs.GetPolicy());
            _SetDefined(rhs.IsDefined());
            _SetChanged(rhs.IsChanged());
        }
        Variable(const Variable&& rhs)
        {
            _data = rhs._data;
            _SetPolicy(rhs.GetPolicy());
            _SetDefined(rhs.IsDefined());
            _SetChanged(rhs.IsChanged());
        }


        Variable(AbstractTypePolicy* policy)
        {
            *this = policy;
        }

        
        Variable(const Record& a_rhs)
        {
            //Used to construct literals.
        }


        Variable(Variable& var, AbstractTypePolicy* policy)
        {
            //Unsure about this one.
            _data = var._data;
            _SetPolicy(policy);
        }



        //Operators


        Variable& operator=(const Variable& rhs)
        {
            _data = rhs._data;
            _SetPolicy(rhs.GetPolicy());
            _SetDefined(rhs.IsDefined());
            _SetChanged(rhs.IsChanged());
            return *this;
        }

        Variable& operator=(const Variable&& rhs)
        {
            _data = rhs._data;
            _SetPolicy(rhs.GetPolicy());
            _SetDefined(rhs.IsDefined());
            _SetChanged(rhs.IsChanged());
            return *this;
        }


        Variable& operator=(AbstractTypePolicy* policy)
        {
            //should clear
            _SetPolicy(policy);
            _data = policy->GetDefault()._data;
            _SetDefined(false);
            _SetChanged(false);
            return *this;
        }

        template <Constructible<VariableComponent> T>
        Variable& operator=(T& value)
        {
            _data = value;
            _SetPolicy(_CheckVariableType());//REFUTE
            _SetDefined(true);
            _SetChanged(true);
            return *this;
        }




        //Deprecated. ConcretePolicy will deal with this later,
        static Variable Default() { return Variable(); }
        
        //This tag is always false, but empty for the API to tell where a variable came from. Essentially allows us to know whether 
       // we're allowed to delete it when it's a pointer or not.
        //DEPRECATED, you aren't allowed to set these from a project, variables from projects can only hold data, and are marked as temporary.
        bool _apiTag = false;

        union
        {
            //REIMPLEMENTATION REMINDER, I would like this to have flags for definition and change

            //This is used to get the 3 least significant bits.
            std::bitset<64> _bits;
            uint64_t        _raw;
            //This might be const later?
            // Also, this is literally useless. Not worth the sun above clouds.
            AbstractTypePolicy* _type = nullptr;
        };
       
        VariableComponent _data{ Void::value() };

        AbstractTypePolicy* GetPolicy() const
        {
            //TODO: return to GetPolicy/SetPolicy, I've disabled the bits because shits experimental
            //return _type;
            return (AbstractTypePolicy*)(_raw & ~k_flags);
        }


    private: 

        void _SetDefined(bool value)
        {
            _bits.set(0, value);
        }

        void _SetChanged(bool value)
        {
            _bits.set(1, value);
        }


        void _SetPolicy(AbstractTypePolicy* policy)
        {
            //_type = policy;


            //Purge except the flags, then add the policy pointer.
            _raw &= k_flags;
            //Check for bits in policy at this junction here.

            _raw |= reinterpret_cast<uint64_t>(policy);
            
        }


    protected:
        
        void SetPolymorphic(bool value)
        {
            _bits.set(2, value);
        }

        AbstractTypePolicy* _CheckVariableType();

    public:

        explicit operator IVariable* ()
        {
            //return IsPolymorphic() ? static_cast<IVariable*>(this) : nullptr;
            if (IsPolymorphic() == false)
                return nullptr;

            //Not really implemented
            return nullptr;
        }
        
        bool IsPolymorphic() const
        {
            return _raw & k_polyFlag;
        }


        bool IsDefined() const
        {
            return _raw & k_defineFlag;
        }

        bool IsChanged() const
        {
            return _raw & k_changeFlag;
        }

        //So typeinfo is something I'll be putting right here. If the value is 0xFFFFFFFF or something rather other,
        // it's any. 0, it's like bare bones default.
        //So, if it exists, we include the value.

        
        std::strong_ordering operator<=>(const Variable& a_rhs) const
        {
            //This is bogus, doesn't really properly test inequivalency
            if (auto result = _raw <=> a_rhs._raw; result != std::strong_ordering::equal)
                return result;
            
            if (_data < a_rhs._data)
                return std::strong_ordering::less;

            if (_data > a_rhs._data)
                return std::strong_ordering::greater;

            return std::strong_ordering::equal;
        }
        
        constexpr bool operator==(const Variable& a_rhs) const
        {
            return operator<=>(a_rhs) == std::strong_ordering::equal;
        }

        BasicType GetBasicType() const
        {
            return BasicType::Void;
        }

        bool IsBasicType(BasicType type) const
        {
            return false;
        }

        //These should also maybe check the policies?
        bool IsVoid() const { return IsBasicType(BasicType::Void); }
        bool IsObject() const { return false; }
        bool IsNumber() const { return false; }
        bool IsString() const { return false; }




        Number AsNumber() { return std::get<Number>(_data); }
        Integer AsInteger() { return 0; }
        String AsString() { return ""; }
        ExternalHandle AsExternal() { return nullptr; }
        Delegate AsDelegate() { return {}; }
        FunctionHandle AsFunction() { return {}; }
        Array AsArray() { return {}; }



        void Clear()
        {
            //Needs implementation
            throw nullptr;

            _data = Void{};
            _SetDefined(false);
            _SetChanged(false);

        }

        //destroy
        void SetBasicType__(BasicType a_rhs)
        {
            //When doing something like this, clear flags

            switch (a_rhs)
            {
            case BasicType::String:     _data = ""; break;
            //case BasicType::Number:     _data = (Number)0; break;
            case BasicType::Array:      _data = Array{}; break;
            case BasicType::Object:     _data = ExternalHandle(nullptr); break;
            case BasicType::Delegate:   _data = Delegate{}; break;
            case BasicType::Function:   _data = FunctionHandle{}; break;

            default:    throw nullptr;// This is in error, just have no idea how.
            }
        }

        //Doesn't transfer qualifiers
        void Copy(const Variable& a_rhs, bool lock_type, bool preserve)
        {
            //If type is locked it will use the AbstractTypePolicy to prevent the variable from changing to a value that it isn't.

            //The idea of either the transfer or assign is that copying a variable will not copy over the actual type.
            // I'm not sure which should do it, or how to handle convertible types.

            //This is the function that will override types and such.

            //These lsb's need to be preserved. Such as if they're polymorphic a copy won't affect that.
            size_t lsb = _raw & k_flags;

            //Though, Im unsure of which I should be keeping to be honest.

            _raw = a_rhs._raw;
            _data = a_rhs._data;
            
            //ClearFlags();
            _raw &= k_flags;
            _raw |= lsb;
        }
        
        //Resolves a variable to be like another variable. If they aren't directly convertible, conversions
        // may be detected and used.
        bool Resolve(AbstractTypePolicy* policy)
        {
            //If resolution fails, it returns false and nullfies the variable.

            Variable& self = *this;

            ICallableUnit* out = nullptr;

            if (GetPolicy()->IsConvertibleTo(policy, out) == false){
                Clear();
            }
            else if (out) {
                //if out has pulled a conversion, it will need set self to the result of the conversion.
                //TODO: Handle the conversions in Variable::Resolve
                //out->HandleConvert(policy, self);
            }
            else {
                //otherwise, it can let it go on as seen and will just adjust the type.
                _SetPolicy(policy);
            }
            

            return IsVoid();

        }



        //With assignments should only be between the types that are considered core to this object, and shouldn't clear flags. There should additionally be 
        // a version of the function that sends exceptions if the wrong interaction is going on.
        //If you use an construction with a different variable this will reset the value (IE, it uses a version that does not preserve flags).
        
        //Revise. The below needs to be primary if I take it to be true
        // other than variables being copied to variables, assignments from the variable components to an unexpected type will result in a crash.
        // to confirm this, the object policy is given the variable form of the new object to see if it's valid.

        //Last revision. No Assignment from the components. Assignments are only allowed between variables, as their idea is the overwriting of previous
        // settings, including types. In that, they are to be used sparingly.
        //When Convert is used (the function that will set one to the other with respects to the types) if the variable component indices don't match 
        // a runtime error will be observed.
        //If you assign with a variable that doesn't have a type

        //For each component then, there's a function that will help find it's type.
        // Since arrays are soon to be an object, as are delegates and function pointers, they're easier to handle in a sense.
        // Boolean is easy, Number will take some doing, Strings are easy etc.
        // So the function to get an

        //It can construct with the components though, note that's not a viable variable for anything.
        
        //Variable& operator=(const Variable& a_rhs)
        //{
        //    Copy(a_rhs, true);
        //}
        //Variable& operator=(const Variable&& a_rhs)
        //{
        //    wchar_t t;
        //}




        //None of these account for index.

        //As will be unsafe for now.

        //TODO: All "Variable::As" functions will return references (hence the unsafeness).

        //I may make a safe version of this to use, that takes a ref argument.
        //Also another version that's not enum dependent.
    };


}


/*

    //Represents the any type, may use just so Any isn't mistaken for void.
    //That, or I could have any rep the type it actually is.
    struct Any {};
    using Property = int64_t;


    //This represents the std::file_location type deal. This is more interface stuff.
    CStringW thisDllDirPath()
    {
        //This is sorta system I'd like to use to associate plugin instances with who's calling for what.
        // I could actually use this plus an external functions to call upon th interface system.

        CStringW thisPath = L"";
        WCHAR path[MAX_PATH];
        HMODULE hm;
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPWSTR)&thisDllDirPath, &hm))
        {
            GetModuleFileNameW(hm, path, MAX_PATH);
            PathRemoveFileSpecW(path);
            thisPath = CStringW(path);
            if (!thisPath.IsEmpty() &&
                thisPath.GetAt(thisPath.GetLength() - 1) != '\\')
                thisPath += L"\\";
        }
        else if (_DEBUG) std::wcout << L"GetModuleHandle Error: " << GetLastError() << std::endl;

        if (_DEBUG) std::wcout << L"thisDllDirPath: [" << CStringW::PCXSTR(thisPath) << L"]" << std::endl;
        return thisPath;
    }


//*/