#pragma once


#include "Number.h"
#include "AbstractTypePolicy.h"
#include "AbstractFunction.h"


//Some notes about the, the setting that's used should be a result of whichever of the 2 sides has more priority. Need to figure out a way
// to do that, then have a function handle the truncating process.
namespace LEX
{
    //RULE: All superfluous things are moved to the back for now.


    //Number should be a pivot instead.
    using Integer = int32_t;



    using Boolean = bool;
    
    


    struct Variable;


    namespace
    {

        template <typename T>
        AbstractTypePolicy* GetConcreteType()
        {
            //the idea of this function is that it needs explicit specialization, or the existence of a certain function within the type to be used.
            // Otherwise, it will return null, or perhaps the void type.
            return nullptr;
        }

        class Array
        {
            //It's possible I opt to make my own container for this, just to cut on space
            //Either way, to get what type ID array is using, one need only (but only within source
            std::vector<Variable> data;

            //Arrays themselves will need to be stocked with a type to submit to the variable when called upon.
            AbstractTypePolicy* type;


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



        class FunctionPtr
        {
            //I completely forgot, this can hold onto methods. This is GOING to need to be an index instead. Consult the method function syntax instead.

            AbstractFunction* _ptr = nullptr;


        public:
            constexpr std::strong_ordering operator <=> (const FunctionPtr& rhs) const
            {
                return _ptr <=> rhs._ptr;
            }


            constexpr bool operator==(const FunctionPtr& a_rhs) const
            {
                return operator<=>(a_rhs) == std::strong_ordering::equal;
            }

            //The abstract function is baiscally what's supposed to be used above all else. It's basically the handler or something, to prevent real interaction with the abstract 
            // function, or just to spruce it the process of calling some, as well as handling and ditating the return. (I could also make a template handle that).
        };

        class Delegate
        {
            //It's possible I opt to make my own container for this, just to cut on space
            //Either way, to get what type ID array is using, one need only (but only within source)

            //notice to the above, delegate will need 
            std::vector<AbstractFunction*> _callbacks;

        public:
            constexpr std::partial_ordering operator <=> (const Delegate& rhs) const
            {
                //Similar to array, but should be measured by it's count of callbacks.
                return std::strong_ordering::equal;
            }


            constexpr bool operator==(const Delegate& a_rhs) const
            {
                return operator<=>(a_rhs) == std::strong_ordering::equal;
            }

        };

        //These likely should be objects, or something that allows for delegate to be more of a reference type.

        // Then again, arrays are in a similar boat.


        
    }
    
   
    
    namespace//Object experiment
    {
        //The object experimentation zone. Nothing is cut in stone,  still messing around with the concepts.
        
        //Span might be better, it's smaller, not aimed to be resized, I think it might fit my needs well.
        template <typename T>
        using ArgContainer = std::span<T, std::dynamic_extent>;

        class Object;
        
        //The consntructor would need to take more than just this.
        //There are 2 types of syntax for objects, creation and finding.

        //So I think the arguments are something like a span of transfer safe strings, and a boolean asking if it's looking for something or creating something.
        // note, the type policy should be the thing that manages whether it's actually able to be looked up or if it's able to be created. 
        using ObjectFactory = Object* (*)();

        //This name is HELLA changing. But it's the interface that deals with queries about the class.
        using ObjectRegistry = void;

        struct ObjectInterface
        {
            static std::map<std::string_view, ObjectFactory> registerTable;

            //This definitely takes more than this, but this is all I'll be doing right now.
            static void RegisterObjectType(std::string_view type, ObjectFactory ctor)
            {

            }

            static ObjectRegistry* GetObjectRegistry(std::string_view type_name)
            {
                return nullptr;
            }
        };

        //This is for a core type that an object derives from, in order to prevent a scenerio where an external type will have versioning that changes the name.
        // I would like there to instead be an error if the object type doesn't derive from the targeted type.
        template<typename T> concept HasObjectType = requires
        {
            requires std::derived_from<T, typename T::ObjectType>;
        }&& std::derived_from<T, Object>;

        

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
                _registry = ObjectInterface::GetObjectRegistry(GetName());
                
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

            ObjectRegistry* _registry = nullptr;
        };


        template <std::derived_from<Object> TObj>
        static void RegisterType(std::string_view path, ObjectFactory ctor)
        {
            //How one uses this is they use register, and specify the project they're assigning this data type. One can only do it to shared, or a project they have
            // control over.
            if constexpr (std::is_final_v<TObj> || !std::is_same_v<decltype(&TObj::GetName), decltype(&Object::GetName)>) {

                return;
            }
            else {
                //Is not viable, or maybe warn
            }
        }


