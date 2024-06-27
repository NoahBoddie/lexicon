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


    constexpr auto testeetet =  GetNumberOffset(NumeralType::Integral, Size::Bit, Signage::Unsigned, Limit::Bound);


    {
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

        Record ast = Parser__::CreateSyntaxTree("Shared", "Commons", test_identifier);

        PrintAST(ast);

        logger::info("Record uses {} Kilobytes", ast.GetMemoryUsage() / 1000.f);

        std::system("pause");
    }
    


    
    
    ProjectManager::InitShared();
 
    Script* script = ProjectManager::GetSharedProject()->GetCommons();
    
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
    
    
    {
        auto funcs = script->FindFunctions("TestBoolean");

        ConcreteFunction* function = dynamic_cast<ConcreteFunction*>(funcs[0]->Get());

        if (function)
        {
            
            Variable result = function->Call();

            std::string number = result.AsNumber().ToString();

            logger::info("result of Test = {}", number);
        }
        else
        {
            logger::info("Function couldn't be found in script");
        }
    }
    //END OF THE CONTROLLED ENVIRONMENT
    return;
    //END OF THE CONTROLLED ENVIRONMENT
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
        //std::getline(std::cin >> std::ws, formula);

        LexTesting("GetValueTest");
    });
    
    std::system("pause");
	return 0;
}

