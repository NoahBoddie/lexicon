

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

    //Record ast = Parser__::CreateSyntaxTree("Fake", "Script", contents, &parser);
    Record ast;

    if (Parser__::CreateSyntaxTree(ast, contents, "Script") == false)
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
    

    Component::Link(LinkFlag::Loaded);
    Component::Link(LinkFlag::Declaration);
	Component::Link(LinkFlag::Definition);
   
    
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

    Component::Link(LinkFlag::Object);
    Component::Link(LinkFlag::External);

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

    ConcreteFunction* function = dynamic_cast<ConcreteFunction*>(funcs[0]->Get());
    
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

        ConcreteFunction* function = dynamic_cast<ConcreteFunction*>(funcs[0]->Get());

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

    Parser__::CreateSyntax<LineParser>(it, "20");

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


//*
namespace LEX
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
        int test=1;
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


    void TESTIDEA()
    {
        TestIdea test;
        test.foo();
    }




    //Static res resolver

    struct IExitDestructor
    {
        virtual ~IExitDestructor() noexcept = default;
    };



    template <typename T>
    struct BasicExitDestructor : public IExitDestructor
    {
        T value;

        template <typename...Args>requires(requires (Args... args) { T{ std::forward<Args>(args)... }; })
        BasicExitDestructor(Args... args) : value{ std::forward<Args>(args)... }
        {
        }

    };
    

    struct DestructorList
    {
        struct Entry
        {
            //Stores the const pointer type of the targeted object. Name is irrelevant, this is a personal object.
            //const std::type_info* type = nullptr;

            //This hash code is either based on the given file, or the pointer of the type id used.
            size_t hash = 0;//This is based on the given location, this way you can have multiple
            std::unique_ptr<IExitDestructor> destructor{};
        };

        //This increases with each confirmed initial use of 
        //std::vector<std::pair< >>

        auto Find(size_t hash)
        {
            return std::find_if(entries->begin(), entries->end(), [hash](Entry& it) {return it.hash == hash; });
        }

        auto Emplace(size_t hash)
        {
            auto it = Find(hash);

            if (entries->end() != it) {
                return it;
            }

            return entries->insert(it, { hash });
        }


        void Place(std::unique_ptr<IExitDestructor>&& dtor, size_t hash, std::span<size_t> dependencies)
        {
            //The placement will be as such, 
            
            
            //Lazy bid to avoid iterator invalidation
            if (entries->capacity() - entries->size() < dependencies.size() + entries->size()) {
                entries->reserve(entries->size() + dependencies.size());
            }


            {
                auto it = Find(hash);

                if (entries->end() != it) {
                    assert_if(!!it->destructor) {
                        //Already taken, please report error
                        throw std::exception("Hash was already taken");
                    }
                }
                else {
                    it = entries->insert(it, { hash });
                }

                it->destructor = std::move(dtor);


                for (auto dep : dependencies)
                {
                    if (dep == hash)
                        continue;

                    auto cmp = Find(dep);

                    if (entries->end() != cmp) {
                        assert_if(cmp >= it) {
                            //Already taken, please report error
                            throw std::exception("Dependency was placed after");
                        }
                    }
                    else {
                        cmp = entries->insert(it, { hash });
                        it = cmp + 1;
                    }
                }


            }


            return;





            std::vector<std::vector<Entry>::iterator> lump;

            for (auto dependency : dependencies)
            {
                if (dependency == hash)
                    continue;

                lump.push_back(Emplace(dependency));
            }
            auto it = Find(hash);

            if (entries->end() != it) {
                assert_if (!!it->destructor) {
                    //Already taken, please report error
                    throw std::exception("Hash was already taken");
                }


                //Here, we want to check for any of the active lumps to be ahead of this (signifies a dependent requires the current static).

                for (auto cmp : lump)
                {
                    logger::info("{} vs {}", std::distance(entries->begin(), cmp), std::distance(entries->begin(), it));

                    assert_if(cmp >= it) {


                        throw std::exception("Dependency was placed after");
                    }


                }

            }
            else {
                it = entries->insert(it, { hash });
            }


            it->destructor = std::move(dtor);

        }


        void TryDestroy()
        {
            if (!refCount) {
                if (destructed) {
                    //logger::debug("successfully destructed ObjectPolicyList");
                }

                for (size_t i = entries->size(); i; i--)
                {
                    auto& it = entries->at(i - 1);
                    it.destructor.reset();
                }
                
                delete entries;
            }

        }

        void ModRefCount(bool inc, bool destructing)
        {
            refCount += inc ? 1 : -1;

            //logger::trace("temp, {} to {}", inc ? "increment" : "decrement", refCount.load());

            assert(refCount >= 0);

            if (refCount && destructed) {
                //logger::debug("{} detected, {} refs remaining", inc ? "increment" : "decrement", refCount.load());
            }
            else assert_if(!refCount && !destructed && !destructing)
            {
                //report::fault::critical("ObjectPolicyManager ran out of uses outside of destruction, this should not happen");
            }

            TryDestroy();
        }

        ~DestructorList()
        {
            ModRefCount(false, true);
            destructed = true;

            if (refCount) {
                //logger::debug("Delaying destruction of ObjectPolicyList, {} refs remaining", refCount.load());
            }
        }
        
        std::vector<Entry>* entries = new std::vector<Entry>;

        std::atomic<ptrdiff_t>  refCount = 1;
        bool destructed = false;

    } singleton;
    

    struct test_uns {};

    //This is used if something isn't contained in a class
    template<StringLiteral Key>
    struct unscoped : public test_uns
    {
        static constexpr size_t hash = std::hash<std::string_view>{}(Key.view());
    };


    template <typename T, typename H = void>
    struct safe_static
    {

    private:
        template<typename T>
        static size_t Hash()
        {
            if constexpr (std::derived_from<T, test_uns>) {
                return T::hash;
            }
            else {
                return std::hash<std::type_index>{}(std::type_index{ typeid(const T*) });
            }
        }
        template<typename T>
        static void AddHash(std::vector<size_t>& list)
        {
            
            //if constexpr (specialization_of<T, std::tuple>)
            //{
            //    (list.push_back(Hash<T>()), ...);
            //}
            //else {
                list.push_back(Hash<T>());
            //}
        }

        static size_t Count()
        {
            if constexpr (std::is_same_v<H, void>) {
                return 0;
            }
            else {
                static size_t count = 1;
                return count++;
            }
        }
    public:
        
        
        struct token
        {
        private:
            ~token() = default;

        public:

            token(T& t) : result{ &t } {}

            constexpr operator T& () noexcept
            {
                return *result;
            }


        private:
            T* result = nullptr;
        };

        template<typename... Deps, typename... Args>
        static token&& init(Args&&... args)
        {
            //Creates hash from the source location,

            //Deps: are the type dependencies that T relies on
            std::vector<size_t> dependencies{};
            
            (AddHash<Deps>(dependencies), ...);


            size_t hash;

            if constexpr (std::is_same_v<H, void>) {
                size_t h1 = Hash<T>();
                size_t h2 = Count();

                // A simple way to combine hashes (boost::hash_combine is more robust)
                hash = h1 ^ (h2 << 1);
            }
            else {
                hash = Hash<H>();
            }
            //auto ptr = new ;

            std::unique_ptr<BasicExitDestructor<T>> dtor = std::make_unique<BasicExitDestructor<T>>(std::forward<Args>(args)...);
            token result{ dtor->value };

            
            singleton.Place(std::move(dtor), hash, dependencies);

            return std::move(result);
        }


    };
    template <typename T>
    struct safe_singleton : public safe_static<T, T>
    {

    };


    namespace
    {
        //Shared singleton is init'd both by meyers singleton, and also static initialization, attempting to
        // be first in either.

        //A singleton that manages other singletons, maintaining their lifetime until they're no longer required

        namespace detail
        {
            struct SharedSingleton
            {
                struct IEntry
                {
                    virtual ~IEntry() noexcept = default;
                };



                template <typename T>
                struct BasicEntry : public IEntry
                {
                    T value;

                    template <typename...Args>requires(requires (Args... args) { T{ std::forward<Args>(args)... }; })
                        BasicEntry(Args... args) : value{ std::forward<Args>(args)... }
                    {
                    }

                };


                static SharedSingleton* GetSingleton()
                {
                    static SharedSingleton singleton{};
                    return &singleton;

                }

                inline static SharedSingleton* singleton = GetSingleton();



                void Place(std::unique_ptr<IExitDestructor>&& dtor)
                {
                    entries->push_back(std::move(dtor));
                }


                void TryDestroy()
                {
                    if (!refCount) {
                        if (destructed) {
                            //logger::debug("successfully destructed ObjectPolicyList");
                        }

                        for (size_t i = entries->size(); i; i--)
                        {
                            auto& it = entries->at(i - 1);
                            it.reset();
                        }

                        delete entries;
                    }

                }

                void ModRefCount(bool inc, bool destructing)
                {
                    refCount += inc ? 1 : -1;

                    //logger::trace("temp, {} to {}", inc ? "increment" : "decrement", refCount.load());

                    assert(refCount >= 0);

                    if (refCount && destructed) {
                        //logger::debug("{} detected, {} refs remaining", inc ? "increment" : "decrement", refCount.load());
                    }
                    else assert_if(!refCount && !destructed && !destructing)
                    {
                        //report::fault::critical("ObjectPolicyManager ran out of uses outside of destruction, this should not happen");
                    }

                    TryDestroy();
                }

                ~SharedSingleton()
                {
                    ModRefCount(false, true);
                    destructed = true;

                    if (refCount) {
                        //logger::debug("Delaying destruction of ObjectPolicyList, {} refs remaining", refCount.load());
                    }
                }

                std::vector<std::unique_ptr<IExitDestructor>>* entries = new std::vector<std::unique_ptr<IExitDestructor>>;

                std::atomic<ptrdiff_t>  refCount = 1;
                bool destructed = false;

            };

        }

        template<typename T, typename... Args>
        static auto&& make_singleton(Args&&... args)
        {
            using detail::SharedSingleton;

            struct token
            {
            private:
                ~token() = default;

            public:

                token(T& t) : result{ &t } {}

                constexpr operator T& () noexcept
                {
                    return *result;
                }


            private:
                T* result = nullptr;
            };

            using Entry = SharedSingleton::BasicEntry<T>;



            std::unique_ptr<Entry> dtor = std::make_unique<Entry>(std::forward<Args>(args)...);

            token result{ dtor->value };


            SharedSingleton::singleton->Place(std::move(dtor));

            return std::move(result);
        }

    }


    //Example:
    // integer loads first, then "first", then "second"
    // "first" should destruct first, then "second", then the integer

    struct First {
        ~First()
        {
            logger::info("first");
        }
    };


    struct Second;


    INITIALIZE_NOW()
    {
        logger::InitializeLogging();
        //static int& test = safe_static<int>::init_<int>(1);
        static int& test  = safe_static<int>::init<Second>(1);
        //static int& test2 = make_singleton<int>();
        logger::info("{} it", test);
    }

    struct Second
    {
        ~Second()
        {
            logger::info("second");
        }
    };

    First& first = safe_singleton<First>::init();
    //First first = {};

    Second& second = safe_singleton<Second>::init<First>();
    //Second second = {};

    //There's the subscribing type, the type we're trying to export to 







}
//*/
