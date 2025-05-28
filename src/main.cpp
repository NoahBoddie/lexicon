

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
using namespace LEX::Impl;

namespace logger
{

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
            Formula<int&(int)>;

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
   

    Initializer::Execute();

    //GetTest<int64_t>();
    //LEX::Report<LEX::IssueType::Compile>::debug("The numbers {} and {} are {}", 69, 420, "nice");
    //std::system("pause");
    //return 0;

    


    SafeInvoke([&]() {
        //std::getline(std::cin >> std::ws, formula);

        LexTesting("GetValueTest");

        //Formula<float>::Run("'something'.size()", "ActorValueGenerator::Commons");

    });
    

    std::system("pause");
	return 0;
}






namespace LEX
{


	namespace NEW_PARSING
	{
		using namespace Impl;
	

	


		struct Tokenizer
		{
			constexpr static std::string_view remains = R"((?:\w|\S))";//Used to consume any other unidentified token
			
			
			std::set<std::string_view> keywords;	//Keywords and conditionals aren't compiled into this due to them already needing to be identifiers
			std::set<std::string_view> conditionals;
			std::set<std::string_view> operators;
			std::set<std::string_view> punctuation;
			std::string mainPattern;
			mutable std::string compiledPattern;

			void AddKeyword(const std::string_view& pattern) {
				keywords.emplace(pattern);
			}

			void AddConditional(const std::string_view& pattern) {
				conditionals.emplace(pattern);
			}

			void AddOperator(const std::string_view& pattern) {
				compiledPattern.clear();
				operators.emplace(pattern);
			}
			
			void AddPunctuation(const std::string_view& pattern) {
				compiledPattern.clear();
				punctuation.emplace(pattern);
			}

			bool IsOperator(const std::string_view& token) {
				return operators.contains(token);
			}


			bool IsKeyword(const std::string_view& token) {
				return keywords.contains(token);
			}


			bool IsConditional(const std::string_view& token) {
				return conditionals.contains(token);
			}

			bool IsPunctuation(const std::string_view& token) {
				return punctuation.contains(token);
			}


			std::string CompilePattern() const
			{
				if (compiledPattern.empty())
				{

					auto sort_code = [](auto& a, auto& b) { return a.size() > b.size(); };

					std::vector<std::string_view> ordered;

					ordered.reserve(operators.size() + punctuation.size());

					ordered.insert_range(ordered.end(), operators);
					ordered.insert_range(ordered.end(), punctuation);

					std::sort(ordered.begin(), ordered.end(), sort_code);

					std::string result = mainPattern + "|";

					for (auto& code : ordered) {

						if (code.size() == 0)
							continue;

						std::string submit;

						if (std::isalpha(code[0]) != 0) {
							submit = code;
						}
						else {
							submit = std::format(R"(\Q{}\E)", code);
						}


						result += std::format(R"((?:{})|)", submit);
					}

					result += remains;//Should end with "|"

					compiledPattern = result;
				}

				return compiledPattern;
			}

		};

		struct ParseModule;

		using ModuleBuilder = std::unique_ptr<ParseModule>(*)();



		ENUM(ModulePriority, uint32_t)
		{
			None = 0x00000000,
				Minimal = 0x00000001,
				Low = 0x000000FF,
				Medium = 0x0000FFFF,
				High = 0x00FFFFFF,
				Max = 0xFFFFFFFF
		};


		ENUM(ParseKeyword, uint8_t)
		{
			None = 0,
				Statement = 1 << 0,

		};


		ENUM(ModuleFlag, uint32_t)//This is just passed around so it can be as big as it wants to be.
		{
			None = 0,
				Atomic = 1 << 0,
				Eof = 1 << 1,//Parser can be used at EOF
		};

		ENUM(ParseFlag, uint32_t)//This is just passed around so it can be as big as it wants to be.
		{
			None = 0,
				Atomic = 1 << 0,
				Direct = 1 << 1,//This parser is being used from another parser, different rules might apply
		};


		struct ParseModule : public ProcessContext
		{
		protected:
			struct SetBuilder
			{
				SetBuilder(ModuleBuilder ctor, const std::string_view& name)
				{
					//move ctor to some place where it can be used.
					ParseModule::AddBuilder(ctor, name);
				}
			};


		private:
			inline static std::vector<std::pair<std::string_view, ModuleBuilder>> buildList;
		



		public:


			//Change Question to Query, 
			//Query to Try,
			// and try to Expect or smth

			static Record _ExecuteModule(ParseModule* mdl, Parser* parser, Record* target);


			static bool _QuestionModule(ParseModule* mdl, Parser* parser, Record* target, ParseFlag flag);

			static bool _QueryModule(ParseModule* mdl, Parser* parser, Record& out, Record* target);


			static Record _TryModule(ParseModule* mdl, Parser* parser, Record* target);

