

//int main(int argc, char** argv) { return 0; }

//*


#include "Lexicon/Engine/SettingManager.h"

#include "Lexicon.h"
#include "Lexicon/Engine/TestFieldB.h"

#include "Lexicon/Issue.h"
//#include "spdlog/spdlog.h"
//#include "spdlog/sinks/stdout_color_sinks.h"
//#include <spdlog/sinks/basic_file_sink.h>
//#include <spdlog/sinks/msvc_sink.h>
//#include <spdlog/sinks/stdout_sinks.h>



//
//#include "Lexicon/Engine/TempConstruct.cpp"
//

using namespace RGL;
using namespace LEX;
using namespace LEX;

namespace logger
{

}


namespace std {
    template <>
    struct hash<std::source_location> {
        std::size_t operator()(const std::source_location& loc) const noexcept {
            
        }
    };
}



static void PrintAST(Record& tree, std::string indent = "")
{
    const static std::string __dent = "|  ";

    std::string log = tree.Print<Syntax>();
    
    RGL_LOG(info, "{}{}", indent, log);

    indent += __dent;

    for (auto& child_rec : tree.children())
    {
        PrintAST(child_rec, indent);
    }
}


void TestProcedure(RuntimeVariable& result, Variable* target, std::vector<Variable*> args, ProcedureData& data)
{
    std::string tar = target->AsString();
    logger::info("string \"{}\", size {}; arg size {} of {} ({})?", tar, tar.size(), args.size(), ((ConcreteFunction*)data.function)->_name, ((ConcreteFunction*)data.function)->parameters.size());
    result = tar.size();
}

using STR = std::string;

double size_backend(std::string a_this)
{
    
	//logger::info("size of \"{}\"", a_this.view());
	return a_this.size();
}


double size_for_int(int a_this)
{
    return a_this;
}

static int ref = 50;

int& RefTest2(StaticTargetTag)
{
    

    return ref;
}


void otherTest(StaticTargetTag)
{
    static int _inc = 0;
    
    
    logger::info("other test {}", _inc++);   
}


float TestCall_backend(float r, float a, float b)
{

    r = r + 3;//This is the bread and butter of what we want. All considered, it should be 93, but if not done right, it's 90.

    return a + b + r;
};

double GetActorValue_backend(double a_this, double othername, double shootfol, double tellinal, double peacefal, double scrundal)
{
    double result = 0;

    logger::info("GetActorValue_BackEnd::Start: {} ({}, {}, {}, {}, {})", a_this, othername, shootfol, tellinal, peacefal, scrundal);
    //int test = othername + shootfol + tellinal + peacefal + scrundal;
            //Nope, it's just broken right now.
            //int test = TestCall(othername + 2, shootfol) + tellinal + peacefal + scrundal;//This causes a crash?

            //This shit's itself for some reason.


    //Array _array2 = _array;
    std::string test_string = "THISSTR";
   
    float first = TestCall_backend(tellinal, othername, shootfol);

    tellinal += 3;
    float test = TestCall_backend(first, othername, shootfol) + tellinal + peacefal + scrundal + a_this;
    //float test = tellinal.TestCall(othername, shootfol)  + tellinal + peacefal + scrundal + this;
    a_this = 0;

    float testB = {};

    const float testConst = 5;

    //testConst = testB;

    
    result = a_this + test + testB + test_string.size();

    logger::info("GetActorValue_BackEnd::End: {} ({}, {}, {}, {}, {}), result: {}", a_this, othername, shootfol, tellinal, peacefal, scrundal, result);

    return result;;
}

#ifdef OBSERVE_ME
float uniqueFloat = 2.0f;

unsigned int uniqueInteger = 2;

float TestingPull()
{
    float testFloat = uniqueInteger
    uniqueInteger = 30 + (HasKeyword(props::PlayerKeyword) * 20) + GetActorValue2("StrengthAdaptive", props::All);
    return testFloat;
}

#endif


void TestParse(Script* append_to = nullptr)
{
    std::string path = "C:/Users/Noah/Desktop/Projects/[Project Data]/Mod Projects/Arithmetic/3.0/{scripted code}/Lexicon/scripts/Shared/parse_test.txt";
    std::ifstream file_input;
    file_input.open(path);

    std::stringstream stream;
    stream << file_input.rdbuf();
    std::string contents = stream.str();

    file_input.close();//Don't really need to do this, seeing as the destructor does, but eh
    
    if (contents.empty())
    {
        report::debug("parse_test.txt is empty");
        std::system("pause");
        return;
    }

    PreprocessorParser parser;

    //Record ast = Parser::CreateSyntaxTree("Fake", "Script", contents, &parser);
    Record ast;

    if (Parser::CreateSyntaxTree(ast, contents, "Script") == false)
    {
        logger::info("not viable thing.");
        std::system("pause");
        return;
    }

    PrintAST(ast);

    logger::info("Record uses {} Kilobytes", ast.GetMemoryUsage() / 1000.f);

    if (append_to)
    {
        append_to->AppendContent(ast.Transform<SyntaxRecord>());
    }


    std::system("pause");



}

void funct(int*const name, std::string_view)
{
    //name = nullptr;
}


template <typename... Qualifiers>
struct QualifierLayer;

template <>
struct QualifierLayer <> 
{
    inline void operator()(Qualifier& qualifiers) {}
};

