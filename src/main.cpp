

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


#include "Lexicon/AttributeData.h"
#include "Lexicon/Engine/NativeAttribute.h"
#include "Lexicon/Interfaces/AttributeManager.h"
//
//#include "Lexicon/Engine/TempConstruct.cpp"
//

#include "Lexicon/Utility/Parameter.h"
#include "Lexicon/Primitive/Enumeration.h"
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








void TestCtor(Util::Attribute<"Core::TestAttribute">&& a_this, int number)
{
    a_this->GetType();
    
    logger::info("CREATED TEST ATTRIBUTE WITH {}", number);

    std::system("pause");
}


void LexTesting(std::string formula)
{
    Variable Test;

    
    ProjectManager::instance->InitMain();
 
    Script* script = ProjectManager::instance->GetShared()->GetCommons();

    TestParse(script);
    

    Component::LinkComponents(LinkFlag::Loaded);
    Component::LinkComponents(LinkFlag::Declaration);
    if (ProcedureHandler::instance->RegisterConstructor(TestCtor, "Core::TestAttribute") == false)
    {
        //This will not set, but I want to 
        report::critical("FUCK");
    }
    else
    {
        logger::critical("YAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAY!");
    }
	Component::LinkComponents(LinkFlag::Definition);
   
    
    //return;
    //ProjectManager::instance->GetFunctionFromPath("Shared::Commons::size");
    if (1)
    {
        if (ProcedureHandler::instance->RegisterFunction(size_backend, "Core::size") == false) {
            report::critical("failure");
        }

        //if (ProcedureHandler::instance->RegisterCoreFunction(size_for_int, "size") == false) {
        //    logger::debug("failure");
        //}

        if (ProcedureHandler::instance->RegisterFunction(size_for_int, "Core::size") == false) {
            report::critical("failure");
        }

        if (ProcedureHandler::instance->RegisterFunction(RefTest2, "Shared::Commons::RefTest2") == false) {
            logger::break_debug("failure");
        }
    }
    //assert(DirectoryManager::instance->GetComponentFromPath(ProjectManager::instance->GetShared(), "TestAttribute", ComponentType::TypeInfo));
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
        if (ProcedureHandler::instance->RegisterFunction(otherTest, "Core::otherTest") == false)
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



        namespace
        {
            struct IGarbage
            {
                virtual ~IGarbage() = default;

                //Bare in mind this should be how many objects are referenced ones, NOT how many references are on the objects in total.
                // That part doesn't really matter. The idea is that this number can ONLY go down.
                virtual size_t GetNumOfReferencedObjects() const = 0;


            };

            //template <typename T>
            //struct 



            struct GarbageCollector
            {
                //The idea is that once when the dereferences something, this number increments. When it passes a setting value
                // OR, when there's only one entry in GarbageCollection and the deref count is equal to that entries refCount, 
                // we force collection.
                //Once collection
                inline static std::atomic<size_t> derefCount = 0;;

                //Total refs is set each time it force collects,
                inline static std::atomic<size_t> interimRefCount = 0;

                inline static std::vector<std::unique_ptr<IGarbage>> collection;

                //I'd like this to be free when adding new garbage, but locked when forcing a collection. A read and write lock
                inline static std::shared_mutex lock;

                static void ForceCollect()
                {
                    std::unique_lock _{lock};

                    derefCount = 0;
                    interimRefCount = 0;
                   

                    auto end = collection.end();
                    auto it = std::remove_if(collection.begin(), end, 
                        [&](std::unique_ptr<IGarbage>& garbage)
                        {
                            size_t result = garbage->GetNumOfReferencedObjects();
                            interimRefCount += result;
                            return !result;
                        });
                    
                    if (it != end)
                        collection.erase(it, end);
                }

                static void AddGarbage(std::unique_ptr<IGarbage>&& add)
                {
                    std::shared_lock _{ lock };

                    if (auto refs = add->GetNumOfReferencedObjects(); refs == 0) {
                        add.reset();
                    }
                    else {
                        collection.push_back(std::move(add));
                        if (!interimRefCount) {
                            interimRefCount = refs;
                        }
                    }
                }

                static void DecrementReference(size_t dec = 1)
                {
                    derefCount += dec;

                    if (derefCount >= interimRefCount) {
                        assert(derefCount == interimRefCount);

                        ForceCollect();
                    }
                }




            };


            struct ref_counter
            {
                template<typename T>
                size_t operator()(const T& it)
                {
                    if constexpr (std::is_same_v<T, Variable>) {
                        return it.GetRefCount();
                    }
                    else if constexpr (std::is_same_v<T, RuntimeVariable>) {
                        return !it.IsVoid() ? it->GetRefCount() : 0;
                    }
                    else {
                        static_assert(std::is_same_v<T, T>, "Unsupported referencable detected.");
                    }

                }
            };

            template <typename T1, typename T2>
            struct CollectionGarbage : public IGarbage
            {

                using element_type = T1;
                using pointer_type = element_type*;

                using counter_type = T2;


                std::span<element_type> range;
                

                CollectionGarbage(std::span<element_type> r) : range{ r }
                {
                }
                

                size_t GetNumOfReferencedObjects() const override
                {
                    counter_type counter{};
                    size_t result = 0;

                    for (auto& it : range) {
                        result += counter(it);
                    }

                    return result;
                }
            };


            void TestRange()
            {
                auto test = std::ranges::iota_view{ 1, 10 };
            }



            //Instead of garbage collection I could keep these via custom pointer and ditch the collection
            // saving, that way I don't have to maintain the entire collection of variables. This can 
            // be useful on particularly large objects, or particularly large arrays that still have data
            // referenced in them.

            template <typename T>
            struct CollectibleData
            {
                using element_type = T;
                using pointer_type = element_type*;

            private:
                union
                {
                    uintptr_t _raw{};
                    element_type* _data;
                };



            public:
                ~CollectibleData()
                {
                    Destroy();
                    //assert_if(IsDataDestroyed() == false) {
                    //    report::fault::critical("Failed to destruct CollectibleData, innate data not destroyed");
                    //}
                }

                CollectibleData() noexcept = default;
                //Since these do not know their size they must be manually implemented
                CollectibleData(const CollectibleData&) = delete;
                CollectibleData(CollectibleData&& other) noexcept : 
                    _data{ std::exchange(other._data, nullptr) }
                {}

                CollectibleData& operator=(const CollectibleData&) = delete;
                CollectibleData& operator=(CollectibleData&& other) noexcept
                {
                    _data = std::exchange(other._data, nullptr);
                    return *this;
                }

                //Pls make the operators, thx


                element_type* data() noexcept
                {
                    return _data;
                }

                const element_type* data() const noexcept
                {
                    return unconst(this)->data();
                }


                element_type& operator[](size_t i)noexcept { return _data[i]; }
                const element_type& operator[](size_t i) const noexcept { return _data[i]; }

                std::span<element_type> range(size_t size)
                {
                    if (auto ptr = data()) {
                        return std::span<element_type>{ ptr, size };
                    }

                    return {};
                }


                constexpr operator bool() const noexcept
                {
                    return _data;
                }


                void Destroy()
                {
                    if (auto ptr = data()) {
                        delete[] ptr;
                        _data = nullptr;
                    }
                }

                void Create(size_t size)
                {
                    Destroy();

                    if (size)
                        _data = new element_type[size];
                }

                [[nodiscard]] pointer_type release()
                {
                    return std::exchange(_data, nullptr);
                }

            private:
                void Transfer(element_type* other, size_t size, bool move = false)
                {
                    for (int i = 0; i < size; i)
                    {
                        if (move) {
                            _data[i] = std::move(other[i]);
                        }
                        else {
                            _data[i] = other[i];
                        }
                    }
                }
            public:
                void Copy(const element_type* other, size_t size)
                {
                    Transfer(unconst(other), size, false);
                }

                void Move(element_type* other, size_t size)
                {
                    Transfer(other, size, true);
                }


            };



        }
        









        using BindCode = uint32_t;

        constexpr BindCode nil_bind_code = -1;



        struct BindID
        {
            InstanceID id{};
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


        template<size_t N>
        struct StringHash {
            constexpr StringHash(const char(&str)[N]) {
                value = std::hash<decltype(str)>{}(str);
            }

            size_t value;
        };


        //The gist of this object is that I want it to work in 3 parts.
        //The LockID, the LockHandle, and the LockManager
        //-The LockID functions as a way to detect if there's already an active thread
        //  handling the lock, if there is we pull that lock up and then wait for it to be done.
        //  If not, it will make a new thread lock. If it runs out of space, 
        //  it will wait until one of the spaces is free. Index 0 is the size lock, which will be
        //  released once any of the locks release.
        //-LockHandle serves as the function to tell if we've stopped needing to lock the object,
        //  as well as the thing that tells us there's free space
        //-The manager takes these in through functions (not freeing stuff though, that role is given
        // through 
        //template<uint32_t Category>
        struct LockIndex
        {
            friend class TestLockManager;
            
            ~LockIndex()
            {
                assert_if(id) {
                    report::fault::critical("LockIndex destroyed before being released.");
                }
            }

            bool IsActive() const noexcept { return id; }

            //TODO:Index is not allowed to die while it isn't 0
            
            //For once 0 will actually be a free space
            mutable std::atomic<uint8_t> id = 0;
        };


        struct LockHandle
        {
            using Dtor = void(const LockIndex*, size_t, uint8_t);

            const LockIndex* id = nullptr;
            size_t hash = 0;
            Dtor* dtor = nullptr;
            uint8_t index = 0;

            constexpr LockHandle() noexcept = default;

            LockHandle(const LockIndex* a1, size_t a2, Dtor* a3) noexcept : id{ a1 }, hash{ a2 }, dtor{ a3 }, index{ a1->id }
            {

            }

            constexpr LockHandle(const LockHandle&) noexcept = default;
            
            LockHandle(LockHandle&& handle) noexcept : LockHandle{ (LockHandle&)handle }
            {
                handle.dtor = nullptr;
            }


            ~LockHandle()
            {
                if (dtor) {
                    dtor(id, hash, index);
                }
            }
        };


        struct LockEntry
        {
            std::thread::id thread{};
            uint8_t index{};
            uint8_t left = 0;
            uint8_t right = 0;
            bool ownerDeleted = false;//If the owner deletes itself early PushNextEntry will return false, preventing any interaction with the lockID
            std::recursive_mutex mutex;


            uint8_t GetNext() const
            {
                return left ? left : right;
            }
            LockEntry() = default;

            void SetIndex(uint8_t i)
            {
                index = i;

                if (index) {
                    left = index - 1;
                }
                else if (index < 255) {
                    right = index + 1;
                }
            }

        };

        using Mutex = std::recursive_mutex;

        struct TestLockManager
        {

            static void DestructLockImpl(const LockIndex* lock)
            {

                lock->id = 0;

            }

            static void DestructLock(const LockIndex* lock, size_t hash, uint8_t index)
            {

                auto it = categories.find(hash);
                
                assert_if (categories.end() == it) {
                    
                }

                if (it->second.IsOwnerDeleted(index) == false)
                    DestructLockImpl(lock);

            }

            static void DeactivateLock(const LockIndex* lock, size_t hash, uint8_t index)
            {

                auto it = categories.find(hash);

                assert_if(categories.end() == it) {

                }

                if (it->second.PushNextEntry(index) == true)
                    DestructLockImpl(lock);
            }


            

            struct LockSet
            {
                static constexpr uint8_t lockCount = 20;

                std::unique_ptr<LockEntry[]> locks = nullptr;
                uint8_t size = 0;
                uint8_t nextEntry = 1;

                std::mutex mutex;
                std::unordered_map <std::thread::id, LockEntry*> activeLocks;

                LockSet()
                {
                    AllocateLocks(lockCount);
                }


                void AllocateLocks(uint8_t count)
                {
                    count++;
                    
                    locks.reset(new LockEntry[count]);

                    for (uint8_t i = 0; i < count; i++){
                        locks[i].SetIndex(i);
                    }
                }


                bool PushNextEntry(uint8_t id)
                {
                    std::lock_guard guard{ mutex };

                    activeLocks.erase(std::this_thread::get_id());
                    auto& entry = locks[id];
                    entry.thread = std::thread::id{};
                    entry.mutex.unlock();

                    bool deleted = std::exchange(entry.ownerDeleted, false);

                    //This will handle the unlock too

                    auto old_index = nextEntry;



                    auto& self = locks[nextEntry];

                    locks[self.left].right = id;
                    locks[self.right].left = id;

                    

                    nextEntry = id;


                    if (!old_index) {
                        locks[0].mutex.unlock();
                    }

                    return !deleted;
                }

                uint8_t PopNextEntry()
                {
                    if (!nextEntry) {
                        std::lock_guard guard{ mutex };
                    }

                    std::lock_guard guard{ mutex };

                    uint8_t result = nextEntry;


                    auto& self = locks[nextEntry];

                    locks[self.left].right = self.right;
                    locks[self.right].left = self.left;

                    nextEntry = self.GetNext();
                    //This should prep the next entry, if the next entry is unavailable but we have a valid,
                    // next entry, it will lock the core lock. If next is valid and we've reached this point
                    // that is an assert failure.
                    return result;
                }

                LockEntry* GetLockEntry()
                {
                    std::lock_guard guard{ mutex };

                    auto it = activeLocks.find(std::this_thread::get_id());
                    auto end = activeLocks.end();
                    if (it != end) {
                        return it->second;
                    }

                    return nullptr;
                }







                //Need a function for for setting an active lock and freeing it.
                // I don't want to run into deadlocking issues.

                //Requires lock
                void RegisterLock(const LockIndex& lock, uint8_t id)
                {
                    //std::lock_guard guard{ mutex };

                    lock.id = id;
                    locks[id].mutex.lock();
                    locks[lock.id].thread = std::this_thread::get_id();;

                }

                bool IsOwnerDeleted(uint8_t index)
                {
                    return locks[index].ownerDeleted;
                }

                void DestroyLock(const LockIndex& lock, size_t hash)
                {
                    locks[lock.id].ownerDeleted = true;
                    lock.id = 0;
                }

                LockHandle HandleLock(const LockIndex& lock, size_t hash)
                {
                    if (lock.id) {
                        if (std::this_thread::get_id() != locks[lock.id].thread) {
                            //Already locked elsewhere and we own it so we 
                            return LockHandle{};
                        }

                        std::lock_guard guard{ locks[lock.id].mutex };
                    }

                    if (LockEntry* entry = GetLockEntry()) {
                        RegisterLock(lock, entry->index);
                        return LockHandle{ &lock, hash, DestructLock };
                    }

                    RegisterLock(lock, PopNextEntry());

                    return LockHandle{ &lock, hash, DeactivateLock };
                }
            };



            inline static std::unordered_map<size_t, LockSet> categories;

            inline static std::mutex mutex;

            static LockSet& GetLockSet(size_t hash)
            {
                std::lock_guard guard{ mutex };

                return categories[hash];
            }


            static void DestroyLock(const LockIndex& lock, size_t hash)
            {
                LockSet& set = GetLockSet(hash);
                return set.DestroyLock(lock, hash);

            }

            static LockHandle HandleLock(const LockIndex& lock, size_t hash)
            {
                LockSet& set = GetLockSet(hash);
                return set.HandleLock(lock, hash);
            }
        };

        template<uint32_t I>
        struct LockID : public LockIndex
        {
            static constexpr uint32_t CODE = std::byteswap(I);
            //static constexpr char name[5]{}


            constexpr size_t hash() const noexcept
            {
                return CODE;
            }

            [[nodiscard]] LockHandle Lock() const
            {
                return TestLockManager::HandleLock(*this, hash());
            }

            constexpr LockID() noexcept = default;
            constexpr LockID(const LockID&) noexcept {}
            constexpr LockID(LockID&&) noexcept {}

            constexpr LockID& operator=(const LockID&) noexcept { return *this; }
            constexpr LockID& operator=(LockID&&) noexcept { return *this; }


            ~LockID()
            {
                if (IsActive() == true) {
                    //This prevents LockIndex from asserting if the lock is still active.
                    TestLockManager::DestroyLock(*this, hash());
                }
            }
        };

        
        struct ScriptObject
        {
        public:
            enum Flag : uint8_t
            {
                kNone,
                kInitialized = 1 << 0,
                kDestructed = 1 << 1,
                kHasBindData =  1 << 2,    //
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

            CollectibleData<RuntimeVariable> _fields;

            //I will only store RuntimeVariables on these, I believe the extra cost is worth it,
            // primarily to simplify access and 
            
            
            Flag flags = kNone;
            //Move access memory to be a thread local system
            LockID<'SOBJ'> lock{};
            uint8_t bytes[2]{};
            StateID stateID{};//If the state ID is invalid, this means it will use the main bind


        public:

            ScriptObject(TypeInfo* type) : _type{ type }
            {
                _fields.Create(type->GetFieldCount());
            }


            ~ScriptObject()
            {
                if (HasFlag(Flag::kDestructed) == false) {
                    //Call destruct
                    Destruct();
                }


            }



        INTERNAL:
            std::span<RuntimeVariable> fields(size_t offset = 0, size_t count = std::dynamic_extent)
            {
                std::span<RuntimeVariable> results = _fields.range(size());
                
                return results.subspan(offset, count);
            }



            void Destruct()
            {
                SetFlag(Flag::kDestructed, true);
            }
        public:

            constexpr bool HasFlag(Flag flag) const noexcept
            {
                return flags & flag;
            }

            void SetFlag(Flag flag, bool value) noexcept
            {
                if (value)
                    flags |= flag;
                else
                    flags &= ~flag;
            }
            


            constexpr TypeInfo* type() const noexcept
            {

                if (HasFlag(Flag::kHasBindData) == true)
                    return _entry->type;
                else
                    return _type;
            }


            size_t size() const
            {
                if (auto a_type = type()) {
                    return a_type->GetFieldCount();
                }

                return 0;
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
                _fields.Destroy();
                _type = nullptr;
            }

            //This gets complicated with bind objects
            void Transfer(const ScriptObject& other)
            {
                Revert();
                _fields.Create(other.size());
                _type = other._type;
            }


            //IDEA
            //Instead of the bind id being on everything, bind id will be used for stateIDs. Nah. 
            // this is a bad idea. I really would like to make some use out of this space though.





            ///I might use some extra flags for this, allowing it to easy denote things like having a bind class, or having a state at a later point.


            //I'm thinking this is how I'm going to handle this. A union that helps contro it being a variable pointer and a runtime pointer. I can then 
            // switch what type it's percieved as.

            //This might make it a pain however.


        };
        REQUIRED_SIZE(ScriptObject, 0x18);

        /*
        //DO not delete, this is simply in the wrong namespace to be valid right now.
        //Script object itself is carried by pointer, to prevent creation.
        template <>
        struct VariableType<ScriptObject*>
        {
            TypeInfo* operator()(const ScriptObject* it = nullptr)
            {
                if (it) {
                    return it->type();
                }

                return nullptr;;
            }

        };


        void DoTest()
        {
            GetVariableType<ScriptObject*>();
        }

        template <>
        struct LEX::ObjectInfo<ScriptObject> : public QualifiedObjectInfo<ScriptObject>
        {
            

            TypeInfo* GetOverrideType(ObjectData& data) override
            {
                ScriptObject& object = get(data);

                return object.type();
            }


            TypeOffset GetTypeOffset(ObjectData& data) override
            {
                return GetOverrideType(data)->GetTypeID();
            }


            TypeInfo* SpecializeType(ObjectData& data, ITypeInfo* type) override
            {
                return GetOverrideType(data);
            }

            //the form object info needs to edit the transfer functions,


            String PrintString(ObjectData& a_self, std::string_view context) override
            {
                return "ScriptObject()";
            }


        };



        template <>
        struct Revariable<ScriptObject*>
        {

            void operator()(ScriptObject*& arg, Variable* var)
            {
                if (var->IsObject() == true) {
                    Object& object = var->AsObject();

                    arg = std::addressof(object.get<ScriptObject>());
                }
                //Is component
                report::error("Cannot address error here");
                
            }
        };
        //*/


        struct ArrayVariable : protected RefVariable
        {
            ArrayVariable() : RefVariable{ nullptr } {}

            using RefVariable::operator=;

            ArrayVariable& operator=(const Variable& other)
            {
                obtain() = other;
                return *this;
            }

            ArrayVariable& operator=(Variable&& other)
            {
                obtain() = std::move(other);
                return *this;
            }


            Variable& obtain()
            {
                if (auto ptr = get()) {
                    return *ptr;
                }

                *this = detach({});

                return ref();
            }

            const Variable& obtain() const
            {
                return unconst(this)->obtain();
            }

            Variable* ptr()
            {
                return std::addressof(obtain());
            }

            const Variable* ptr() const
            {
                return unconst(this)->ptr();
            }



            Variable* operator->() noexcept
            {
                return ptr();
            }

            const Variable* operator->() const noexcept
            {
                return ptr();
            }

            operator Variable&()
            {
                return obtain();
            }

            operator const Variable&() const
            {
                return obtain();
            }
        };


        //*   

        //I think this version of array should actually be intrinsic
        struct Array
        {
        public:

            OBJECT_INFO_DATA(1)
            {
                "ARRAY", 1
            };


        public:
            
            //This will likely be just an object array.
            Array() noexcept = default;


            Array(TypeInfo* type, uint32_t a_size) : _type{ type }
            {
                resize(a_size);
            }

            Array(TypeInfo* type) : Array{type, 0} {
            }


            Array(const Array& other) : Array{ other.type() }
            { 
                copy(other); 
            }
            
            Array(Array&& other) :
                _data{ std::move(other._data) },
                _type{ other._type },
                _size{ std::exchange(other._size, 0) }
            {}
            
            template <typename T> requires (!std::is_base_of_v<LEX::detail::not_implemented, Revariable<T>>)//stl::castable_from<Variable>
            Array(std::vector<T>& other) : Array{GetVariableType<T>(), (uint32_t)other.size() }
            {
                for (uint32_t i = 0; i < _size; i++) {
                    Revariable<T>{}(other[i], std::addressof(_data[i].obtain()));
                }
            }
            template <typename T> requires (!std::is_base_of_v<LEX::detail::not_implemented, Revariable<T>>)//stl::castable_from<Variable>
            Array(std::vector<T>&& other) : Array{ other } {
            }


            Array& operator=(const Array& other)
            {
                copy(other);
                return *this;
            }
            Array& operator=(Array&& other)
            {
                move(std::move(other));
                return *this;
            }



            template <typename T> requires (!std::is_base_of_v<LEX::detail::not_implemented, Unvariable<T>>)//stl::castable_from<Variable>
            explicit operator std::vector<T>() const
            {
                if (!_size)
                    return {};


                if constexpr (std::is_same_v<T, Variable>)
                {
                    return std::vector<T>{_data.data(), _size };
                }
                else {
                    auto begin = _data.data();
                    auto end = begin + _size;

                    std::vector<T> result{};

                    result.resize(data.size());

                    std::transform(begin, end, result.begin(), [](ArrayVariable& it)
                        {
                            return Unvariable<T>{}(it.get());
                        });

                    return result;
                }

            }

        public:


            std::string PrintString(const std::string_view& context) const
            {
                //std::vector<std::string> entries{ size };
                std::string result = "[";


                if (_data)
                {

                    for (int i = 0; i < _size; i++)
                    {
                        if (i)
                            result += ", ";

                        result += _data[i]->PrintString();


                    }
                }
                result += "]";

                //Ypu've got all these fancy ways to do this, but I'm just gonna do this for now and see if that works.
                return result;


            }



            ArrayVariable* data()
            {
                return _data.data();
            }

            const ArrayVariable* data() const
            {
                return _data.data();
            }

            void resize(uint32_t new_size)
            {
                auto guard = _lock.Lock();

                if (_size == new_size) {
                    return;
                }

                std::unique_ptr<ArrayVariable[]> old_data{ _data.release() };
                
                _data.Create(new_size);

                if (_size) {
                    assert(old_data);
                    
                    //This should steal the pointers.
                    for (uint32_t i = 0; i < _size; i++) {
                        _data[i] = std::move(old_data[i]);
                    }
                }


                if (new_size > _size) {
                    TypeInfo* type = this->type();

                    for (uint32_t i = _size; i < new_size; i++) {
                        _data[i] = RefVariable{ detach(type->GetDefault()) };
                    }
                }
            }


            //I don't know how I want to handle access just yet, I think when it comes to
            // the comings and goings I want it to be strict about what is allowed to assign,
            // or what references are given out. I think I'll give them in the form of a RuntimeVariable.




            TypeInfo* type() const noexcept
            {
                return _type;

            }


            size_t size() const
            {
                return _size;
            }

            void clear()
            {
                _data.Destroy();
                _size = 0;
            }
        private:

            void copy(const Array& other)
            {
                auto data = other.data();
                auto size = other.size();

                if (_size != size)
                    _data.Create(size);

                _data.Copy(data, size);
            }


            void move(Array&& other)
            {
                _data = std::move(other._data);
                _type = other._type;
                _size = std::exchange(other._size, 0);
            }


        private:


            CollectibleData<ArrayVariable> _data;
            TypeInfo* _type = nullptr;
            uint32_t _size = 0;
            LockID<'SOBJ'> _lock{};
            std::byte freeSpace[3]{};



        };
        REQUIRED_SIZE(Array, 0x18);
        //*/


        struct ObjectSettings_Array : public ObjectInfo<Array>
        {
            OBJECT_INFO_DATA(1)
            {
                "ARRAY", 1
            };

            template <specialization_of<std::vector> Vec>
            static Array ToObject(const Vec& obj)
            {
                std::vector<Variable> buff;
                buff.reserve(obj.size());
                //const std::vector<void*> test;

                //void* other = test[1];


                std::transform(obj.begin(), obj.end(), std::back_inserter(buff), [&](auto it) {return it; });


                return Array{ buff };

            }


            TypeOffset GetTypeOffset(const ObjectParams& data) override
            {
                return data.get<Array>().type() != nullptr;
            }


            TypeInfo* SpecializeType(const ObjectParams& data, ITypeInfo* type) override
            {
                TypeInfo* result;

                if (type->IsResolved() == false) {

                    GenericArray array{ nullptr, {data.get<Array>().type()} };

                    auto result = type->GetTypeInfo(array.TryResolve());

                    if (!result) {
                        report::error("Failed to specialized 'ARRAY' offset of 1.");
                    }

                }
                else {
                    result = type->GetTypeInfo(nullptr);
                }

                return result;
            }

            //the form object info needs to edit the transfer functions,


            String PrintString(const ObjectParams& a_self, std::string_view context) override
            {
                return a_self.get<Array>().PrintString(context);
            }

            /*
            //This was mere test data
            bool CreateLiteralData(std::string_view literal, uintptr_t& hash, ObjLitCtor& ctor) override
            {
                auto func = [](std::string_view lit) -> Object
                    {

                        std::vector<Variable> result { std::string(lit)};
                        return Array{ result };
                        //No idea why this doesn't work
                        //return ObjectTranslator<decltype(result)>{}(result);
                    };


                hash = std::hash<std::string_view>{}(literal);
                ctor = func;
                return true;
            }
            //*/
        };

        template<typename T>
        struct ProxyGuide {};

        template<typename T>
        struct ProxyGuide <std::vector<T>> : public RefCollection
        {

            TypeInfo* VariableType(const std::vector<T>* vec)
            {
                return IdentityManager::instance->GetTypeByOffset("ARRAY", 0)->GetTypeInfo(nullptr);
                //TODO: This literally does not work, please implement this properly.
                //return Array::GetVariableType(vec);
            }

            Array ObjectTranslator(const std::vector<T>& obj)
            {
                std::vector<Variable> buff;
                buff.reserve(obj.size());
                //const std::vector<void*> test;

                //void* other = test[1];


                std::transform(obj.begin(), obj.end(), std::back_inserter(buff), [&](auto it) {return it; });


                return Array{ buff };

            }






            //TODO:Unboiler plate revariable pls, k thx
            void Revariable(const std::vector<T>& arg, Variable* var)
            {
                Array& array = var->AsObject().get<Array>();

                //We are making some assumptions here, and doing no checks

                if (auto size = array.size(); arg.size() != size) {
                    report::runtime::error("const array's size was adjusted erroneously.");
                }

                auto data = array.data();

                for (size_t i = 0; i < arg.size(); i++)
                {
                    auto& entry = arg[i];
                    auto& to = data[i];

                    LEX::Revariable<const T> revar;

                    revar(entry, to.ptr());

                    Collect(std::addressof(entry), to.ptr());

                    TryToCollect(revar);
                }
            }



            void Revariable(std::vector<T>& arg, Variable* var)
            {
                Array& array = var->AsObject().get<Array>();

                //We are making some assumptions here, and doing no checks

                if (auto size = array.size(); arg.size() != size) {
                    array.resize(size);
                }

                auto data = array.data();

                for (size_t i = 0; i < arg.size(); i++)
                {
                    auto& entry = arg[i];
                    auto& to = data[i];

                    LEX::Revariable<T> revar;

                    revar(entry, to.ptr());

                    Collect(std::addressof(entry), to.ptr());

                    TryToCollect(revar);
                }
            }


        };

        struct Revariable_Array : public RefCollection
        {
            void Fill(const Array& a_this, Array& other, bool assign)
            {

                auto this_data = a_this.data();
                auto other_data = other.data();

                for (size_t i = 0; i < a_this.size(); i++)
                {
                    auto& entry = this_data[i].obtain();
                    auto& to = other_data[i].obtain();

                    Collect(std::addressof(entry), std::addressof(to));

                    if (const Object* object = entry.FetchObject()) {
                        if (auto entry_array = object->fetch<Array>()) {

                            Fill(*entry_array, std::addressof(to), assign);
                            continue;
                        }
                    }

                    if (assign)
                        to.Assign(entry);

                }
            }

            void Fill(const Array& arg, Variable* var, bool assign)
            {
                Array& arg_array = var->AsObject().get<Array>();

                //We are making some assumptions here, and doing no checks

                if (auto size = arg.size(); arg_array.size() != size) {
                    if (assign)
                        arg_array.resize(size);
                    else
                        report::runtime::error("const array's size was adjusted erroneously.");
                }

                return Fill(arg, arg_array, assign);
            }


            void operator()(const Array& arg, Variable* var)
            {
                return Fill(arg, var, false);
            }

            void operator()(Array& arg, Variable* var)
            {
                return Fill(arg, var, true);
            }
        };

        template <typename T> requires (std::is_same_v<std::remove_cvref_t<T>, Array> && (std::is_const_v<T> || std::is_reference_v<T>))
            struct Revariable<T> : public Revariable<Array> {};


        void TestArray()
        {
            std::vector<int> test = {};

            Array array = test;
        }


        template <typename T>
        struct chain_object
        {
            using self = chain_object<T>;
            using element_type = T;
            using pointer_type = T*;
            using reference_type = T&;


            element_type& value() noexcept
            {
                return _value;
            }

            const element_type& value() const noexcept
            {
                return _value;
            }

        private:
            chain_object(const element_type& v, self** p) : _value{ v }, _pos{ p }, _prev { p ? *p : nullptr }
            {
                if (_pos)
                    *_pos = this;
            }
        public:
            chain_object(element_type v, self*& p) : chain_object{ v, std::addressof(p) } {}

            chain_object(element_type v) : chain_object{ v, nullptr } {}



        private:
            chain_object(const chain_object&) = default;
        public:
            chain_object(chain_object&& other) : chain_object(other)
            {
                if (*_pos)
                    *_pos = this;

                other._pos = nullptr;
            }

            ~chain_object()
            {
                if (_pos)
                    *_pos = _prev;
            }


        protected:
            element_type _value;
            self* const _prev = nullptr;
            self** _pos = nullptr;

        };



        namespace
        {

            struct IExternReference
            {
                enum
                {
                    kValue,
                    kDelegate,
                };


                virtual ~IExternReference() = default;

                virtual void Update(bool delegate) = 0;
                
                virtual Variable& GetDelegate() = 0;
                const Variable& GetDelegate() const { return make_const(this)->GetDelegate(); }


                void UpdateDelegate()
                {
                    Update(kDelegate);
                }


                void UpdateValue()
                {
                    Update(kValue);
                }


                size_t ModRefCount(bool inc = true) const
                {
                    size_t result = _refs += inc ? 1 : -1;

                    assert(_refs >= 0);

                    if (!_refs)
                        delete this;

                    return result;
                }
            private:
                mutable std::atomic<intptr_t> _refs{};
            };

            /// <summary>
            /// A class that represents an externally accessible variable
            /// </summary>
            struct ExternVariable
            {
                ~ExternVariable() { Unhandle(); }

                ExternVariable(IExternReference* ref) : _ref{ ref }
                {
                    _ref->ModRefCount(true);
                }



                ExternVariable(const ExternVariable& other)
                {
                    Transfer(other, true);
                }


                ExternVariable(ExternVariable&& other)
                {
                    Transfer(other, false);
                }


                ExternVariable& operator=(const ExternVariable& other)
                {
                    if (_ref != other._ref)
                        Unhandle();

                    Transfer(other, true);
                    return *this;
                }

                ExternVariable& operator=(ExternVariable&& other)
                {
                    if (_ref != other._ref)
                        Unhandle();

                    Transfer(other, false);
                    return *this;
                }

                constexpr operator bool() const noexcept
                {
                    return _ref;
                }

                constexpr IExternReference* ref() const noexcept
                {
                    return _ref;
                }

                IExternReference* operator->() noexcept
                {
                    return _ref;
                }

                const IExternReference* operator->() const noexcept
                {
                    return _ref;
                }



                void Clear()
                {
                    Unhandle();
                    _ref = nullptr;
                }
            private:

                void Transfer(const ExternVariable& other, bool copy)
                {
                    if (auto ref = other._ref)
                    {
                        if (copy) {
                            ref->ModRefCount(true);
                        }
                        else {
                            other._ref = nullptr;
                        }

                        _ref = ref;
                    }
                }

                void Unhandle()
                {
                    if (_ref) {
                        _ref->ModRefCount(false);
                    }
                }

            private:

                //This should be created the moment it comes into existence
                mutable IExternReference* _ref = nullptr;
            };
            REQUIRED_SIZE(ExternVariable, 0x8);




            struct NativeReferenceBase : public IExternReference
            {


            public:
                Variable& GetDelegate() override { return _delegate; }


            protected:
                NativeReferenceBase(void* tar) : _target{ tar } {}
 

                //I'm thinking that this only updates at the end, or when someone specifies they want to update it some how.

                //Type must both be able to use unvariable and have be transferable into being a variable.
                //Used to be VariableComponent only


            protected:
                void* _target = nullptr;
                Variable _delegate{};
            };

            template <Assignable<Variable> T> requires(!std::derived_from<Unvariable<T>, LEX::detail::not_implemented>)
                struct NativeReference : public NativeReferenceBase
            {
                ~NativeReference()
                {
                    UpdateDelegate();
                }

                explicit NativeReference(T& tar) : NativeReferenceBase{ std::addressof(tar) }
                {
                    //Is there an actual reason to do this right here?
                    NativeReference::Update(true);
                }



                void Update(bool delegate) override
                {
                    T& dest = *reinterpret_cast<T*>(_target);

                    if (delegate) {
                        _delegate = dest;
                    }
                    else {
                        //Here I would like some way to define equivalency so setting isn't required.
                        dest = Unvariable<T>{}(std::addressof(_delegate));
                    }
                }


            };


            template<typename T>
            ExternVariable make_extern(T& tar)
            {
                return ExternVariable{ new NativeReference<T> (tar) };
            }
        }



        namespace Inner
        {
            //These will go in variable. The main point is to be able to know what type
            // was submitted to it, and thus what it should be percieved as.
            //Copy should maintain the left hand sides ype
            struct ComponentVariable
            {
                IComponent* component = nullptr;
                ComponentType type = ComponentType::IComponent;
            };

            struct InfoVariable
            {
                Info* info = nullptr;
                InfoType type = InfoType::Info;

            };


            struct ObjectParams
            {
                ObjectParams(const Object& obj) :
                    data{ obj.data(true) },
                    type{ obj.type },
                    context{ obj.GetContext() }
                {

                }


                ObjectData data{};
                ObjectDataType type = ObjectDataType::kNone;
                std::optional<uint16_t> context = 0;


                void CheckValid() const
                {
                    assert_if(type == ObjectDataType::kNone) {
                        report::runtime::error("Cannot retrieve data of empty ObjectData.");
                    }
                }


                //Make an empty or function.
                template <typename T>
                decltype(auto) get()
                {
                    CheckValid();
                    return data.get<T>();
                }

                template <typename T>
                decltype(auto) get() const
                {
                    CheckValid();
                    return data.get<T>();
                }


                template <typename T>
                auto* ptr()
                {
                    CheckValid();
                    return data.ptr<T>();
                }

                template <typename T>
                const auto* ptr() const
                {
                    CheckValid();
                    return data.ptr<T>();
                }
            };




        }




        struct IAttribute;
        struct ScriptInterface;
        //S

        namespace Util
        {



            void ParameterTest()
            {
                LEX::Util::Parameter<"int"> test = 1;



                auto result = Unvariable<decltype(test)>{}(nullptr);
            }


            ////////////////////////////////////////

            //This only holds the relevant data. It has no barings on how anything else is handled.
            struct CustomObjectData
            {
                union
                {
                    intptr_t raw{};
                    IAttribute* attribute;
                    ScriptObject* object;
                };
                //I have the space, I might as well
                DataType dataType = DataType::Invalid;

                uint32_t typeIndex = -1;//I forget where the unmagic number is.
            };

            template <StringLiteral Name>
            struct CustomObjectTemplateBase : public CustomObjectData
            {
                using Self = CustomObjectTemplateBase<Name>;

                inline static TypeInfo* type = nullptr;
            
                inline static DataType data = DataType::Invalid;

                static void InitDataType(DataType data_type)
                {
                    assert_if(data != DataType::Invalid) {

                    }

                    //This does not handle enums, if this happens, someone did something wrong
                    //assert_if(data_type == DataType::Enum) {}


                    data = data_type;
                }


                static TypeInfo* GetVariableType(const Self*)
                {
                    if (!type) {
                        //Use project manager to get the type
                    }

                    return type;
                }

            };

            template <StringLiteral Name, DataType Type>
            struct CustomObjectTemplate : public CustomObjectTemplateBase<Name>
            {
                using Base = CustomObjectTemplateBase<Name>;
            private:
                struct init
                {
                    init()
                    {
                        Base::InitDataType(Type);
                    }
                };

                inline static init _init = init{};

                //This object will handle what -> and * translate to

            public:
            };

            template <StringLiteral Name>
            struct Class : public CustomObjectTemplate<Name, DataType::Class>
            {
           
            };

            
            template <StringLiteral Name>
            struct Struct : public CustomObjectTemplate<Name, DataType::Struct>
            {

            };

            template <StringLiteral Name>
            struct Interface : public CustomObjectTemplate<Name, DataType::Interface>
            {

            };

            template <StringLiteral Name>
            struct Attribute : public CustomObjectTemplate<Name, DataType::Attribute>
            {

            };
        }
        //Util::Attribute<"Shared::AttributeName"> test;
        



        namespace Attributes
        {

            //Info will have an internal only function that allows it to add an attribute base.
            // This will also be the function that controls whether it can go onto an object
            // or not.

            //Error is caused when trying to construct an attribute from anything that isn't an attribute
            // this is an internal only function
            

        }

        void TestICompBase(IComponentBase* base)
        {
            base->As<Component>();
        }

#ifdef REENABLE_THIS
        struct FormulaParam
        {
            FormulaParam() = default;

            FormulaParam(const std::string_view& path) : value{ path }
            {

            }

            FormulaParam(TypeInfo* type) : value{ type }
            {

            }


            TypeInfo* GetType(IScript* script)
            {
                if (script)
                {
                    switch (value.index())
                    {
                    case variant_index<decltype(value), std::string_view>():
                        value = script->GetTypeFromPath(std::get<std::string_view>(value))->As<TypeInfo>();
                        [[fallthrough]];
                    case variant_index<decltype(value), TypeInfo*>():
                        return std::get<TypeInfo*>(value);
                    }
                }

                return nullptr;
            }

            std::variant<TypeInfo*, std::string_view> value;
        };
#endif

        namespace
        {
            enum struct CallableType : uint8_t
            {
                Invalid,
                Method,
                Function,
                Extension,
                Constructor,
                Total,
            };

            //A callable unit is now something that is attached to a thing that 
            struct ICallableUnit
            {
                //I'd like this to be the thing that I use to handle anything parameter related.T

                virtual RuntimeVariable GetDefault(size_t i, std::span<RuntimeVariable> args = {}) = 0;
            };

            
            //This should only be available on something concrete. So I think I'll maybe
            // put in in the the implementation of Function
            struct FunctionThing : public FunctionData
            {

                bool RunDefault(RuntimeVariable& out, size_t i, std::span<RuntimeVariable> args = {})
                {
                    ParameterInfo* info = std::addressof(parameters[i]);
                    Routine* routine = info->defFunc.get();


                    if (!routine)
                        return false;


                    Runtime runtime{ *routine, nullptr, args };

                    //Variable& a_this = *this;

                    out.AssignRef(runtime.Run());

                    return true;
                }



                bool FillArgumentsImpl(std::vector<RuntimeVariable>& out)
                {
                    if (out.size() < defaultIndex) {
                        //Couldn't extend it.
                        return false;
                    }


                    for (int i = defaultIndex; i < GetArgCount(); i++)
                    {
                        RuntimeVariable in;

                        if (RunDefault(in, i, out) == false)
                            return false;
                        
                        out.push_back(in);
                    }


                    return true;
                }

                bool FillArguments(std::vector<RuntimeVariable>& out)
                {
                    if (HasTarget() == true)
                        return false;

                    FillArgumentsImpl(out);

                    return true;
                }



                bool FillArguments(RuntimeVariable& a_this, std::vector<RuntimeVariable>& out)
                {
                    if (HasTarget() == false)
                        return false;


                    out.insert(out.begin(), a_this);

                    bool result = FillArgumentsImpl(out);

                    out.erase(out.begin());

                    return result;
                }
            };


        }

        constexpr bool test = std::is_bounded_array_v<int[4]>;
        using IT = std::remove_extent_t<int[]>;

        template <typename T, typename Char = char>
        struct is_char_array : public std::bool_constant<std::is_bounded_array_v<std::remove_reference_t<T>> && std::is_same_v<std::remove_extent_t<std::remove_reference_t<T>>, const Char>> {};

        template <typename T, typename Char = char>
        constexpr bool is_char_array_v = is_char_array<T, Char>::value;



        //<std::is_same_v<qualify_extracted_type_t<std::remove_cvref_t<T>, std::remove_cvref_t>>, Char*>>>        
        template <typename T, typename Char = char>
        struct is_c_string : public std::bool_constant<std::is_same_v<
            qualify_extracted_type_t<std::remove_cvref_t<T>, std::remove_cvref_t>, 
            Char*>> {};

        template <typename T, typename Char = char>
        constexpr bool is_c_string_v = is_c_string<T, Char>::value;


        template <typename T, typename Char = char>
        concept char_array = is_char_array_v<T, Char>;



        struct string_taker
        {
            struct helper
            {
                helper(const char*) {}
                helper(const std::string_view&) {}
            };

            string_taker(const helper&) {}
            
            template<typename Char>
            string_taker(Char) 
                requires(std::is_pointer_v<Char> && std::is_same_v<std::remove_const_t<std::remove_pointer_t<Char>>, char>) {}
            
            template<size_t N>
            consteval string_taker(const char(&other)[N]) requires(N < 16) {  
            }
            
            template<size_t N>
            string_taker(const char(&other)[N]) requires(N >= 16) {
            }


            //template<typename T>// requires (requires(T t, const char* it) { it = t; })
            //string_taker(const T& test) {}


            string_taker(const std::string_view&) {}

            string_taker(string_taker&&) {}
            string_taker(const string_taker&&) {}


            //template<std::same_as<const char*> Char>
            //void foo(Char) {}

            template<typename = void>
            void foo(const char*) {}

            template<size_t N>
            void foo(const char(&other)[N])
            {

            }

            template<typename T> requires (is_c_string_v<T> || is_char_array_v<T>)
            void func(T&& test) 
            {
                if constexpr (is_char_array_v<T>)
                {

                }
                else
                {

                }
            }

            template<> 
            void func<const char[1]>(const char (&&test)[1])
            {
              
            }

        };


   

        INITIALIZE()
        {
            struct FirstAttribute : public AttributeData
            {
                bool CanAllowAttach(AttributeOwner* owner) override
                {
                    IComponent* component = owner->AsOwner<IComponent>();

                    ITypeInfo* type = component->As<ITypeInfo>();

                    if (!type) {
                        report::compile::failure("TFirstAttribute can only attach to a type");
                        return false;
                    }

                    return true;
                }
            };

            IFunction* func = nullptr;
            FirstAttribute r;

            AttributeManager::instance->RegisterNativeData<FirstAttribute>("Core::TestAttribute");
        }


        /*
        void MakeAttribute(TypeInfo* context)
        {
            ConcreteType* type = (ConcreteType*)0;

            CustomAttribute* attribute = (CustomAttribute*)0;

            uintptr_t budget = (uintptr_t)type->GetFieldRange();

            attribute->_fields.Create((uint32_t)budget);

            
            

            type->VisitTrees([&](InheritNode& node)
                {

                    if (auto members = node.tree->GetMembers())
                    {
                        size_t index = node.memberIndex;
                        size_t field_count = node.tree->GetFieldCount();

                        //auto begin = attribute->_fields.data() + index;
                        //auto end = begin + field_count;

                        //std::span range{ begin , end };

                        std::span range = attribute->fields(index, field_count);

                        for (int i = 0; i < field_count; budget--, i++)
                        {
                            auto& field = members->fields[i];
                            auto type = field.GetType();


                            auto real = type->GetTypeInfo(nullptr);


                            range[i] = detach(real->GetDefault());
                        }
                        
                    }
                });

            assert_if (budget) {
                //Not proper.
            }

            
        }


        //THESE functions will no longer belong to Variable, they will belong to the class that handles membered able data classes
        
        ScriptObject* GetScriptData(Variable& a_this)
        {
            ScriptObject* result = std::visit([](auto&& self) -> ScriptObject* {
                using T = std::decay_t<decltype(self)>;

                if constexpr (std::is_same_v<T, IComponent*>) {
                    IAttribute* attribute = self->As<IAttribute>();
                    return attribute ? attribute->GetScriptObject() : nullptr;
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

        namespace Src
        {
            //These are to exclusively be used internally.


            bool GetField(size_t& out, MemberPointer ptr, ITemplateBody* body, uint32_t viewpoint = -1)
            {
                return false;
            }

            bool GetMethod(IFunction*& out, MemberPointer ptr, ITemplateBody* body, uint32_t viewpoint = -1)
            {
                return false;
            }
        }


        namespace ScrObj
        {
            //These are for use within script object externally and such.
            // The template body probably should not be used here. Its main purpose
            // is to resolve the member pointer, and to check (but not resolve) the type.
            //Also of note, before it checks it will need to make up for the difference the body
            // may lack. Rather I may need to partialize it, and fill it with it's own templates.

            bool GetField(size_t& out, const std::string_view& name, uint32_t viewpoint = -1)
            {
                return false;
            }

            bool GetMethod(IFunction*& out, const std::string_view& name, uint32_t viewpoint = -1)
            {
                return false;
            }

            //The above will be used by the CustomObject helpers and will assert if either return false.
        }

        //Both of these


        

        bool GetMemberField(Variable& a_this)
        {
            ScriptObject* object = GetScriptData(a_this);


            if (!object) {
                return false;
            }
        }
        //*/
        void CtorBuilder(RoutineCompiler* compiler, SyntaxRecord& target)
        {

            InstructList out;

            //IF this type is instantiable

            auto this_var = compiler->GetScope()->SearchField(parse_strings::this_word);
            auto undefined = LiteralManager::ObtainUndefined();

            //These 2 will be handled after handle the init var stuff
            Instruction not_equal{ InstructionType::NotEqualTo, compiler->GetPrefered(), this_var.AsSolution(compiler), undefined.first };
            Instruction drop{ InstructionType::DropStack, Operand{2, OperandType::Differ}, Operand{compiler->GetPrefered(), OperandType::Register} };
            Instruction init_var{ InstructionType::DeclareVariable, this_var.AsSolution(compiler), Operand{this_var.GetType(), OperandType::Type} };
            //These don't work for some reason
            //compiler->EmplaceInstruction(not_equal);
            //compiler->EmplaceInstruction(drop);
            //compiler->EmplaceInstruction(init_var);
            //Jumps if not equal to each other.
            {
                Scope main_block{ compiler, ScopeType::Required, out };


            }

        }


        void InlineRoutine(RoutineCompiler* compiler, std::vector<Instruction>& instruction, RecordHolder* holder, Routine* base)
        {
            constexpr uintptr_t test1 = 5;

            constexpr uintptr_t test2 = test1 - ((uintptr_t)0 - 6);

        }
    }



}
#include "Lexicon/Engine/TestToss.h"