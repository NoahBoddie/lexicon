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

void LexTesting(std::string formula)
{
    //LEX::ProjectManager::InitShared();
    

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

    std::string text = R"(
        struct Double intrinsic NUMBER::82;

        int GetActorValue()
        {
            int test = othername + shootfol + tellinal + peacefal + scrundal;
            
            return test;
        };

    )"s;

    //std::string formula = "2 * (4 + 6) / 5";
    //*/
    //There used to be a thingy here.
    //std::string text = "1 + 1 + 1 + 1";

    ///Record ast = Parser__::CreateSyntaxTree("project", "name", "return " + formula, ReturnParser::GetSingleton());
    Record ast = Parser__::CreateSyntaxTree("project", "name", text);

    PrintAST(ast);

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
    constexpr auto offset = LEX::Number::Settings::GetOffset(LEX::NumeralType::Floating);

    LEX::ITypePolicy* number_type = LEX::IdentityManager::GetTypeByID(offset + 1);
    
    LEX::FunctionData test_data{};

    test_data.temp_returnType = number_type;
    std::vector<LEX::ParameterInfo> parameters
    {
        LEX::ParameterInfo{number_type, 0, "scrundal"},
        LEX::ParameterInfo{number_type, 1, "peacefal"},
        LEX::ParameterInfo{number_type, 2, "tellinal"},
        LEX::ParameterInfo{number_type, 3, "shootfol"},
        LEX::ParameterInfo{number_type, 4, "othername"}
    };
    test_data.parameters = parameters;

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



    std::string number = static_cast<std::string>(result.AsNumber());

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


int main(int argc, char** argv) {
    

    //GetTest<int64_t>();
    //LEX::Report<LEX::IssueType::Compile>::debug("The numbers {} and {} are {}", 69, 420, "nice");
    //std::system("pause");
    //return 0;
    


    //using input causes a crash for some reason.
    std::string formula;
    //std::cin >> formula;
    


    try {
        std::getline(std::cin >> std::ws, formula);
        
        LexTesting(formula);
        //std::cout << formula  << "\n";
    }
    catch (...) {
        RGL_LOG(critical, "critical error occured.");
        std::system("pause");
        std::exception_ptr p = std::current_exception();
        
        
        std::rethrow_exception(p);
    }
    

	//std::string out;
	//std::cin >> out;
    
    std::system("pause");
	return 0;
}