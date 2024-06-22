#include "Lexicon.h"

#include "Lexicon/Impl/OperandType.h"
#include "Lexicon/Impl/Register.h"
#include "Lexicon/Impl/Operand.h"

//Component must move from impl.
#include "Lexicon/Component.h"

//#include "Lexicon/Impl/TestField.h"
#include "Lexicon/Impl/Parser.h"
#include "Lexicon/Impl/ParserTest.h"
#include "Lexicon/Impl/RoutineBase.h"
#include "Lexicon/Impl/RoutineCompiler.h"

//#include "spdlog/spdlog.h"
//#include "spdlog/sinks/stdout_color_sinks.h"
//#include <spdlog/sinks/basic_file_sink.h>
//#include <spdlog/sinks/msvc_sink.h>
//#include <spdlog/sinks/stdout_sinks.h>


#include "Lexicon/Impl/Runtime.h"

//
//#include "Lexicon/Impl/TempConstruct.cpp"
//






using namespace RGL;
using namespace LEX;
using namespace LEX::Impl;




static void PrintAST(Record& tree, std::string indent = "")
{
    const static std::string __dent = "|  ";

    std::string log = tree.PrintAs<Syntax>();

    RGL_LOG(info, "{}{}", indent, log);

    indent += __dent;

    for (auto& child_rec : tree.GetChildren())
    {
        PrintAST(child_rec, indent);
    }
}


void TestProcedure(RuntimeVariable& result, Variable* target, std::vector<Variable*> args, ProcedureData& data)
{
    std::string tar = target->AsString();
    logger::info("string \"{}\", size {}; arg size {} of {} ({})?", tar, tar.size(), args.size(), ((ConcreteFunction*)data.srcFunc)->_name, ((ConcreteFunction*)data.srcFunc)->parameters.size());
    result = tar.size();
}

double size_backend(std::string a_this)
{
	logger::info("size of \"{}\"", a_this);
	return a_this.size();
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

void LexTesting(std::string formula)
{
    if (false)
    LEX::ProjectManager::InitShared();
    

    //for (const auto iterator = std::filesystem::directory_iterator("C:/Users/Noah/Desktop/Modding/[Lab]/{Lab Tools}/lex-tester/src"); const auto & entry : iterator) {
    //    if (entry.exists() == false) continue;
    //    logger::info("{}", entry.path().string());
    //}



    std::string project = "shared";
    std::string file = "Test";
    //Note the existence of these

    //int test2[5];
    //int result = !++test2[2];
    //cout << "Hello World << " << result;
    //constexpr int test = 1 - - -  - - - 1;

    /*
    std::string text = "function( ) * (8 * 4 + 5) / -6";


    /*/
    //adding the ; seems to help. Some how, it seems to simultaneously not be able to read the last token
    // and also skip right past it.
    //*
#pragma region Texts
    std::string text_hope = R"(
        ActorValue::ActorValueStruct GetActorValue(float a1, float a2, commons::type a3)
        {
            var test = function( ) * (8 * 4 + 5) / -6;
            //This is a comment.

            var next = test /*This is also a comment.*/ + 5;

            return (test * next) / 2;
        }

    )"s;

    std::string text_v1 = R"(
        int GetActorValue()
        {
            int test = 77 * (8 + 4) / 2;
            //This is a comment.

            //we aint ready for this one
            //int next = test /*This is also a comment.*/ + 5;

            //return 20;
            return test / 2;
        }

    )"s;


    std::string crash1 = R"(
        struct float intrinsic NUMBER::82;//is actually double, also float is a keyword

        float GetActorValue(float othername, float shootfol, float tellinal, float peacefal, float scrundal)
        {
            return 1;
        };

    )"s;


    std::string crash3 = R"(
        Scope::Scope::Scope::Type1 ref type = 2;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        
        
        float TestCall(this float, float a, float b)
        {
            return a + b + this;
        };
        //I have to put extra ";" after everything that expects an end now. Delimit might not be fit to use for this anymore.
        float GetActorValue(this float, float othername, float shootfol, float tellinal, float peacefal, float scrundal)
        {
            //int test = othername + shootfol + tellinal + peacefal + scrundal;
            //Nope, it's just broken right now.
            //int test = TestCall(othername + 2, shootfol) + tellinal + peacefal + scrundal;//This causes a crash?
            

            float test = tellinal.TestCall(othername, shootfol) + tellinal + peacefal + scrundal + this; ;
            this = 0;
            
            return this + test;
        };

    )"s;
