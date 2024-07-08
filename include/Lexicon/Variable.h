#pragma once

#include "String.h"
#include "Number.h"
#include "AbstractTypePolicy.h"
//#include "AbstractFunction.h"
//#include "ExternalHandle.h"
#include "Object.h"
#include "Array.h"
//#include "Delegate.h"
//#include "FunctionHandle.h"


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
    

    
    
    
    //using Typer = decltype(std::variant<size_t>::_Which);
    //constexpr auto size__ = sizeof(decltype(std::variant<size_t>::_Which));
    
    
   
    
    
    
    //TODO: Remove Array, FunctionHandle, and Delegate from being handled here. All of which are likely to be handled by object.
    using VariableValue =  std::variant
        <
        //Types that will not need to exist coming soon:
        // prompt, Index,
        Void,						    //Void given form. Invalidates all other types.
        Number,                         //Represents all numeric values. Integers, Floats, and Boolean values.
        String,
        Object
        >;
    
    

    using SizeType = std::_Variant_index_t<std::variant_size_v<VariableValue>>;

    constexpr auto req_size = 8 - sizeof(SizeType);

    ENUM(VariableFlag, uint8_t)
    {
        None,
        Polymorphic     = 1 << 0,
        Readonly        = 1 << 1, //This is a constant that is baked into the variable. Used for things like literals maybe.
        ExternCreated   = 1 << 2, //likely to be unused, but a left over from the previous flag.
        Defined         = 1 << 3, //Used to read if the value is defined. Generally, this will just be ticked in situations like make object, or assigning a value.
    };

    struct VariableData
    {
        //Program the clearing of this padding into the component.
        
        mutable VariableFlag flags = VariableFlag::None;
        
        uint8_t changed = 0;//Used to tell if it has changed between function calls.


        uint8_t GetChangeNum()
        {
            return changed;
        }

        bool HasFlag(VariableFlag flag) const
        {
            return flags & flag;
        }

        void SetFlag(VariableFlag flag, bool value)
        {
            if (value)
                flags |= flag;
            else
                flags &= ~flag;
        }



        bool IsPolymorphic() const { return HasFlag(VariableFlag::Polymorphic); }
        bool IsReadonly() const { return HasFlag(VariableFlag::Readonly); }
        bool IsDefined() const { return HasFlag(VariableFlag::Defined); }



        void Clear()
        {
            *this = VariableData();
        }
    };
    static_assert(sizeof(VariableData) <= req_size, "VarData must equal the size of the padding in VariableValue");