			//Doing this here because templates have to be inline and I can't provide that.
			static ParseModule* _GetBuiltModule(Parser* parser, const std::type_info& info);
		public:


			//TODO:Make ParseModule::GetPriority a constexpr function, this way I don't have to worry about anyone changing the values at runtime.
			virtual uint32_t GetPriority() const;


			virtual bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const = 0;

			virtual Record HandleToken(Parser* a_this, Record* target) = 0;


			//Here I'll place a line of common context keywords that will be enabled by using one of these.
			virtual ParseKeyword GetKeywords();

			std::optional<bool> GetKeywordState(std::string_view str) override;


			//Controls what can be percieved as being part of a single statement when just trying to encapsulate the next valid statement


			//This is what should be used if one wishes to access another parse module
			// also instead of requiring it derives from this, require that it derive from a singleton class for parse modules.
			template <std::derived_from<ParseModule> Module>
			static Record ExecuteModule(Parser* parser, Record* target)
			{
				//Static HandleToken (to be ExecuteModule) will not accept constraint of being from "PivotSingleton<ParseModule>". Needs investigation.
				// note, it will if it derives from ParseModule

				ParseModule* mdl = _GetBuiltModule(parser, typeid(Module));

				std::unique_ptr<ParseModule> new_mod;

				if (!mdl) {
					new_mod = std::make_unique<Module>();
					mdl = new_mod.get();
				}

				return _ExecuteModule(mdl, parser, target);

				//Doing it like this deals with process chains.
				//parser->_ExecuteModule(result, target, mdl);
				//return result;

				//Might have something about process chains right here.
				//Use a different version plz.
				//return mdl->HandleToken(parser, target);
			}

			template <std::derived_from<ParseModule> Module>
			static bool QueryModule(Parser* parser, Record& out, Record* target)
			{
				//Does what TryModule does, but will return in the case that it fails instead of croaking..
				//Try module will try to use use try module, and if it's unsuccessful, it will croak.
				// Basically a checked ParseAtomic

				ParseModule* mdl = _GetBuiltModule(parser, typeid(Module));

				std::unique_ptr<ParseModule> new_mod;

				if (!mdl) {
					new_mod = std::make_unique<Module>();
					mdl = new_mod.get();
				}

				return _QueryModule(mdl, parser, out, target);
				//return parser->_TryModule(out, target, mdl);
			}

			template <std::derived_from<ParseModule> Module>
			static Record TryModule(Parser* parser, Record* target)
			{
				ParseModule* mdl = _GetBuiltModule(parser, typeid(Module));

				std::unique_ptr<ParseModule> new_mod;

				if (!mdl) {
					new_mod = std::make_unique<Module>();
					mdl = new_mod.get();
				}

				return _TryModule(mdl, parser, target);
			}
			template <std::derived_from<ParseModule> Module>
			static bool QuestionModule(Parser* parser, Record* target, ParseFlag flag)
			{
				ParseModule* mdl = _GetBuiltModule(parser, typeid(Module));

				std::unique_ptr<ParseModule> new_mod;

				if (!mdl) {
					new_mod = std::make_unique<Module>();
					mdl = new_mod.get();
				}

				return _QuestionModule(mdl, parser, target, flag);
			}

			virtual std::string FailureMessage() const
			{
				return "Module failed to parse";
			}

			//Should actually be pure but it's defined for ease of use right now.
			std::string_view GetContext() override
			{
				return "";
			}

			virtual std::string_view GetModuleName() = 0;

			virtual ParseMode GetParseMode() const
			{
				return ParseMode::kBasic;
			}



			virtual bool IsAtomic() const { return false; }

			virtual bool CanParseEOF() const { return false; }

			//New stuff



			static void AddBuilder(ModuleBuilder build, const std::string_view& category)
			{
				auto pair = std::make_pair(category, build);
				buildList.insert(std::upper_bound(buildList.begin(), buildList.end(), pair, [](auto& lhs, auto& rhs)
					{return lhs.second()->GetPriority() > rhs.second()->GetPriority(); }),
					pair);
			}

			static std::vector<std::unique_ptr<ParseModule>> BuildModules(const std::string_view& name)
			{
				std::vector<std::unique_ptr<ParseModule>> result{};
				result.reserve(buildList.size());

				for (auto& [category, builder] : buildList)
				{
					if (category.empty() || category.starts_with(name) == true) {
						result.push_back(builder());
					}
				}

				return result;
			}


			virtual bool HasConditional(const std::string_view& name) const
			{
				return false;
			}


		};





		Record ParseModule::_ExecuteModule(ParseModule* mdl, Parser* parser, Record* target)
		{
			if (!mdl) {
				parser->GetInput()->croak("something about static handle token.");
			}

			Record result{};

			//Doing it like this deals with process chains.
			parser->_ExecuteModule(result, target, mdl);

			return result;

			//Might have something about process chains right here.
			//Use a different version plz.
			//return mdl->HandleToken(parser, target);
		}