template <typename T, typename... Qualifiers>
struct QualifierLayer <T, Qualifiers...> : public T, QualifierLayer<Qualifiers...> 
{
    using Base = QualifierLayer<Qualifiers...>;

    inline void operator()(Qualifier& qualifiers) 
    {
        T::operator()(qualifiers);
        Base::operator()(qualifiers);
    }
};


template<size_t I, auto F> requires std::is_function_v<std::remove_pointer_t<decltype(F)>>
struct deduce_qualifiers : public QualifierLayer<> {};




//When we handle it like this, we'll want to derive it from the qualifiers we want to use. 
// This system will NOT be used with references however, what they seek to represent able to be made iron clad, where the concept of readonly
// can never truly be represented at compile time.


constexpr size_t return_pos = (size_t)-1;


template<typename T, typename F> requires std::is_function_v<std::remove_pointer_t<F>>
struct parameter_index;


template<size_t I, typename T, typename... Args>
struct parameter_detect_layer : public
    std::conditional_t
    <std::is_same_v<std::tuple_element_t<I, std::tuple<Args...>>, T>,
        std::integral_constant<size_t, I>,
        std::conditional_t
        <I == sizeof...(Args) - 1,
            std::monostate,
            parameter_detect_layer<I + 1, T, Args...>
        >
    > {};


template<size_t I, typename T>
struct parameter_detect_layer<I, T> {};

template<typename T, typename R, typename... Args>
struct parameter_index<T, R(Args...)> : public std::conditional_t <
    std::is_same_v<R, T>,
    std::integral_constant<size_t, (size_t)-1>,
    parameter_detect_layer<0, T, Args...>
>
{};

template<typename T, typename R, typename... Args>
struct parameter_index<T, R(*)(Args...)> : public parameter_index<T, R(Args...)>
{};

constexpr auto loc2 = parameter_detect_layer<0, int, void, std::string, const int&, int>::value;

constexpr auto loc = parameter_index<int, void(std::string, const int&, int)>::value;



#define ADD_QUALIFIERS(mc_index, mc_func,...) template<>  \
struct deduce_qualifiers<mc_index, mc_func> __VA_OPT__( : public) QualifierLayer<__VA_ARGS__> {};

#define ADD_TYPE_QUALIFIERS(mc_type, mc_func,...) template<>  \
struct deduce_qualifiers<parameter_index<mc_type, decltype(mc_func)>::value, mc_func> __VA_OPT__( : public) QualifierLayer<__VA_ARGS__> {}


struct readonly { 
    void operator()(Qualifier& qualifiers) { qualifiers.MakeReadonly(false); }
};


ADD_QUALIFIERS(0, funct, readonly);
ADD_TYPE_QUALIFIERS(std::string_view, funct, readonly);




void LexTesting(std::string formula)
{
    Variable Test;

    
    ProjectManager::instance->InitMain();
 
    Script* script = ProjectManager::instance->GetShared()->GetCommons();

    TestParse(script);
    

    Component::LinkComponents(LinkFlag::Loaded);
    Component::LinkComponents(LinkFlag::Declaration);
	Component::LinkComponents(LinkFlag::Definition);
   
    
    //return;
    //ProjectManager::instance->GetFunctionFromPath("Shared::Commons::size");
    if (1)
    {
        if (ProcedureHandler::instance->RegisterCoreFunction(size_backend, "size") == false) {
            logger::debug("failure");
        }

        if (ProcedureHandler::instance->RegisterCoreFunction(size_for_int, "size") == false) {
            logger::debug("failure");
        }

        if (ProcedureHandler::instance->RegisterFunction(RefTest2, "Shared::Commons::RefTest2") == false) {
            logger::break_debug("failure");
        }
    }

    Initializer::Execute("function_register");

    Component::LinkComponents(LinkFlag::Object);
    Component::LinkComponents(LinkFlag::External);

    auto funcs = script->FindFunctions("GetActorValue");


    if (funcs.size() == 0)
    {
        logger::trace("_a");

    }


    if  constexpr (1)
    {
        if (ProcedureHandler::instance->RegisterCoreFunction(otherTest, "otherTest") == false)
        {
            logger::info("Function couldn't be set");
        }
    }

    ConcreteFunction* function = dynamic_cast<ConcreteFunction*>(funcs[0]);
    
    if (function)
    {
        //Testing the proceedure
        if constexpr (0)
        {
            if (ProcedureHandler::instance->RegisterFunction(GetActorValue_backend, function) == false)
            {
                logger::debug("failure");
                std::system("pause");
            }
            

        }

        //std::vector<Variable> args
        //{
        //    LEX::Number{1.0},
        //    LEX::Number{2.0},
        //    LEX::Number{3.0},
        //    LEX::Number{4.0},
        //    LEX::Number{5.0},
        //};
        
        //This is just to prove all versions of call work as desired.
        //Variable result = unit->Call(arguments);        
        //Variable result = function->Call(args);
        //Variable result = function->Call(args[0], args[1], args[2], args[3], args[4]);
        
        //function->_procedure = TestProcedure;
        //A conversion is supposed to happen here.
        
        double a_this = 68.0;
        
        Variable result = function->Call(Default{ 5 }, extern_ref(a_this), 1.0, 2.0, 3.0, 4.0, 5.0);


        logger::info("result of {} = {} (a_this = {})", formula, result.PrintString(), a_this);
        std::system("pause");
    }
    else
    {
        logger::info("Function couldn't be found in script");
    }
    
    if constexpr (0)
    {
        auto funcs = script->FindFunctions("TestBoolean");

        ConcreteFunction* function = dynamic_cast<ConcreteFunction*>(funcs[0]);

        if (function)
        {
            

            Variable result = function->Call();

            std::string number = result.AsNumber().string();

            logger::info("result of Test = {}", number);
        }
        else
        {
            logger::info("Function couldn't be found in script");
        }
    }

    std::string_view new_script = R"(
    float TestingManualCreation()
    {
        return 69 * 420;//nice.
    }

    float manual_object = 26;
    )";

    auto result = ProjectManager::instance->CreateScript(ProjectManager::instance->GetShared(), "Manual", "", new_script);

    logger::info("manual result >> {}", magic_enum::enum_name(result));


    TestForm();

    
    TestRun();

    //END OF THE CONTROLLED ENVIRONMENT
    return;
    //END OF THE CONTROLLED ENVIRONMENT
   
}