#pragma endregion

    std::string crash2 = R"(

        interface Addable{};
        interface Primitive{};
        interface BiggerAddable : public Addable{};
        struct Number intrinsic NUMBER::0 : public Addable, public Primitive;
        struct __float64 intrinsic NUMBER::45;//is actually double, also float is a keyword
        struct __string8 intrinsic STRING::0;
        
        class Array external ARRAY;


        void otherTest()
        {
        };

        float size(this string)
        {
            //defined elsewhere.
            return 0;
        }

        float TestCall(this float, float a, float b)
        {

            this = this + 3;//This is the bread and butter of what we want. All considered, it should be 93, but if not done right, it's 90.

            return a + b + this;
        };

        float GetActorValue(this float, float othername, float shootfol, float tellinal, float peacefal, float scrundal)
        {
            //int test = othername + shootfol + tellinal + peacefal + scrundal;
            //Nope, it's just broken right now.
            //int test = TestCall(othername + 2, shootfol) + tellinal + peacefal + scrundal;//This causes a crash?
            
            //This shit's itself for some reason.
            Number testValue = 12;

            Array const _array = Array();
            //Array _array2 = _array;
            __string8 test_string = "THISSTR";
            //Calling TestCall twice like this makes it crash
            float test = tellinal.TestCall(othername, shootfol).TestCall(othername, shootfol)  + tellinal + peacefal + scrundal + this;
            //float test = tellinal.TestCall(othername, shootfol)  + tellinal + peacefal + scrundal + this;
            this = 0;

            float testB = __float64();

            const float testConst = 5;

            //testConst = testB;

            return this + test + testB + test_string.size();
            //Old answer was 93.
        };

    )"s;
    
    std::string test_identifier = R"(
        struct __float64 intrinsic NUMBER::45 : public ::::SomeProject::SomeScript::SomeType::OtherType::Number, public internal Commons::Primitive;

        ::::name1::name2::more_name::final_name;

        const readonly int flag;

        ::name1::name2::name const readonly flag;

        ::name1::name2::name const readonly flag(int name1, int name2, int name3) const
        {
            const Array test_again = Array();

            {
                ::name1::name2::name const readonly flag;
                ::name1::name2::name const readonly flag2;

                return flag2;
            }
        }

        typename_here identifier = something * something_else.(some() + process * that - does / stuff + 20).inner_something;

        identifer = a_thing + another_thing - a_third_thing;
    )"s;
    std::string this_functions = R"(
        float size(this string)
        {
            //defined elsewhere.
            return 0;
        }
    )"s;

    //Identifier, Header
	std::string text = crash2;
    //std::string text = test_identifier;
    //std::string text = this_functions;


    //std::string formula = "2 * (4 + 6) / 5";
    //*/
    //There used to be a thingy here.
    //std::string text = "1 + 1 + 1 + 1";

    ///Record ast = Parser__::CreateSyntaxTree("project", "name", "return " + formula, ReturnParser::GetSingleton());
    Record ast = Parser__::CreateSyntaxTree("project", "name", text);

    PrintAST(ast);
    
    logger::info("Record uses {} Kilobytes", ast.GetMemoryUsage() / 1000.f);
    
    //return;
    
    
    Script* script = Component::Create<Script>(ast);

    Component::Link(LinkFlag::Declaration);
	Component::Link(LinkFlag::Definition);
	Component::Link(LinkFlag::External);

    //return;

    if (1)
    {
        auto funcs = script->FindFunctions("size");

        if (funcs.size() != 0)
        {

            ConcreteFunction* function = dynamic_cast<ConcreteFunction*>(funcs[0]->Get());

            if (function)
            {
				if (RegisterFunction(size_backend, function) == false) {
					logger::debug("failure");
					std::system("pause");
				}
                //function->_procedure = TestProcedure;
            }
        }
    }

    auto funcs = script->FindFunctions("GetActorValue");

    if (funcs.size() == 0)
    {
        logger::trace("_a");

    }

    ConcreteFunction* function = dynamic_cast<ConcreteFunction*>(funcs[0]->Get());
    
    if (function)
    {
        //Testing the proceedure
        if constexpr (0)
        {
            if (RegisterFunction(GetActorValue_backend, function) == false)
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
        Variable result = function->Call(69.0, 1.0, 2.0, 3.0, 4.0, 5.0);

        std::string number = result.AsNumber().ToString();

        logger::info("result of {} = {}", formula, number);
    }
    else
    {
        logger::info("Function couldn't be found in script");
    }
    
    //END OF THE CONTROLLED ENVIRONMENT
    return;
    //END OF THE CONTROLLED ENVIRONMENT


    //No longer needed to test stuff
    /*
    if (false)
    {
        LEX::PolicyBase* policy = LEX::ObtainPolicy(ast.GetChild(1));
        
        if (policy) {
            policy->ConstructFromRecord(ast.GetChild(1));
            logger::info("id? {}", policy->GetName());
        }
        else
            logger::info("no policy");

        return;
    }
    //*/
    constexpr auto offset = LEX::Number::Settings::GetOffset(LEX::NumeralType::Floating);

    QualifiedType number_type = QualifiedType{ LEX::IdentityManager::instance->GetTypeByID(offset + 1) };
    
    LEX::FunctionData test_data{};


    test_data._returnType = number_type;
    std::vector<LEX::ParameterInfo> parameters
    {
        LEX::ParameterInfo{number_type, "scrundal", 0},
        LEX::ParameterInfo{number_type, "peacefal", 1},
        LEX::ParameterInfo{number_type, "tellinal", 2},
        LEX::ParameterInfo{number_type, "shootfol", 3},
        LEX::ParameterInfo{number_type, "othername", 4}
    };
    //test_data.parameters = parameters;

    //remember to actually use these now

    
    
    std::vector<LEX::Variable> arguments
    {
        LEX::Number{1.0},
        LEX::Number{2.0},
        LEX::Number{3.0},
        LEX::Number{4.0},
        LEX::Number{5.0},
    };

    
    //LEX::RoutineBase routine = LEX::RoutineCompiler::Compile(ast, &test_data);//this is if it's just the function
    LEX::RoutineBase routine = LEX::RoutineCompiler::Compile(ast.GetChild(1), &test_data);
    
    
    for (int i = 0; i < routine.GetOperativeCapacity(); i++)
    {
        logger::debug("instruct at {} = {}", i, routine[i]._instruct);
    }
    
  
    LEX::Runtime runtime{ routine, arguments };//The creation of runtime yields 2 numbers that should not exist.
    
    LEX::Variable result = runtime.Run();



    std::string number = result.AsNumber().ToString();

    logger::info("result of {} = {}", formula, number);

    //I want to make a temporary runtime and fire this.
    //LEX::ProjectManager::InitShared();
    //RGL::Record ast = Parser__::CreateSyntaxTree(project, file, text);

    //PrintAST(ast);
}