		bool ParseModule::_QuestionModule(ParseModule* mdl, Parser* parser, Record* target, ParseFlag flag)
		{
			if (!mdl) {
				parser->GetInput()->croak("something about static handle token.");
			}

			return parser->_QueryModule(target, mdl, flag);
		}

		bool ParseModule::_QueryModule(ParseModule* mdl, Parser* parser, Record& out, Record* target)
		{
			if (!mdl) {
				parser->GetInput()->croak("something about static handle token.");
			}

			return parser->_TryModule(out, target, mdl, ParseFlag::Direct);
		}




		Record ParseModule::_TryModule(ParseModule* mdl, Parser* parser, Record* target)
		{

			if (!mdl) {
				parser->GetInput()->croak("something about static handle token.");
			}


			Record result{};

			//Try module will try to use use try module, and if it's unsuccessful, it will croak.
			// Basically a checked ParseAtomic for specific modules.
			if (_QueryModule(mdl, parser, result, target) == false)
				parser->GetInput()->croak(mdl->FailureMessage());

			return result;
		}

		ParseModule* ParseModule::_GetBuiltModule(Parser* parser, const std::type_info& info)
		{
			return parser->GetBuiltModule(info);
		}



		uint32_t ParseModule::GetPriority() const
		{
			return ModulePriority::Minimal;
		}

		/*
		std::optional<bool> ParseModule::GetKeywordState(std::string_view str)
		{
			switch (Hash(str))
			{
			case "Statement"_ih:
				return GetKeywords() & ParseKeyword::Statement;
			}

			return false;
		}
		//*/



		template <typename T, StringLiteral Cat = "">
		struct AutoParser : public ParseModule//The idea behind the auto parser is that there are some par
		{
		private:
			static std::unique_ptr<ParseModule> GetParseModule() { return std::make_unique<T1>(); }

			inline static SetBuilder _initBuild{ GetParseModule, Cat };

		public:
		};















		using Iterator = std::string_view::const_iterator;

		class ParsingStream;

		//Returns the parsing the beginning of the new token with it becoming the end of that token.
		using TokenParser = Iterator(ParsingStream*, Iterator& it, Iterator);



		//I'd like a macro system as a part of a preprocessor system that couples with handling the preprocessor inputs and where they occur

		class ParsingStream : public IProcess
		{
		public:

			//Friends with just the parse module
			friend class Parser__;
			friend class ParseModule;


			struct Memory
			{

				Column column = 0;
				Line line = 0;
				Iterator current;
				RecordData peek;
				RecordData prev;

			};

			
			
			
			ParsingStream(const std::string_view& str, Line l = 1, Column c = 1) : _stream{ str }
			{
				_memory.column = endColumn = c;
				_memory.line = endLine = l;

				//Will croak without outputting line and column at this point.

				//TODO: Change this to have a single object that handles this
				//std::string kinda_basic = TokenHandler::GetRegex(false);

				//_regex = boost::regex{ kinda_basic };

				_memory.current = _stream.cbegin();
				_end = _stream.cend();
				CalcColumnLine(_end, endColumn, endLine);
			}


			ParsingStream(const std::string_view& str, Tokenizer* tokenizer, std::function<TokenParser> func, Line l = 1, Column c = 1) {}


			void CalcColumnLine(Iterator from, Iterator until, Column& a_column, Line& a_line)
			{
				//The lines and columns calculated are off. Need to fix it somehow.

				//Better to just use a while actually.
				auto test = std::count_if(from, until, [&](auto it)
					{//will count the number of line breaks and when one is detected it will reset the column
						bool res = it == '\n' || it == '\r';
						if (res) {
							a_column = 1;

							if (max_value<Line> == a_line)
								croak(std::format("Max number of line reached, more than {} lines.", max_value<Line>));

							a_line += 1;
						}
						else {
							if (max_value<Column> == a_column)
								croak(std::format("Column at line {} exceeds max character limit of {}.", a_column, max_value<Column>));

							a_column += 1;
						}
						return res;
					});
			}

			void CalcColumnLine(Iterator until, Column& a_column, Line& a_line)
			{
				return CalcColumnLine(current(), until, a_column, a_line);
			}

			void croak(std::string msg, Token* token = nullptr){}