void SafeInvoke(std::function<void()> func)
{
    if (!func)
        return;
    
    if (IsDebuggerPresent() == false)
    {
        try {
            func();

        }//*
        catch (...) {
            logger::critical("critical error occured.");
            std::system("pause");
            std::exception_ptr p = std::current_exception();


            std::rethrow_exception(p);
        }
    }
    else {
        func();
    }
}



#ifdef FIALED_PROPERTIES
namespace PropertyTest
{

    //Property can possibly be a template basically. With the idea being that it has a version where it's inited
    // with a this, and thus given functional status, or one where it isn't and access to a varible is controlled
    //I think to control this there will be a difference between float and float()

    //Want to figured a good way to differ these 2. I think

    struct TestProp;


    inline std::vector<std::pair<void*, void*>> props;

    template<typename F, typename R, typename T>
    concept Getter = std::is_function_v<F> && requires(F func, T* t)
    {
        { func(t) } -> std::same_as<R>;
    };

    template<typename R, typename = void>
    struct Property
    {
        template<typename T>
        using Get = R(*)(T*);

        template<typename T>
        using Set = void(*)(T*, R);


        uint32_t offset = 0;
        uint32_t propSet = -1;
        //These will use a vector to contain the functions 

        Get<void> get = nullptr;
            
        Set<void> set = nullptr;

        void* GetSelf()
        {
            return this + offset;
        }


        Property<R>& operator= (R value)
        {
            set(GetSelf(), value);
            return *this;
        }

        operator R()
        {
            return get(GetSelf());
        }


        template <typename O>
        Property(O* a_this, void(*func)(O*))
        {
            


        }
        
        template <typename O>
        Property(O* a_this, Get<O> _get, Set<O> _set)//, O* a_this = this)
        {
            //offset = static_cast<uint32_t>(static_cast<uint64_t>(this) - static_cast<uint64_t>(a_this));


        }
        
    };

    template <typename T>
    concept prop_constraint = std::is_reference_v<T>;

    template<prop_constraint R>
    struct Property<R>
    {
        using Type = std::remove_reference_t<R>;

    private:
        Type value;

    public:

        //These will use a vector to contain the functions 

        //Property() = default;

        Property(void* something)
        {
            
        }
    };



    


    struct TestProp
    {
        
        Property<float> test
        {
            this,
            &[](auto*) ->void {}
        };

        
    };

    INITIALIZE()
    {
        TestProp test;
    }

}
#endif




int nested_func(int c)
{
    throw std::exception("we out");
    return c + 1;
}

int func(int b)
{
    return nested_func(b + 1);
}


//Varadic testing.

void VATest(int test...)
{
    int t = test;
}

using TestVA = decltype(VATest);



template<typename T>
using remove_runtype_t = std::conditional_t<std::is_same_v<std::remove_cvref_t<T>, runtime_type>, inherit_qualifier_t<Variable, T>, T>;



void TestingQualis(const int * const test)
{
    using Type = remove_runtype_t<const runtime_type&>;
    using Type1 = inherit_qualifier_t<Variable, const runtime_type*&>;
    decltype(test) cop{};
    inherit_qualifier<bool, decltype(test)>::type it{};
}
void TestingDumbness()
{
    return;
    auto run_form1 = Formula<runtime_type()>::Create("int", "1 + 4");
    auto run_form2 = Formula<runtime_type(runtime_type)>::Create("int", {"int", "param1"}, "1 + 4");
    auto run_form3 = Formula<runtime_type(runtime_type::*)()>::Create("int", "object", "1 + 4");
    auto run_form4 = Formula<runtime_type(runtime_type::*)(runtime_type&)>::Create("int", "object", { "int", "param1" }, "1 + 4");

    //return;
    logger::info("it {}", Formula<int>::RunDefault("4"));
    return;

    SyntaxRecord it;

    Parser::CreateSyntax<LineParser>(it, "20");

    auto handler = LEX::detail::FormulaBase<Formula<int(Variable, Variable, int)>, int, StaticTargetTag, runtime_type, runtime_type, int>::Create(
        { "int","a1" },
        { "int", "a2" },
        "a3",
        "something",
        it);


    if (handler) {
        int value = handler(1, 2, 3);
        logger::info("it {}", value);
    }
    else {
        logger::info("No it");
    }

}