#pragma pack(pop)
    
    namespace 
    {
        //Method to find an object or variable.

        template <class T, class U> struct is_one_of;

        template <class T, class... Ts>
        struct is_one_of<T, std::variant<Ts...>>
            : std::bool_constant<(std::derived_from<T, Ts> || ...)>
        { };


        template<typename T>
        concept ObjectClass = is_one_of<T, VariableValue>::value;

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
        String,
        Object,
        Total,
    };

    constexpr std::array<size_t, (size_t)BasicType::Total> basicTypeIndices
    {
        variant_index<VariableValue, Void>(),
        variant_index<VariableValue, Number>(),
        variant_index<VariableValue, String>(),
        variant_index<VariableValue, Object>()
    };

    struct DataHelper
    {
        //This will help clear the Variable data spot without me having to put clear in every constructor. Hopefully.


        VariableData& GetData()
        {
            auto a_this = (uintptr_t)this;

            return *reinterpret_cast<VariableData*>(a_this + sizeof(VariableValue) - sizeof(VariableData));
        }
        
        const VariableData& GetData() const
        {
            auto a_this = (uintptr_t)this;

            return *reinterpret_cast<VariableData*>(a_this + sizeof(VariableValue) - sizeof(VariableData));
        }


        void HelpClear()
        {
            GetData().Clear();
        }


        DataHelper()
        {
            HelpClear();
        }

        
        DataHelper(DataHelper&& other)
        {
            HelpClear();
        }

        DataHelper(const DataHelper& other)
        {
            HelpClear();
        }


        DataHelper& operator=(const DataHelper& other) = default;



        DataHelper& operator=(DataHelper&& other) = default;
    };

    //Make a copy of the copy constructors
    struct VariableComponent : public DataHelper, public RGL::ClassAlias<VariableValue>
    {
        ALIAS_HEADER;

        //I want to make some ease of access functions in this.
        //If I do, this might become the primary class Variable uses

        BasicType GetBasicType() const
        {
            //I want to put this shit within Variable Component and make this use that instead.
            switch (index())
            {
            case basicTypeIndices[BasicType::Void]:     return BasicType::Void;
            case basicTypeIndices[BasicType::Number]:   return BasicType::Number;
            case basicTypeIndices[BasicType::String]:   return BasicType::Number;
            case basicTypeIndices[BasicType::Object]:   return BasicType::Object;
            default:                                    return BasicType::Total;
            }
        }
    };
    static_assert(sizeof(VariableComponent) == sizeof(VariableValue), "Size of Variable Component and Value must be equal for data helper to work.");

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

    //template <typename T>
    //AbstractTypePolicy* TypeOf()
    //{
        //The idea of this function is that it takes the core type and splits how it finds it's type.
        //For objects it should be their object interface that does it. However, part of this might be that the actual object will need to find that itself (if it can).
        // For example, delegates and function pointers will be able to use their real types to get that information
        //For Numbers the setting should do it
        //For Booleans it's obvious
        //For
        //return nullptr;
    //}


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

        Variable() = default;
        
        //*
        template <Constructible<VariableComponent> T>
        Variable(T&& value)//, AbstractTypePolicy* type)
        {
            _value = value;
            SetPolicy(_CheckVariableType());//REFUTE
            _SetDefined(true);
        }
        //Variable(const Variable& rhs) = default;
        Variable(const Variable& rhs)
        {
            _value = rhs._value;
            SetPolicy(rhs.Policy());
            _SetDefined(rhs.IsDefined());
            _SetChanged(rhs.IsChanged());
        }
        //*/

        Variable(Variable&& rhs) = default;


        template <Assignable<VariableComponent> T>
        Variable(T&& other, AbstractTypePolicy* policy)
        {
            _value = other;
            //Might not do it like this no more.
            SetPolicy(policy);
            _SetDefined(true);
        }





        Variable(Variable& var, AbstractTypePolicy* policy)
        {
            //Unsure about this one.
            _value = var._value;
            SetPolicy(policy);
        }



        //Variable& operator=(const Variable& rhs) = default;
        Variable& operator=(const Variable& rhs)
        {
            _value = rhs._value;
            SetPolicy(rhs.Policy());
            _SetDefined(rhs.IsDefined());
            _SetChanged(rhs.IsChanged());
            return *this;
        }

        //Variable& operator=(Variable&& rhs) noexcept = default;
        Variable& operator=(Variable&& rhs) noexcept
        {
            _value = std::move(rhs._value);
            SetPolicy(rhs.Policy());
            _SetDefined(rhs.IsDefined());
            _SetChanged(rhs.IsChanged());
            return *this;
        }


        template <Constructible<VariableComponent> T>
        Variable& operator=(T& value)
        {
            _value = value;
            SetPolicy(_CheckVariableType());//REFUTE
            _SetDefined(true);
            _SetChanged(true);
            return *this;
        }



    private:
        Variable(const Record& a_rhs)
        {
            //Used to construct literals.
        }
    public:

        //IMPORTANT, DO NOT DELETE.
        /*
        

        //For its
        



        Variable(const Variable&& rhs)
        {
            _value = rhs._value;
            SetPolicy(rhs.GetPolicy());
            _SetDefined(rhs.IsDefined());
            _SetChanged(rhs.IsChanged());
        }


        Variable(AbstractTypePolicy* policy)
        {
            *this = policy;
        }


        




        //Operators


        Variable& operator=(const Variable& rhs)
        {
            _value = rhs._value;
            SetPolicy(rhs.GetPolicy());
            _SetDefined(rhs.IsDefined());
            _SetChanged(rhs.IsChanged());
            return *this;
        }

        //Pretty sure this is implicitly more important than the other.
        Variable& operator=(const Variable&& rhs) noexcept
        {
            _value = rhs._value;
            SetPolicy(rhs.GetPolicy());
            _SetDefined(rhs.IsDefined());
            _SetChanged(rhs.IsChanged());
            return *this;
        }


        Variable& operator=(AbstractTypePolicy* policy)
        {
            //should clear
            SetPolicy(policy);
            _value = policy->GetDefault()._value;
            _SetDefined(false);
            _SetChanged(false);
            return *this;
        }

        template <Constructible<VariableComponent> T>
        Variable& operator=(T& value)
        {
            _value = value;
            SetPolicy(_CheckVariableType());//REFUTE
            _SetDefined(true);
            _SetChanged(true);
            return *this;
        }

        //*/


        AbstractTypePolicy* _type = nullptr;

       
        VariableComponent _value{ Void::value() };

        AbstractTypePolicy* Policy() const
        {
            return _type;
        }


    private: 

        void _SetDefined(bool value)
        {
            //Defined is now based upon the value that is used. To cover stuff like classes not being set up properly, I'm keeping the defined bit.
            _value.GetData().SetFlag(VariableFlag::Defined, value);
        }

        void _SetChanged(bool value)
        {
            if (value) {
                auto& change = _value.GetData().changed;

                change++;

                if (!change) change++;
            }
                
            else
                _value.GetData().changed = 0;
        }




    protected:
        
        void SetPolymorphic(bool value)
        {
            _value.GetData().SetFlag(VariableFlag::Polymorphic, value);
        }

        AbstractTypePolicy* _CheckVariableType();

    public:

        //PRIVATE_INTERNAL:
        void SetPolicy(AbstractTypePolicy* policy)
        {
            _type = policy;
        }


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
            return _value.GetData().IsPolymorphic();
        }


        bool IsDefined() const
        {
            //TODO: Use IsDefined to determine definition. It will be vital at some point.
            return !IsVoid();// && _value.GetData().IsDefined();
        }

        bool IsChanged() const
        {
            return _value.GetData().changed;
        }

        //So typeinfo is something I'll be putting right here. If the value is 0xFFFFFFFF or something rather other,
        // it's any. 0, it's like bare bones default.
        //So, if it exists, we include the value.

        
        std::strong_ordering operator<=>(const Variable& a_rhs) const
        {
            //This is bogus, doesn't really properly test inequivalency
            if (auto result = _type <=> a_rhs._type; result != std::strong_ordering::equal)
                return result;
            
            if (_value < a_rhs._value)
                return std::strong_ordering::less;

            if (_value > a_rhs._value)
                return std::strong_ordering::greater;

            return std::strong_ordering::equal;
        }
        
        constexpr bool operator==(const Variable& a_rhs) const
        {
            return operator<=>(a_rhs) == std::strong_ordering::equal;
        }

        BasicType GetBasicType() const
        {
            return _value.GetBasicType();
        }

        bool IsBasicType(BasicType type) const
        {
            return GetBasicType() == type;
        }

        //These should also maybe check the policies?
        bool IsVoid() const { return IsBasicType(BasicType::Void); }
        bool IsObject() const { return IsBasicType(BasicType::Object);}
        bool IsNumber() const { return IsBasicType(BasicType::Number); }
        bool IsString() const { return IsBasicType(BasicType::String); }




        Number AsNumber() { return std::get<Number>(_value); }
        Integer AsInteger() { throw nullptr; }
        String AsString() { return std::get<String>(_value); }
        Object AsObject() { return std::get<Object>(_value); }
        Array AsArray() { throw nullptr; }



        void Clear()
        {
            _value = Void{};
            _SetDefined(false);
            _SetChanged(false);
        }

        //destroy
        void SetBasicType__(BasicType a_rhs)
        {
            //When doing something like this, clear flags

            switch (a_rhs)
            {
            case BasicType::String:     _value = ""; break;
            //case BasicType::Number:     _value = (Number)0; break;

            case BasicType::Object:     _value = Object{}; break;

            default:    throw nullptr;// This is in error, just have no idea how.
            }
        }

      
        
        //Resolves a variable to be like another variable. If they aren't directly convertible, conversions
        // may be detected and used.
        bool Resolve(AbstractTypePolicy* policy)
        {
            //If resolution fails, it returns false and nullfies the variable.

            Variable& self = *this;

            Conversion out;

            if (auto result = Policy()->IsConvertibleTo(policy, policy, out); result <= ConvertResult::Failure) {
                Clear();
            }
            else if (out) {
                //if out has pulled a conversion, it will need set self to the result of the conversion.
                //TODO: Handle the conversions in Variable::Resolve
                //out->HandleConvert(policy, self);
            }
            else {
                //otherwise, it can let it go on as seen and will just adjust the type.
                SetPolicy(policy);
            }
            
            
            return IsVoid();

        }
    
        void CheckAssign(AbstractTypePolicy* other)
        {
            //Conversion must be a type conversion.
            //if (other->IsConvertibleTo(_type) == false)
            //    report::runtime::error("No type conversion between values.");
        }

        
        //This is probably the main thing I seek to be using.
        //Assign is kinda rough, for starters, it needs something to exist to work. Second, I'm not 100% sure where it would actually be used.
        // I guess it would be used in situation where I need to guard a given variable. Thus, if I'm working with RuntimeVariables, and I'm NOT
        // intending on setting 
        Variable& Assign(Variable& other)
        {
            if (_type)
                CheckAssign(FetchVariableType(other));

            _value = other._value;

            _SetDefined(true);
            _SetChanged(true);

            return *this;
        }

        Variable& Assign(Variable&& other)
        {
            return Assign(other);
        }

        template <Constructible<VariableComponent> T>
        Variable& Assign(T&& other)
        {
            if (_type)
                CheckAssign(GetValueType(other));
           
            _value = other;
            
            _SetDefined(true);
            
            
            return *this;
        }


       
        //template <class _Ty, enable_if_t<!is_same_v<_Remove_cvref_t<_Ty>, variant>
        //&& is_constructible_v<_Variant_init_type<_Ty, _Types...>, _Ty>
        //    && is_assignable_v<_Variant_init_type<_Ty, _Types...>&, _Ty>,
        //    int> = 0>
        //_CONSTEXPR20 variant& operator=(_Ty&& _Obj) noexcept(


        //If it's the exact thing it should try to give a reference I think
        template<variant_convertible_from<VariableValue> To>
        explicit operator To()
        {

            To result = {};

            std::visit([&](auto&& self) {
                logger::info("{} THING", typeid(decltype(self)).name());
                if constexpr (stl::castable_to<decltype(self), To>)
                    result = static_cast<To>(self);
                }, _value);

            return result;
        }


        template<variant_convertible_from<VariableValue> To, size_t I = 0>
        inline bool CanCastTo() const
        {
            //make one without the I, make that the impl version
            constexpr size_t k_index = convertible_variant_index_v<VariableValue, To, I>;


            if (_value.index() == k_index)
                return true;

            if constexpr (k_index != -1)
            {
                if constexpr (constexpr size_t _I = k_index + 1; _I < std::variant_size_v<VariableValue>)
                    return CanCastTo<To, _I>();

            }

            return false;
        }

        RuntimeVariable Convert(AbstractTypePolicy* to);
    };


   
    template <Assignable<VariableComponent> T>
    Variable MakeVariable(T&& other)
    {
        //TODO: Constraint to make_variable should be has assign variable and get storage type. For now, I'll only target it to do what the constructor did previously.
        //TODO: Later, needs to implement assign variable, which creates the initial variable to send.

        //NOTE: I'd actually like the names to be create/make. The idea would be that make variable turns the object into a variable, and create (what this will be named)
        // turns it into a responsible one.

        VariableComponent temp{ other };

        //This shouldn't need to work like this, but it's what it does for now.
        AbstractTypePolicy* policy = std::visit([](auto&& lhs) {
            return FetchVariableType(lhs);
            }, temp);

        //AbstractTypePolicy* policy = GetValueType(other);

        Variable result{ other, policy };


        return result;
    }
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