			//Proper name would be nexttoken or producetoken
			RecordData ReadNextImpl(const std::function<TokenParser>& func)
			{
				//This should be an impl function, the outer will keep calling this until it returns a "Valid" record.
				// Validity is in the eye of the beholder due to it throwing away what it finds. 
				//if (eof() == true)
				//	return {};

				auto curr = current();

				if (curr == _end)
					return {};


				auto res = func(this, curr, _end);


				if (curr == res) {
					report::parse::error("nothing moved, issue");
				}

				CalcColumnLine(res, _memory.column, _memory.line);

				auto data = RecordData{ std::string(res, curr), Token { column(), line() } };

				if (!data)
					croak("Empty string requested for use");

				current() = res;

				CalcColumnLine(curr, _memory.column, _memory.line);

				current() = curr;
				

				//If the creater was not a default, this will use whatever it's given as custom.
				if (&func != &defaultParser)
					data.TOKEN().type = TokenType::Custom;
				else {
					SetTokenType(data);
				}


				if (tokenizer && tokenizer->IsConditional(data.GetView()) == true) {
					data.TOKEN().isConditional = true;
				}


				return data;
			}

			RecordData ReadNext(const std::function<TokenParser>& func)
			{	
				do
				{
					RecordData result = ReadNextImpl(func);

					if (result.TOKEN().type != TokenType::Comment)
						return result;
				} while (true);
			}




			constexpr Line line() const noexcept { return _memory.line; }
			constexpr Column column() const noexcept { return _memory.column; }

			RecordData CheckConditional(RecordData data)
			{
				//This will should check the currently queried parse module if it has a conditional with this name.
				if (data.TOKEN().isConditional && false)
					data.TOKEN().type = TokenType::Keyword;

				return data;
			}


			RecordData next()
			{
				//Using this while eof should likely result in a parsing error.

				RecordData token = _memory.peek;

				_memory.peek = {};//Should make a clear func for this.

				if (!token) {
					token = ReadNext(defaultParser);
				}

				_memory.prev = token;

				return CheckConditional(token);
			}

			RecordData peek() {
				//So it's something 
				if (!_memory.peek) {
					//auto it = _current;
					//_peek = _ReadNext(it);
					_memory.peek = ReadNext(defaultParser);
				}

				return CheckConditional(_memory.peek);
			}


			RecordData prev()
			{
				return _memory.prev;
			}


			bool eof() {
				return  current() == _end || !peek();
			}

			



			bool Search(const std::string& token, const std::vector<std::string_view>& strings)
			{
				//move to source.
				for (int i = 0; i < strings.size(); i++)
				{
					if (token == strings[i])
						return true;
				}

				return false;
			}


			void SetTokenType(RecordData& data)
			{

				constexpr char quote_char = '\"';
				constexpr char apost_char = '\'';

				std::string& token = data.GetTag();
				TokenType& type = data.TOKEN().type;
				//FOR ALL THINGS THAT SEARCH FRONT AND BACK, only the front should be exampled, however, the back should be a question for failure. Make that a different function
				// that should croak if need be. Such a thing should always return true.

				
				if (std::isdigit(token.front()) || token.front() == '.' && token.size() > 1 && std::isdigit(token[1])) {
					type = TokenType::Number;
				}
				else if (token.front() == quote_char || token.front() == apost_char) {
					type = TokenType::String;
				}
				else if (token.find(":{") == 0 && token.rfind("}") == token.size() - 1) {
					type = TokenType::Object;
				}
				else if (token == "true" || token == "false" || token == "maybe") {
					type = TokenType::Boolean;
				}
				else if (std::isalpha(token.front()) || token.front() == '_') {
					type = TokenType::Identifier;
				}
				else if (Search(token, TokenHandler::GetKeywords())) {
					type = TokenType::Keyword;
				}
				else if (Search(token, TokenHandler::GetPuncutation())) {
					type = TokenType::Punctuation;
				}
				else if (Search(token, TokenHandler::GetOperators())) {
					type = TokenType::Operator;
				}
				else if (token == "\n") {
					type = TokenType::Whitespace;
				}
				//else if (true) {
				//  return token.starts_with(parse_strings::format_start);
				//	type = TokenType::Format;
				//}
				else {
					report::parse::error("Tokenizing: Unidentified token '{}' detected", data.GetTag());
				}



#ifdef DISABLE_COMMENT
				case TokenType::Comment:
					if (auto size = token.size(); size >= 2) {
						int res = std::strncmp(token.c_str(), "//", 2) == 0 ?
							1 : std::strncmp(token.c_str(), "/*", 2) == 0 ?
							2 : 0;
						//IE the second comment type
						const char& end = *(token.end() - 2);

						if (res == 2 && std::strncmp(&end, "*/", 2) != 0) {
							//Later, this is to be a generic function, able to print a generic name.
							croak("COMMENT unclosed at end of file.", &data.TOKEN());
						}

						return res;
					}
					else
						return false;
#endif	

			}



			Record Parse(bool atomic) {
				bool success;

				int i = 0;

				Record result{};
				Record* nested = nullptr;
				do
				{
					success = _SearchModule(result, nested, atomic);

					//should only crash if atomic modules fails
					if (!success && !nested)
						unexpected();

					nested = &result;

					i++;
				} while (success && (!eof() || nested));//eof to prevent searches once past the point.

				return result;
			}