int main(int argc, char** argv) {
    

    //logger::InitializeLogging(true);
#ifdef _DEBUG
    //Need a way to only have this happen when holding down a key
    if (GetKeyState(VK_RCONTROL) & 0x800) {
        constexpr  LPCSTR text1 = "Request for debugger detected. If you wish to attach one and press Ok, do so now if not please press Cancel.";
        constexpr LPCSTR text2 = "Debugger still not detected. If you wish to continue without one please press Cancel.";
        constexpr LPCSTR caption = "Debugger Required";

        int input = 0;

        do
        {
            input = MessageBoxA(NULL, !input ? text1 : text2, caption, MB_OKCANCEL);
        } while (!IsDebuggerPresent() && input != IDCANCEL);
    }
#endif
  
    Initializer::Execute("main_init");
    Initializer::Execute();
    //return 0;

    //GetTest<int64_t>();
    //LEX::Report<LEX::IssueType::Compile>::debug("The numbers {} and {} are {}", 69, 420, "nice");
    //std::system("pause");
    //return 0;
    
    SharedClient::instance->AddFormatter("TEST", [](IScript* script, const std::string_view& format, const std::string_view& content)
        {

            logger::info("format '{}' from '{}':\n{}", format, script->GetFullName(), content);
            return true;
        });

    SafeInvoke([&]() {
        //std::getline(std::cin >> std::ws, formula);
        LexTesting("GetValueTest");
        TestingDumbness();

        //Formula<float>::Run("'something'.size()", "ActorValueGenerator::Commons");

    });
   
    
    std::system("pause");
	return 0;
}




void KillMe()
{
    int i = 1;
    //std::vector<int&> test;
    //test.push_back(i);
    //std::span<int&> test2;
}

#include "RGL/Impl/macro_map.h"

#define VA_ARGALEAN "func1", "func2", "func2"

#define DO_LEAN(mc_string) auto CONCAT(doLean_,__COUNTER__) = mc_string;

MAP(DO_LEAN, VA_ARGALEAN)


//*
namespace LEX::Test
{
    struct Test1
    {
        //int operator+(int)
        //{
        //    return 0;
        //}
        int operator+(Test1)
        {
            return 0;
        }
        operator int()
        {
            return {};
        }

    };
    struct Test
    {
        //int operator+(int)
        //{
        //    return 0;
        //}
        int operator+(String)
        {
            return 0;
        }

        operator Test1()
        {
            return {};
        }
    };

    void Destroy()
    {
        Number other;
        std::string lhs;
        String rhs;

        Test test;

        //test + 1;
        //test + other;
        //test + rhs;

        //1 + test;
        //test + test;
    }

    void TestSomething(StaticTargetTag, variadic<int> params)
    {
        logger::info("Testing something (Vards)");
        int i = 0;
        for (auto& arg : params)
        {
            logger::info("{}: {}", ++i, arg);
        }
    }




    struct __declspec(dllimport)TestImex
    {
        int test = 1;
        [[maybe_unused]] virtual void TestFunc()
        {
            test = 2;
        }
    };

    INITIALIZE("function_register")
    {
        return;
        int num = 1;
        logger::info("success(TestSomething) => {}", num++, ProcedureHandler::instance->RegisterFunction(TestSomething, "Shared::Commons::TestSomething"));
        std::system("pause");

        std::tuple<StaticTargetTag, std::vector<int>> tups;

        auto result = BasicDispatcher<int, StaticTargetTag, variadic<int>>::tie_as_tuple<StaticTargetTag, variadic<int>>(tups);

    }

    struct DeepA
    {
        void foo() {}
    };

    struct ClassA : public virtual DeepA
    {

    };

    struct DeepB
    {

    };
    struct ClassB : public virtual DeepA
    {
        void foo() {}
    };

    struct TestIdea : public ClassA, public ClassB
    {

    };



    ENUM(DirectiveType)
    {
        Invalid
    };

    struct Directive
    {

    };



    //Here's the structure idea, I have, the interface functions are introduced in a base class
    // that can be attached to each respective thing, then there's an interface version
    // that uses proper inheritance
    //There's one problem with how this works, these base classes will be using the interface versions of these things, and the later ones
    // will be using the actual ones (at least in terms of some of them.

    //I could implement a similar thing to what I did before where I have a different name that I then override in a manner than prevents overlap 
    // in reimplementation. I could have the overriden in the INTERFACE_VERSION, and have the set up removed in SOURCE_CODE

    //An idea I had to get around the weirdness of declaring it later, is in the inheriting version if IProject and all those, it will cast itself into
    // the version of itself (using type traits) and call the OTHER version of itself, and will this return that. The only problem is specification.
    //^This procedure would only need to happen for the interface classes, like Element and Environment, because for the end of the line classes like 
    // script, the implementations will be as one.


    struct Subdirectory;





    /////////////////////////////////////////////






    //The interface pointer is a struct that 
    template<typename T>
    struct interface_ptr
    {
        using element_type = std::remove_const_t<T>;
        using pointer_type = element_type*;
        constexpr static uintptr_t kExpected = element_type::version;

        //When activated



        constexpr interface_ptr() noexcept = default;

        interface_ptr(pointer_type p) : _ptr{ p }
        {
            verify();
        }

        template<std::derived_from<element_type> U>
        interface_ptr(interface_ptr<U> p) : interface_ptr{ unconst(p.get()) }
        {
            verify();
        }