        struct ObjectPtr
        {
            //the main pointer to an object. Is the main thing that handles interaction with the object in question, making safe calls to all of its vfunctions.

            Object* _data = nullptr;


            constexpr std::strong_ordering operator <=> (const ObjectPtr& rhs) const
            {
                //This will need to decipher if it's a value type or not, but ultimately the actual object is supposed
                // to handle that. Non null is always greater than null unless they're the same.
                return _data <=> rhs._data;
            }

            constexpr bool operator==(const ObjectPtr& a_rhs) const
            {
                return operator<=>(a_rhs) == std::strong_ordering::equal;
            }


            constexpr ObjectPtr(std::nullptr_t) {}
        };


    }
    


    struct GameObject : public Object
    {


        //Game object is a class that's used to manage objects from skyrim. Seeing as this project isn't IN Clib though, just going to have to stomach its current set up.
    };
    

    using String = std::string;
    class String_
    {
        using Self = String_;

        char* _ptr = nullptr;
        size_t _length = 0;
    public:
        ~String_()
        {

        }


        constexpr String_() = default;

        String_(const char* str)
        {

        }

        String_(const std::string& str)
        {

        }

        template <size_t N>
        String_(const char(&str)[N])//maybe use string lit?
        {

        }

        //virtual void Destroy() const
        //{
            //Unsure if this is required
            //delete this;
        //}
    };

    

    
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
        Array,                          //Represents all array types.
        String,                         //
        Boolean,                        //Straight boolean. Has some way to validate with every single type.
        FunctionPtr,                  //Is the abstract function, can be specialized or normal function, or membered or whatever. I sorta want to obscure some of the abstract function stuff. But this NEEDS to be here, and not as an object.
        ObjectPtr,                         //_object
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
        Boolean,
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
        variant_index<VariableData, Boolean>(),
        variant_index<VariableData, FunctionPtr>(),
        variant_index<VariableData, Delegate>(),
        variant_index<VariableData, ObjectPtr>()
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
            case basicTypeIndices[BasicType::Boolean]:  return BasicType::Boolean;
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

        //For its
        template <Assignable<VariableComponent> T>
        Variable(T&& value, AbstractTypePolicy* policy)
        {
            _data = value;
            //Might not do it like this no more.
            _SetPolicy(policy);
        }

        //Deprecated. TypePolicy will deal with this later,
        static Variable Default() { return Variable(); }
        
        //This tag is always false, but empty for the API to tell where a variable came from. Essentially allows us to know whether 
       // we're allowed to delete it when it's a pointer or not.
        //DEPRECATED, you aren't allowed to set these from a project, variables from projects can only hold data, and are marked as temporary.
        bool _apiTag = false;

        union
        {

            //This is used to get the 3 least significant bits.
            std::bitset<64> _bits;
            uint64_t        _raw;
            //This might be const later?
            // Also, this is literally useless. Not worth the sun above clouds.
            AbstractTypePolicy* _type = nullptr;
        };
       
        VariableComponent _data{ Void::value()};

        AbstractTypePolicy* GetPolicy()
        {
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



    public:

        explicit operator IVariable* ()
        {
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




        Number AsNumber() { return {}; }
        Integer AsInteger() { return 0; }
        String AsString() { return ""; }
        ObjectPtr AsObject() { return nullptr; }
        Delegate AsDelegate() { return {}; }
        FunctionPtr AsFunction() { return {}; }
        Array AsArray() { return {}; }



        Variable() = default;

        void Clear()
        {
            //Needs implementation
            throw nullptr;
        }

        
        void SetBasicType(BasicType a_rhs)
        {
            //When doing something like this, clear flags

            switch (a_rhs)
            {
            case BasicType::String:     _data = ""; break;
            case BasicType::Number:     _data = (Number)0; break;
            case BasicType::Array:      _data = Array{}; break;
            case BasicType::Object:     _data = ObjectPtr(nullptr); break;
            case BasicType::Boolean:    _data = Boolean(0); break;
            case BasicType::Delegate:   _data = Delegate{}; break;
            case BasicType::Function:   _data = FunctionPtr{}; break;

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
        //Can only be created with argType?
        Variable(BasicType a_rhs)
        {
            //Never fucking use this.
            SetBasicType(a_rhs);
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

        template <Constructible<VariableComponent> T>
        Variable(T&& value)//, AbstractTypePolicy* type)
        {
            _data = value;           
        }

        //Used to construct literals.
        Variable(const Record& a_rhs)
        {
            
        }



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