			Record ParseAtomic() {
				Record result = Parse(true);
				return result;
			}

			Record ParseExpression() {
				Record result = Parse(false);
				return result;
			}

			void LinkContext(ProcessChain& chain)
			{
				//I want to give more power to these functions, so much that they manage the process of construction too
				contextChain = &chain;
			}

			void ExecuteModule(Record& record, Record* rec_nest, ParseModule* mdl)
			{
				//This version of the function should have no checks, it's expected that it should work, then any error fatal to the process.
				//logger::info("C {}", peek().GetTag());
				//ProcessChain link = contextChain->InheritChain(mdl, contextChain);//(mdl, contextChain, this);
				ProcessChain link{ mdl, contextChain };

				//I think I want to bake the reset into it's blood
				//_LinkContext(link);
				//logger::info("C {}", peek().GetTag());
				record = mdl->HandleToken(this, rec_nest);



				//_UnlinkContext();
			}

			bool QueryModule(Record* rec_nest, ParseModule* mdl, ParseFlag flag)
			{
				//This is where the above module checks should go. Uses execute module
				// also note, try should probably not check the current context, as it's likely the one who fired it.
				// this allows more control where there are situations where the only ways a thing happens is if the parser is in control.

				//Mesures first if the module can handle the current situation, but then measures if the currently loaded context will allow such a thing.


				if (eof() && mdl->CanParseEOF() == false) {
					return false;
				}

				bool module_check = mdl->CanHandle(this, rec_nest, flag);
				bool context_check = module_check ? contextChain->current->ContextAllowed(mdl, contextChain) : false;

				return module_check && context_check;

			}

			bool TryModule(Record& out, Record* rec_nest, ParseModule* mdl, ParseFlag flag)
			{
				//This is where the above module checks should go. Uses execute module
				// also note, try should probably not check the current context, as it's likely the one who fired it. 
				// this allows more control where there are situations where the only ways a thing happens is if the parser is in control.

				//Mesures first if the module can handle the current situation, but then measures if the currently loaded context will allow such a thing.

				bool handle = QueryModule(rec_nest, mdl, flag);


				if (handle) {
					ExecuteModule(out, rec_nest, mdl);
				}

				return handle;

			}


			bool SearchModule(Record& out, Record* rec_nest, bool atomic)
			{
				//In truth, should access the ParseHandler


				for (std::unique_ptr<ParseModule>& mod : _modules)
				{
					if (!mod)
						continue;
					//The first doesn't count, if not nested atomic will not be considered.
					if (rec_nest && atomic && mod->IsAtomic() == false)
						continue;

					//If not atomic, and the parseModule isn't atomic
					bool success = TryModule(out, rec_nest, mod.get(), atomic ? ParseFlag::Atomic : ParseFlag::None);

					if (success)
						return true;
				}

				return false;

			}



			Record ParseTopLevel()
			{
				//I think this function could actually be allowed to be free.
				if (std::addressof(_init) != contextChain)
					croak("Cannot parse top level while parsing.");

				if (!contextChain)//Should be warning?
					croak("ParseStream is expired, cannot parse top level.");


				if (!contextChain->current)
					croak("No top level ParseModule detected.");

				ParseModule* mdl = dynamic_cast<ParseModule*>(contextChain->current);

				//If this can't work, that's one weird occurance. Probably not going to account for that, I'd rather a crash.

				Record result = mdl->HandleToken(this, nullptr);

				if (!eof())
					croak("Top level file finished not EOF");

				//Severing the chain means that this string has been parsed to it's full ability, and (likely) parsed properly.
				// make this a function btw.
				contextChain = nullptr;

				return result;
			}




		protected:
			const Iterator& current() const
			{
				return _memory.current;
			}
			
			Iterator& current()
			{
				return _memory.current;
			}

			Tokenizer* tokenizer = nullptr;
			std::function<TokenParser> defaultParser;



			std::string_view _stream;

			Memory _memory;

			Iterator _end;

			Column endColumn;
			Line endLine;

			ProcessChain _init = CreateChain();//{ nullptr, nullptr, this };
			std::vector<std::unique_ptr<ParseModule>> _modules;


		};





		auto lambda_for_parser(Tokenizer* tokenizer)
		{
			boost::regex regex{ tokenizer->CompilePattern()};

			return [regex, tokenizer](ParsingStream* stream, Iterator& it, Iterator end) -> Iterator
				{
					if (it == end)
						return {};


					boost::match_results<Iterator> what;

					if (boost::regex_search(it, end, what, regex) == true) {

						auto& subject = what[0];


						if (subject.length() == 0)
							stream->croak(std::format("empty string found in parse results {}", std::string(it, end)));

						it = subject.second;

						return subject.first;
					}

					it = end;

					return {};
				};

		}




#ifndef NOT_SHOWING
		class Parser : public IProcess
		{
			//Friends with just the parse module
			friend class Parser__;
			friend class ParseModule;