        interface_ptr(const interface_ptr<T>& p) : interface_ptr{ unconst(p.get()) }
        {
            verify();
        }



        pointer_type get() noexcept { return _ptr; }
        const pointer_type get() const noexcept { return _ptr; }

        operator pointer_type() noexcept { return get(); }
        operator const pointer_type() const noexcept { return get(); }

        pointer_type operator->() { return get(); }
        const pointer_type operator->() const noexcept { return get(); }


    private:

        void verify()
        {
#ifndef LEX_SOURCE
            assert_if(_ptr && _ptr->CheckVersion<element_type>(kExpected) == false) {
                report::link::critical("interface of '{}' failed to meet expected version of {}", type_name_v<element_type>, kExpected);
            }
#endif
        }

        pointer_type _ptr = nullptr;
    };

    interface_ptr<IProject> TestIFace()
    {
        return nullptr;
    }





    void TestV()
    {
        IProject::version;

        interface_ptr<IProject> test = TestIFace();

        interface_ptr<IElement> test2 = test;

        test->CheckVersion<IProject>(1);



    }




    










#pragma region Constants

    constexpr int TestConstexpr1();
    constexpr int TestConstexpr2()
    {
        return 1 + TestConstexpr1();
    }

    constexpr int TestConstexpr1()
    {
        return TestConstexpr2() - 1;
    }

    //constexpr int test_constexpr = TestConstexpr1();



    //Here's a concept that I'll be doing when it comes to const things, first, constant functions and constant variables exist 
    // on another plane basically. They cannot be external.  (Im unsure if I want to include object literals.)
    // But in linkage, it will link on definition but instead of declaration, it will link on a new linkage called constant.
    //When compiling for something that has a compiler option of  of constant, it can only use constant values, and expressions deemed constant.
    // Or rather, specific things will have a qualifier flag of "NotConstant" to mark that it cannot be used. Example would be, return from a non
    // const function, object literals. Also probably things that pull stuff like object infos and such.
    //Note, const stuff should be able to be saved into the script.

    //However, alternatively, I think one thing I can do is possibly have them link to a literal instead. it depends on where it's intended to be used.

    //Also, a rule on constant functions. They can be called any old way, even by non-constant functions obviously, however. When called from a constant
    // space, they can only be loaded with things and with methods that we know are constant.

    //Lastly, I'd like something called intrinsic for external functions. This means that the definition of the function is known at compile time
    // because the function is a core one, and thus can be used. The rule for this will likely just be thus, if the function is constant it
    // MUST be defined by the time constant occurs. If it hits constant, it will fail.

    //I want to consider situations in which I can allow it to have non-primative constants. But I'm not pressed about it.

    struct Constant;


    

    //The idea with constants should be that they all essentially go to the same spot, the same as
    // literals, only there's a wrapper for them so they can be accessed in normal situations.
    struct Constant : public DestructibleVarInfo
    {
        std::string_view GetName() const override
        {
            return "<constant>";
        }


        Literal* literal = nullptr;
    };

    //Constants are loaned out based on a literal they subscribe to.
    struct ConstantManager
    {
        std::map <Literal*, Constant> constants;
    };




#pragma endregion 


#pragma region GenericSignature
    
    
    
    
    struct SpecialArgument : public ITemplateBodyPart
    {
        inline static std::set<SpecialArgument> arguments;

        [[nodiscard]] static SpecialArgument* EmplaceArgument(std::span<ITypeInfo*> entries)
        {
            SpecialArgument arg{};

            arg._types = { entries.begin(), entries.end() };

            auto it = arguments.emplace(arg);

            return unconst(&*it.first);
        }


        virtual size_t GetSize() const { return _types.size(); }


        ITypeInfo* GetPartArgument(size_t i) const override
        {
            return _types[i];
        }

        TypeInfo* GetBodyArgument(size_t i) const override
        {
            if (GetState())
            {
                auto& type = _types[i];


                return NULL_OP(NULL_Q(type)->GetTypeInfo(nullptr));
            }

            return nullptr;
        }



        bool IsResolved() const override
        {
            //This is rare to ever actually be used, it merely exists on the one situation where it is.
            if (_state == kUnknown)
            {
                return AllResolved(_types);
            }

            return _state == kBody;
        }



        auto operator<=>(const SpecialArgument& other) const noexcept = default;

    private:
        std::vector<ITypeInfo*> _types;

    };

    void test_equal()
    {
        SpecialArgument test;
        SpecialArgument* test1{};
        SpecialArgument* test2{};

        *test1 == *test2;
    }

    
    struct GenericArugment : public ITemplatePart
    {
        std::set<SpecialArgument> arguments;
    };
    
    //A singleton type that basically is a stand in for the arguments of a given type.
    // The idea is depending on how many arguments 
    


#pragma endregion



    namespace ClassStructSystem
    {
        //This type would serve for something to link to a function with generic structures.
        // Like the ref types, this would basically alias the object. I don't think I'll require this however.
        template <typename Type, StringLiteral... TempArgs>
        using generic_type = int;

        template <StringLiteral... TempParams>
        struct GenericFunctionTag {};


        //Will be used to represent custom class objects, preventing it from being instantiated
        template <StringLiteral TypeName>
        using class_type = int;

        //Will be used to represent custom struct objects, preventing it from being instantiated
        template <StringLiteral TypeName>
        using struct_type = int;