namespace
{
    //This is how I may want to set up something like GetObjectType for external users.
    template <typename T>
    void GetTest()
    {
        std::cout << "1";
    }

    template  <std::integral T>
    void GetTest()
    {

        std::cout << "2";
    }


    template  <>
    void GetTest<uint32_t>()
    {
        std::cout << "3";
    }
}

template <typename T = std::string>
void TestAgain()
{

}

void Funckle(std::initializer_list<int> test)
{
    TestAgain();
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


int main(int argc, char** argv) {
    
#ifdef _DEBUG
    //Need a way to only have this happen when holding down a key
    if (GetKeyState(VK_RCONTROL) & 0x800) {
        constexpr  LPCSTR text1 = "Request for debugger detected. If you wish to attach one and press Ok, do so now if not please press Cancel.";
        constexpr LPCSTR text2 = "Debugger still not detected. If you wish to continue without one please press Cancel.";
        constexpr LPCSTR caption = "Debugger Required";

        int input = 0;

        do
        {
            input = MessageBox(NULL, !input ? text1 : text2, caption, MB_OKCANCEL);
        } while (!IsDebuggerPresent() && input != IDCANCEL);
    }
#endif
    Initializer::Execute();
 


    Funckle({ 1, 2, 4, 5 });

    //GetTest<int64_t>();
    //LEX::Report<LEX::IssueType::Compile>::debug("The numbers {} and {} are {}", 69, 420, "nice");
    //std::system("pause");
    //return 0;
    


    //using input causes a crash for some reason.
    std::string formula;
    //std::cin >> formula;

    SafeInvoke([&]() {
        std::getline(std::cin >> std::ws, formula);

        LexTesting(formula);
    });
    
    std::system("pause");
	return 0;
}