			Parser(TokenStream& t, ParseModule* mdl);

			//parser should come with a context, this shit aint 
			Parser(TokenStream&& t, ParseModule* mdl);



			void _LinkContext(ProcessChain& chain);

			void _UnlinkContext();

			void _ExecuteModule(Record& record, Record* rec_nest, ParseModule* mdl);

			bool _QueryModule(Record* rec_nest, ParseModule* mdl, ParseFlag flag);

			bool _TryModule(Record& out, Record* rec_nest, ParseModule* mdl, ParseFlag flag);


			bool _SearchModule(Record& out, Record* rec_nest, bool atomic);


			Record _Parse(bool atomic);


			Record _ParseTopLevel();


		public:



			bool IsType(TokenType type, std::string_view str = "");

			RecordData ConsumeType(TokenType type, std::string_view str = "");

			void SkipType(TokenType type, std::string_view str);

			bool SkipIfType(TokenType type, std::string_view str);

			//Would like something called required type which fulfills the role of next and istype

			bool WasType(TokenType type, std::string_view str = "");



			std::vector<Record> Delimited(std::string_view start, std::string_view stop, std::string_view separator, std::function<ParseFunc> func);


			std::vector<Record> Delimited(std::string_view start, std::string_view stop, std::function<void()> separator, std::function<ParseFunc> func);


			//A shorthand so new lambdas don't need to get made contantly to send arg-less calls.
			std::vector<Record> Delimited(std::string_view start, std::string_view stop, std::string_view separator, std::function<Record()> func)
			{
				//std::function<ParseFunc> _b = nullptr;

				//_b = begin ? [=](auto, auto) { return begin(); } : _b;

				return Delimited(start, stop, separator, [=](auto, auto) { return func(); });
			}

			//template <typename TClass>
			std::vector<Record> Delimited(std::string start, std::string stop, std::string separator, std::function<Record(Parser*)> func)
			{
				return Delimited(start, stop, separator, [=](Parser* a1, auto) { return func(a1); });
			}



			//A shorthand so new lambdas don't need to get made contantly to send arg-less calls.
			std::vector<Record> Delimited(std::string start, std::string stop, std::function<void()> separator, std::function<Record()> func)
			{
				//std::function<ParseFunc> _b = nullptr;

				//_b = begin ? [=](auto, auto) { return begin(); } : _b;

				return Delimited(start, stop, separator, [=](auto, auto) { return func(); });
			}



			//template <typename TClass>
			std::vector<Record> Delimited(std::string start, std::string stop, std::function<void()> separator, std::function<Record(Parser*)> func)
			{
				return Delimited(start, stop, separator, [=](Parser* a1, Record*) { return func(a1); });
			}


			//std::vector<Record>

			void unexpected();


			ProcessChain GetChain();


			//I think the above works like this. 
			// Search looks for a module.
			// Try will try to use that module
			// execute will execute, regardless of conditions so the last thing needs to check itself.






			//This is the only function that's actually required now.
			Record ParseExpression();

			Record ParseAtomic();


			Record EndExpression(Record& rec);
			Record EndExpression(Record&& rec);



			TokenStream* GetTokenizer();

			InputStream* GetInput();


			std::string name();

			//Would like to seperate these from parser(steam) and move it to Parser__ (to be named Parser)
			//Would also like to remove the expression from this, there's no reason for it to be.
			static Record CreateExpression(std::string str, Syntax expr = SyntaxType::None, std::vector<Record> children = {});

			//This has no reason to be in here specifically.
			static Record CreateExpression(RecordData data, Syntax expr, std::vector<Record> children = {});



			template<std::same_as<Record>... Rs>requires (sizeof...(Rs) != 0)
				static Record CreateExpression(RecordData data, Syntax expr, Rs... records)
			{
				return CreateExpression(data, expr, { records... });
			}

			ParseModule* GetBuiltModule(const std::type_info& ref) const
			{
				const std::type_info* other = &ref;

				for (auto& mod : _modules)
				{
					auto ptr = mod.get();

					auto info = &typeid(*ptr);

					if (info == other)
					{
						return ptr;
					}
				}

				return nullptr;
			}
			template <std::derived_from<ParseModule> Module>
			ParseModule* GetBuiltModule() const
			{
				return GetBuiltModule(typeid(Module));
			}









		private:
			TokenStream& tokenizer;
			ProcessChain _init = CreateChain();//{ nullptr, nullptr, this };
			std::vector<std::unique_ptr<ParseModule>> _modules;


		public:
			//The idea is adding to this allows things to be registered regardless of order, or skip the line.
			// or really, anything. It's upto interpretation. But I wanted to use it for some preprocessors.
			//std::vector<Record> persistent;


			//Would like this to be private too honestly, and const when given.
			ProcessChain* contextChain = &_init;