        //This represents an enum value, with the type
        template <StringLiteral TypeName>
        using enum_type = int;


        struct test_struct_class
        {


            template <typename T>
            T member(std::string_view name)
            {
                //This is how one would access a class struct, however, instead of doing it like this, I'd like something similar to a native_reference,
                // a scripted reference. Which basically would conversely serve to take a native object and impose changes on a scripted variable.
                // notably, by doing this, I can have referenciable return types
                return {};
            }
        };



        namespace complete_reference_coverage
        {
            //This is an idea by which external references can cover native to native, scripted to native, and native to scripted references.
            // Think the issue of returning a reference parameter, but that parameter leads to a scripted object. That's what this attempts to resolve.
            // An object that serves as the handler for that, something similar to the local ref types, aliasing themselves as that object while serving
            // as a reference to either a scripted object or a native one.

            //
        }


        //When field data is going to die and a variable or runtime variable still 
        struct PreservedField
        {
            enum Type
            {
                kVariable,
                kRuntimeVar,
            };

            RuntimeVariable* data = nullptr;
            size_t size = 0;
            Type type{};

        };

        
        struct FieldData
        {
            using T = RuntimeVariable;

            T* data;

            ~FieldData()
            {
                Destroy();
            }

            bool TryPreserve()
            {
                //Check if any of the data has references, and if so move them to garbage collection
                return false;
            }

            void Destroy()
            {
                if (data) {
                    if (TryPreserve() == false) {
                        delete[] data;
                        data = nullptr;
                    }
                }
            }

            void Create(uint32_t size)
            {
                Destroy();

                data = new T[size];
            }

            void Transfer(T* other, uint32_t size, bool move = false)
            {
                for (int i = 0; i < size; i)
                {
                    if (move) {
                        data[i] = std::move(other[i]);
                    }
                    else {
                        data[i] = other[i];
                    }
                }
            }

            void Transfer(const T* other, uint32_t size)
            {
                return Transfer(unconst(other), size);
            }


            void Transfer(const void* other, uint32_t size)
            {
                //Shouldn't happen, this is just a dump overload.
            }
        };




        using BindCode = uint32_t;

        constexpr BindCode nil_bind_code = -1;



        struct BindID
        {
            InstanceID id = nil_instance_id;
            BindCode code = nil_bind_code;
        };
        
        struct StateID
        {
            uint32_t id{};
        };


        struct BindData;

        struct BindEntry
        {
            TypeInfo* type = nullptr;
            BindData* data = nullptr;
            Object bindObject{};
            //I could make this say if it has siblings or not in order to make features for getting siblings faster
            //Requires the entry to be made with the thing that created it

        };

        //This might be an interface that I'll use to interact with bind entry.
        struct BindData
        {
            
            BindID bindID{};
            Object bindTarget{};
            std::vector<BindEntry> entries;
        };



        ENUM(ObjectAccess, uint8_t)
        {
            Field,
            Method,
            Total,
        };

        struct AccessMemory
        {
            static constexpr uint32_t code = 0b111111111111;
            static constexpr uint32_t code_width = std::bit_width(code);

            static constexpr uint32_t fieldCode = code;
            static constexpr uint32_t methodCode = code << code_width;
            static constexpr uint32_t limit = fieldCode | methodCode;

            std::pair<uint32_t, uint32_t> GetMemory(ObjectAccess access)
            {
                uint32_t value = 0;
                uint32_t other = 0;
                std::memcpy(&value, &bytes, 3);

                if (access == ObjectAccess::Method)
                {
                    other = value & fieldCode;
                    value &= methodCode;
                    value >>= code_width;
                }
                else
                {
                    other = value & methodCode;
                    value &= fieldCode;
                }

                if (value >= code) {
                    value = -1;
                }

                return { value , other };
            }

            void SetMemory(ObjectAccess access, uint32_t value, uint32_t other)
            {
                if (value > code) {
                    value = code;
                }


                if (access == ObjectAccess::Method) {
                    value = other | (value << code_width);
                }
                else {
                    value = other | value;
                }
                assert(value <= limit);


                std::memcpy(&bytes, &value, 3);
            }

            std::array<std::byte, 3> bytes{};
        };


        //A base class for ScriptObjects and Attributes
        //NVM, just make this a script object
        struct CustomObject
        {
        public:
            enum Flag : uint8_t
            {
                kNone,
                kInitialized = 1 << 0,
                kDestructed = 1 << 1,
                HasBindData =  1 << 2,    //
            };

        protected:
        public:
            
            union
            {
                //BindEntry basically holds onto the type, as well as the bind data. This moves the burden of the extra pointer,
                // and for non-bound objects it will decrease the size (of which there will be more of them).
                uintptr_t _raw{};
                BindEntry* _entry;
                TypeInfo* _type;//Static cast to AttributeType if attribute. or use as actually. Easier.

            };

            FieldData fieldList;

            //I will only store RuntimeVariables on these, I believe the extra cost is worth it,
            // primarily to simplify access and 

            //Move access memory to be a thread local system
            AccessMemory recentAccess{};
            Flag flags = kNone;
            StateID stateID{};//If the state ID is invalid, this means it will use the main bind


        public:

            size_t size()
            {
                return 0;
            }

            void Instantiate(TypeInfo* self)
            {

            }



            bool GetMethod(const std::string_view& name, IFunction*& out)
            {
                return {};
            }