			void FlagFailure()
			{
				_failure = true;
			}

			bool _failure = false;

			//Note, a statement like this would need parser memory
			//1 < 2 > (4);



			//TODO:An init function for Parser will need to take a parsemodule instead, which sets the base for init, as 
			// well as the entry parse module, who's carried out without consideration.
		};


		Parser::Parser(TokenStream& t, ParseModule* mdl) : tokenizer{ t }, _modules{ ParseHandler::BuildModules() }
		{
			_init.current = mdl;
		}

		//parser should come with a context, this shit aint 
		Parser::Parser(TokenStream&& t, ParseModule* mdl) : tokenizer{ t }
		{
			_init.current = mdl;
		}



		void Parser::_LinkContext(ProcessChain& chain)
		{
			//I want to give more power to these functions, so much that they manage the process of construction too
			contextChain = &chain;
		}

		void Parser::_UnlinkContext()
		{
			//Add checks please
			contextChain = contextChain->previous;
		}

		void Parser::_ExecuteModule(Record& record, Record* rec_nest, ParseModule* mdl)
		{
			//This version of the function should have no checks, it's expected that it should work, then any error fatal to the process.
			//logger::info("C {}", peek().GetTag());
			//ProcessChain link = contextChain->InheritChain(mdl, contextChain);//(mdl, contextChain, this);
			ProcessChain link{ mdl, contextChain };

			//I think I want to bake the reset into it's blood
			//_LinkContext(link);
			//logger::info("C {}", peek().GetTag());
			record = mdl->HandleToken(this, rec_nest);



			//_UnlinkContext();
		}

		bool Parser::_QueryModule(Record* rec_nest, ParseModule* mdl, ParseFlag flag)
		{
			//This is where the above module checks should go. Uses execute module
			// also note, try should probably not check the current context, as it's likely the one who fired it.
			// this allows more control where there are situations where the only ways a thing happens is if the parser is in control.

			//Mesures first if the module can handle the current situation, but then measures if the currently loaded context will allow such a thing.


			if (eof() && mdl->CanParseEOF() == false) {
				return false;
			}

			bool module_check = mdl->CanHandle(this, rec_nest, flag);
			bool context_check = module_check ? contextChain->current->ContextAllowed(mdl, contextChain) : false;

			return module_check && context_check;

		}

		bool Parser::_TryModule(Record& out, Record* rec_nest, ParseModule* mdl, ParseFlag flag)
		{
			//This is where the above module checks should go. Uses execute module
			// also note, try should probably not check the current context, as it's likely the one who fired it. 
			// this allows more control where there are situations where the only ways a thing happens is if the parser is in control.

			//Mesures first if the module can handle the current situation, but then measures if the currently loaded context will allow such a thing.

			bool handle = _QueryModule(rec_nest, mdl, flag);


			if (handle) {
				_ExecuteModule(out, rec_nest, mdl);
			}

			return handle;

		}


		bool Parser::_SearchModule(Record& out, Record* rec_nest, bool atomic)
		{
			//In truth, should access the ParseHandler


			for (std::unique_ptr<ParseModule>& mod : _modules)
			{
				if (!mod)
					continue;
				//The first doesn't count, if not nested atomic will not be considered.
				if (rec_nest && atomic && mod->IsAtomic() == false)
					continue;

				//If not atomic, and the parseModule isn't atomic
				bool success = _TryModule(out, rec_nest, mod.get(), atomic ? ParseFlag::Atomic : ParseFlag::None);

				if (success)
					return true;
			}

			return false;

		}



		Record Parser::_ParseTopLevel()
		{
			//I think this function could actually be allowed to be free.
			if (std::addressof(_init) != contextChain)
				croak("Cannot parse top level while parsing.");

			if (!contextChain)//Should be warning?
				croak("ParseStream is expired, cannot parse top level.");


			if (!contextChain->current)
				croak("No top level ParseModule detected.");

			ParseModule* mdl = dynamic_cast<ParseModule*>(contextChain->current);

			//If this can't work, that's one weird occurance. Probably not going to account for that, I'd rather a crash.

			Record result = mdl->HandleToken(this, nullptr);

			if (!eof())
				croak("Top level file finished not EOF");

			//Severing the chain means that this string has been parsed to it's full ability, and (likely) parsed properly.
			// make this a function btw.
			contextChain = nullptr;

			return result;
		}



		RecordData Parser::next()
		{
			return tokenizer.next();
		}
		RecordData Parser::peek()
		{
			return tokenizer.peek();
		}

		RecordData Parser::prev()
		{
			return tokenizer.prev();
		}

		bool Parser::eof()
		{
			return tokenizer.eof();
		}
		void Parser::croak(std::string msg, Token* token)
		{
			return GetInput()->croak(msg, token);
		}