            bool GetMethod(MemberPointer member, IFunction*& out)
            {
                return {};
            }



            bool GetField(const std::string_view& name, RuntimeVariable& out)
            {
                return {};
            }

            bool GetField(MemberPointer member, RuntimeVariable& out)
            {
                return {};
            }

            bool IsRuntimeType() const
            {
                return false;
            }

            void Revert()
            {
                fieldList.Destroy();
                _type = nullptr;
                //size = 0;
            }


            void Transfer(const CustomObject& other)
            {
                Revert();
                _type = other._type;
                //size = other.size;
                //lhs.Create(other.size);
                //lhs.Transfer(rhs.data, other.size);


            }

        };




        struct ScriptObject : public CustomObject
        {

        protected:
            TypeInfo*& type()
            {
                return reinterpret_cast<TypeInfo*&>(_type);
            }
        public:




            //IDEA
            //Instead of the bind id being on everything, bind id will be used for stateIDs. Nah. 
            // this is a bad idea. I really would like to make some use out of this space though.





            ///I might use some extra flags for this, allowing it to easy denote things like having a bind class, or having a state at a later point.
            

            //I'm thinking this is how I'm going to handle this. A union that helps contro it being a variable pointer and a runtime pointer. I can then 
            // switch what type it's percieved as.

            //This might make it a pain however.



        };



        struct CustomObjectRep
        {

        };

        template <StringLiteral TypeName>
        using Class = int;

        //Will be used to represent custom struct objects, preventing it from being instantiated
        template <StringLiteral TypeName>
        using Struct = int;


        //This represents an enum value, with the type
        template <StringLiteral TypeName>
        using enum_type = int;



        struct IAttribute : public Interface, public LEX::IComponent
        {
            virtual TypeInfo* GetType() = 0;
            virtual Info* GetParent() = 0;
            
            virtual CustomObject* GetCustomObject() = 0;
            
            virtual bool GetField(std::string_view name, Variable& out) = 0;
        };


        struct AttributeType;

        struct Attribute : public Component, public IAttribute, public CustomObject
        {
            Info* parent = nullptr;

            CustomObject* GetCustomObject() override
            {
                return this;
            }


            AttributeType*& type()
            {
                return reinterpret_cast<AttributeType*&>(_type);
            }
        };





        //With no parameters this can be used, as it will contain no personal data of its own.
        struct AttributeType : public ConcreteType
        {
            //Was going to put this on here until I realized it would need to play catch up.

            //std::unique_ptr<Attribute> basicAttribute = nullptr;//Used when an attribute doesn't have contents
        };





        void MakeAttribute(TypeInfo* context)
        {
            AttributeType* type = nullptr;

            Attribute* attribute = nullptr;

            uintptr_t budget = (uintptr_t)type->GetFieldRange();

            attribute->fieldList.Create((uint32_t)budget);

            
            

            type->VisitTrees([&](InheritNode& node)
                {

                    if (auto members = node.tree->GetMembers())
                    {
                        size_t index = node.memberIndex;
                        size_t field_count = node.tree->GetFieldCount();

                        auto begin = attribute->fieldList.data + index;
                        auto end = begin + field_count;

                        std::span range{ begin , end };

                        for (int i = 0; i < field_count; budget--, i++)
                        {
                            auto& field = members->fields[i];
                            auto type = field.GetType();


                            auto real = type->GetTypeInfo(nullptr);


                            range[i] = real->GetDefault();
                        }
                        
                    }
                });

            assert_if (budget) {
                //Not proper.
            }

            
        }


        //THESE functions will no longer belong to Variable, they will belong to the class that handles membered able data classes
        //
        CustomObject* GetCustomData(Variable& a_this)
        {
            CustomObject* result = std::visit([](auto&& self) -> CustomObject* {
                using T = std::decay_t<decltype(self)>;

                if constexpr (std::is_same_v<T, IComponent*>) {
                    IAttribute* attribute = self->As<IAttribute>();
                    return attribute ? attribute->GetCustomObject() : nullptr;
                }
                else if constexpr (std::is_same_v<T, Object>) {
                    return nullptr;
                    //return self.get<ScriptObject>();
                }
                else {
                    return nullptr;
                }
                }, a_this.value());

            return result;
        }


        bool GetMemberField(Variable& a_this)
        {
            CustomObject* object = GetCustomData(a_this);


            if (!object) {
                return false;
            }
        }
        
        struct RunVarData
        {
            using SizeType = std::_Variant_index_t<std::variant_size_v<RunValue>>;

            static constexpr auto req_size = 8 - sizeof(SizeType);

            static constexpr uint32_t nil_offset = -1;

            //The offset is for the purposes of the 


            mutable uint32_t offset = nil_offset;//Offset is what 

        };
        static_assert(sizeof(RunVarData) <= RunVarData::req_size, "RunVarData must equal the size of the padding in RunTypes.");



        struct RunDataHelper
        {
            enum Flag
            {
                None = 0,
                Init = 1 << 0,
                Refr = 1 << 1,
                Ptr = 1 << 2,	//Should establish a pointer ref, and needs no ref value. Best used when it's unknown if var is a RuntimeVariable
                Free = 1 << 3,	//A given runtime variable has freed its index but retains a pointer.
            };

            enum Type
            {
                kInvalid,
                kVariable,
                kReference,
                kDetached,
                kExternal,
            };

            //This will help clear the Variable data spot without me having to put clear in every constructor. Hopefully.