		bool Parser::IsType(TokenType type, std::string_view str) {
			RecordData tok = tokenizer.peek();

			if (!tok)
				return false;

			return (type == TokenType::Invalid || tok.GetEnum<Token>().type == type) && (str == "" || tok.GetTag() == str);
		}

		bool Parser::WasType(TokenType type, std::string_view str) {
			RecordData tok = tokenizer.prev();
			return tok && tok.GetEnum<Token>().type == type && (str == "" || tok.GetTag() == str);
		}


		RecordData Parser::ConsumeType(TokenType type, std::string_view str)
		{
			//TODO:Fix Format #1
			//TokenToString
			if (IsType(type, str) == false) {
				auto token = peek();
				tokenizer.croak(std::format("Expecting {}, recieved {} from \"{}\"", magic_enum::enum_name(type), magic_enum::enum_name(token.TOKEN().type), token.GetTag()));
			}

			return tokenizer.next();
		}

		void Parser::SkipType(TokenType type, std::string_view str)
		{
			ConsumeType(type, str);
		}

		bool Parser::SkipIfType(TokenType type, std::string_view str)
		{
			bool result = IsType(type, str);

			if (result)
				tokenizer.next();

			return result;
		}

		std::vector<Record> Parser::Delimited(std::string_view start, std::string_view stop, std::function<void()> separator, std::function<ParseFunc> func) {
			//I would like a delimit that expects the seperator to be the second to last, and another that prevents it from being second to last.
			//Having a version of this with no parameters or just parser would be ideal, that way I can use member functions.

			//Checking for first might not really be that difficult.

			std::vector<Record> result;

			bool first = true;

			//if it's empty it will consume anything.
			if (start.empty() == false)
				SkipType(TokenType::Invalid, start);

			while (tokenizer.eof() == false) {
				//bool start = first;

				if (IsType(TokenType::Invalid, stop) == true) break;

				//RGL_LOG(debug, "skipping seperator '{}'", separator);

				if (first) first = false;
				else if (separator) separator();

				if (IsType(TokenType::Invalid, stop) == true) break;

				//Thinking of using the second arg for something.
				//Record entry = start && begin ? begin(this, nullptr) : func(this, nullptr);
				Record entry = func(this, nullptr);

				if (entry) {
					result.push_back(entry);
				}
				else {
					report::parse::trace("empty record discared");
				}
			}

			SkipType(TokenType::Invalid, stop);

			return result;
		}




		//Make a version of this that can take a parser and record, and a version that can take nothing at all.
		std::vector<Record> Parser::Delimited(std::string_view start, std::string_view stop, std::string_view separator, std::function<ParseFunc> func) {
			return Parser::Delimited(start, stop, [&]() { SkipType(TokenType::Punctuation, separator); }, func);
		}



		void Parser::unexpected() {
			//want this to look prettier at some point.
			//TODO:Fix Format #2
			tokenizer.croak(std::format("Unexpected token: {} ({})", tokenizer.peek().GetTag(), magic_enum::enum_name(tokenizer.peek().TOKEN().type)));//"Unexpected token");//
		}


		ProcessChain Parser::GetChain()
		{
			return *contextChain;
		}

		Record Parser::_Parse(bool atomic) {
			bool success;

			int i = 0;

			Record result{};
			Record* nested = nullptr;
			do
			{
				success = _SearchModule(result, nested, atomic);

				//should only crash if atomic modules fails
				if (!success && !nested)
					unexpected();

				nested = &result;

				i++;
			} while (success && (!eof() || nested));//eof to prevent searches once past the point.

			return result;
		}


		Record Parser::ParseAtomic() {
			Record result = _Parse(true);
			return result;
		}

		Record Parser::ParseExpression() {
			Record result = _Parse(false);
			return result;
		}

		Record Parser::EndExpression(Record& rec)
		{
			ParseModule::ExecuteModule<EndParser>(this, nullptr);
			return std::move(rec);
		}


		Record Parser::EndExpression(Record&& rec)
		{
			ParseModule::ExecuteModule<EndParser>(this, nullptr);
			return std::move(rec);
		}

		TokenStream* Parser::GetTokenizer()
		{
			return &tokenizer;
		}

		InputStream* Parser::GetInput()
		{
			return GetTokenizer()->GetInput();
		}


		std::string Parser::name()// const
		{
			return GetInput()->name();
		}

		//Would like to seperate these from parser
		// Would also like to make "create header" for this.
		Record Parser::CreateExpression(std::string str, Syntax expr, std::vector<Record> children)
		{
			return Record{ str, expr, children };
		}

		//This has no reason to be in here specifically.
		Record Parser::CreateExpression(RecordData data, Syntax expr, std::vector<Record> children)
		{
			auto tok = data.GetEnum<Token>();

			expr.line = tok.line;
			expr.column = tok.column;

			return Record{ data.GetTag(), expr, std::move(children) };
		}



#endif


	}


}
//*/