            using _Ref = std::reference_wrapper<Variable>;


            static constexpr auto offset = sizeof(RunValue) - sizeof(RunVarData);
            //*
            RunVarData& GetData()
            {
                auto a_this = (uintptr_t)this;

                return *reinterpret_cast<RunVarData*>(a_this + offset);
            }

            const RunVarData& GetData() const
            {
                auto a_this = (uintptr_t)this;

                return *reinterpret_cast<RunVarData*>(a_this + offset);
            }
            //*/
            RunValue& GetValue()
            {
                return *reinterpret_cast<RunValue*>(this);
            }

            const RunValue& GetValue() const
            {
                return *reinterpret_cast<const RunValue*>(this);
            }

            Type index() const
            {
                return static_cast<Type>(GetValue().index());
            }

            bool IsReference() const
            {
                return index() == kReference;
            }


            const Variable* GetRefVariable() const
            {

                const RunValue& a_this = GetValue();

                switch (a_this.index())
                {
                case kReference:
                    return std::addressof(std::get<_Ref>(a_this).get());


                default:
                    return nullptr;
                }
            }


        protected:


        public:
            //bool IsRefNegated() const
            //{
            //	return index() == kReference && Refs();
            //}
        private:
            //void SetNegate(bool value) const 
            //{
                //auto var = GetRefVariable();
                //if (var)
                //var->GetData().refs = value ? -1 : 0;
            //}

        protected:

            //void TrySetNegated(bool value) const
            //{
            //	if (index() == kReference)
            //	{
            //		SetNegate(value);
            //	}
            //}

            void Unhandle()
            {
                if (index() == kReference)
                {
                    //if (!Refs())
                    GetRefVariable()->Dec();
                }
                else if (index() == kVariable)
                {
                    //if (auto refs = GetData().refs; refs) {
                    //	report::runtime::critical("{} refs remaining for run var ending {:X}", refs, (uintptr_t)this);
                    //}
                }
            }

            void Handle(const Variable& var)noexcept
            {
                //if (index() == kReference)
                {
                    //auto* help = other->GetRefHelper();
                    //if (!other->IsRefNegated())
                    var.Inc();
                }




            }

            void Handle(const RunDataHelper& other) noexcept
            {
                GetValue() = other.GetValue();

                if (auto var = other.GetRefVariable())
                {
                    Handle(*var);
                }
            }

        public:


            int32_t Refs() const
            {
                return GetRefVariable()->GetData().refs;//GetData().refs;
            }

            constexpr RunDataHelper() noexcept = default;

            ~RunDataHelper()
            {
                Unhandle();
            }

            RunDataHelper(const RunDataHelper& other)
            {
                Handle(other);

            }


            RunDataHelper(RunDataHelper&& other)
            {
                Handle(other);
            }


            RunDataHelper& operator=(const RunDataHelper& other)
            {
                Handle(other);
                Unhandle();
                return *this;
            }



            RunDataHelper& operator=(RunDataHelper&& other)
            {
                Handle(other);
                Unhandle();
                return *this;
            }

            RunDataHelper(const VariableRef& other)
            {
                Handle(other.get());
            }


            RunDataHelper(VariableRef&& other)
            {
                Handle(other.get());
            }


            /*
            void FUNDERSON() { logger::info("Funderson called"); }
            //I don't think this will actually do anything
            RunDataHelper& operator=(const VariableRef& other)
            {
                FUNDERSON();
                Unhandle();
                Handle(other.get());
                return *this;
            }
            RunDataHelper& operator=(VariableRef&& other)
            {
                FUNDERSON();
                Unhandle();
                Handle(other.get());
                return *this;
            }
            //*/
        };

        struct FakeRuntimeVariable : protected RunDataHelper
        {
            Variable var;


            const Variable& Ref() const
            {
                return var;
                //return const_cast<Variable&>(std::as_const(*this).Ref());
            }


            void AdjustOffset(TypeInfo* type = nullptr) const
            {
                auto& offset = GetData().offset;

                if (!type) {
                    offset = RunVarData::nil_offset;
                    return;
                }

                auto& value = Ref();

                TypeInfo* var_type = value.GetTypeInfo();

                assert_if(!var_type) {
                    //error
                    return;
                }

                if (var_type->IsScriptObject() == false) {
                    offset = RunVarData::nil_offset;
                    return;
                }


                //TODO: this needs to have a virtual function handle this part.
                {
                    auto var_tree = var_type->GetHierarchyTree();

                    auto i = var_tree->GetInheritIndex(type->GetHierarchyTree());

                    assert_if(i == -1) {
                        //error
                        return;
                    }

                    offset = static_cast<uint32_t>(i);
                }

            }
        };


        static void Adjust(RuntimeVariable& ret, Operand a_lhs, Operand a_rhs, InstructType, Runtime* runtime)
        {
            //Left doesn't matter, right should be 

            FakeRuntimeVariable& target  = reinterpret_cast<FakeRuntimeVariable&>(a_lhs.AsVariable(runtime));

            auto adjust_type = a_lhs.GetTypeInfo(runtime);

            assert(adjust_type);

            target.AdjustOffset(adjust_type);

        }


        void InlineRoutine(std::vector<Instruction>& instruction, RecordHolder* holder, RoutineBase* base)
        {
            
        }
    }
}
#include "Lexicon/Engine/TestToss.h"