#pragma once



//#include "Object.h"
//#include "ObjectID.h"
//#include "ObjectIDMapper.h"

#include "Lexicon/Engine/Operand.h"


#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"
#include "Lexicon/Interfaces/ProjectManager.h"

#include "Lexicon/Exception.h"
//#include "Parser.h"
#include "Lexicon/Engine/ParserTest.h"



#include "Lexicon/Variable.h"



//#include "SignatureManager.h"//suppressed for now

//This is a test space for this. I'd like to move it to a more fitting location.

//New
//*
#include "Lexicon/Engine/Instruction.h"
#include "Lexicon/Engine/InstructionType.h"
#include "Lexicon/Engine/OperatorType.h"
#include "Lexicon/Engine/Runtime.h"
#include "Lexicon/RuntimeVariable.h"
#include "Lexicon/IVariable.h"
#include "Lexicon/Engine/GlobalVariable.h"
#include "Lexicon/Engine/Literal.h"
#include "Lexicon/Engine/LiteralManager.h"
#include "Lexicon/Engine/Scope.h"
#include "Lexicon/Engine/Solution.h"
#include "Lexicon/Engine/RoutineBase.h"
#include "Lexicon/Engine/Operation.h"
#include "Lexicon/Engine/Operand.h"
#include "Lexicon/Engine/OperandType.h"
#include "Lexicon/Engine/Target.h"
#include "Lexicon/RuntimeVariable.h"
#include "Lexicon/MemberPointer.h"
#include "Lexicon/Engine/RoutineCompiler.h"



#include "Lexicon/TypeID.h"


#include "Lexicon/Engine/ConcretePolicy.h"


#include "Lexicon/VariableType.h"

#include "Lexicon/Engine/ConcreteFunction.h"

#include "Lexicon/Engine/OverloadClause.h"




#include "Lexicon/Object.h"
#include "Lexicon/ObjectData.h"
#include "Lexicon/ObjectInfo.h"
#include "Lexicon/ObjectPolicy.h"
#include "Lexicon/ObjectPolicyHandle.h"
#include "Lexicon/Interfaces/ObjectPolicyManager.h"


#include "Lexicon/ObjectPoolData.h"





#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/InterfaceManager.h"
#include "Lexicon/Interfaces/InterfaceSingleton.h"
#include "Lexicon/Versioning.h"

#include "Lexicon/DeclareSpecifier.h"

#include "Lexicon/Engine/OverloadInput.h"

#include "Lexicon/Engine/Declaration.h"

#include "Lexicon/Interfaces/ProcedureHandler.h"
#include "Lexicon/Dispatcher.h"

#include "Lexicon/String.h"

#include "Lexicon/Engine/Signature.h"

#include "Lexicon/Engine/GenericBase.h"

#include "Lexicon/GenericPartArray.h"
#include "Lexicon/GenericBodyArray.h"
#include "Lexicon/Engine/GenericType.h"

#include "Lexicon/Interfaces/ReportManager.h"

#include "Lexicon/Engine/SyntaxRecord.h"

#include "Lexicon/RelateType.h"


#include "Lexicon/Engine/SettingManager.h"

#include "Lexicon/Formula.h"

#include "Lexicon/Engine/ConcreteGlobal.h"

#include "Lexicon/NativeReference.h"

namespace std
{
	template <class _Elem, class _Alloc>
	struct hash<vector<_Elem, _Alloc>>
	{
		_NODISCARD static size_t _Do_hash(const vector<_Elem, _Alloc>& _Keyval) noexcept {
			return _Hash_array_representation(_Keyval.data(), _Keyval.size());
		}
	};
}

//This is to be my method of hashing.
inline std::hash<std::vector<uint64_t>> hasher;


struct TestRef
{
	int& ref;

	TestRef(int& r) : ref{ r } {}
};


void TestFunc()
{
	int a = 1;
	LEX::RuntimeVariable var{};
	


	TestRef t{ a };

	TestRef t2 = t;
}

namespace LEX
{
	
	/////////////////////////
	//Implementations
	////////////////////////

#define CONSEAL(mc) //mc

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(Base)
			{

				virtual int GetBar() = 0;
				CONSEAL(int a_1);
				CONSEAL(int a_2);
				
			};

			struct INTERFACE_VERSION(Derives)
			{
				CONSEAL(int b_1);
				CONSEAL(int b_2);
			};
		}

		namespace _2
		{
			
			struct INTERFACE_VERSION(Base)
			{
				CONSEAL(int a_3);
				CONSEAL(int a_4);
				CONSEAL(int a_5);

			};
			
			struct INTERFACE_VERSION(Derives)
			{
				CONSEAL(int b_3);
			};
		}

		namespace _3
		{
			struct INTERFACE_VERSION(Derives)
			{
				CONSEAL(int b_4);
				CONSEAL(int b_5);
			};
		}

		CURRENT_VERSION(Base, 2);
		namespace Current {
			struct Derives : public _3::Derives {
				inline static constexpr uintptr_t version = 3; uintptr_t Version() const override final {
					return 3;
				}
			};
		};
	}


	struct IMPL_VERSION(Base)
	{
		
	};
	

	struct Derives : public Version::Current::Derives, public Base
	{
		virtual int GetBar() =0;
	};

	void TestD(Derives& foo)
	{
		foo.GetBar();

		Base& bar = foo;
	}

	
	/*
	struct TestA
	{
	};


	struct StructA
	{
		virtual TestA* Foo() = 0;
	};


	struct TestB;



	struct StructB : public StructA
	{
		TestA* Foo() override final;

		virtual TestB* Foo(int = 0) = 0;

	};


	

	struct TestB : public TestA {};

	TestA* StructB::Foo()
	{
		return Foo({});
	}

	struct StructC : public StructB 
	{
		TestB* Foo(int = 0) override
		{
			return nullptr;
		}

	};
	//*/




	namespace _1
	{
		namespace _2
		{
			struct Test
			{
				void foo()
				{

				}


			};
		}

		namespace _3
		{
			struct Test 
			{
				void foo()
				{

				}


			};
		}
	}

	struct Test : public _1::_2::Test, public _1::_3::Test
	{


	};

	namespace _A
	{
		namespace _B
		{
			namespace _C
			{
				int test;
			}
		}

		namespace _B1
		{
			namespace _B
			{
				namespace _C
				{
					int test;
				}
			}
		}

		namespace _A1
		{
			void Foo()
			{
				_B::_C::test;
			}
		}
	}

	void test_a(int&)
	{
	}

	//*
	//This may work, but it will need to use the respective type it's going to become.
	//In this case, guide type is a type that relates to the dispatcher
	template <typename GuideType, typename TupleType, std::size_t... Indices>
	auto tie_as_tuple(TupleType& t, std::index_sequence<Indices...>) {
		//std::forward_as_tuple<std::tuple_element_t<Indices, TupleType>...>
		return std::forward_as_tuple(std::forward<std::tuple_element_t<Indices, GuideType>>(std::get<Indices>(t))...);
	}

	template <typename... GuideTypes, typename TupleType>
	auto tie_as_tuple(TupleType& t) {
		return tie_as_tuple<std::tuple<GuideTypes...>>(t, std::make_index_sequence<std::tuple_size_v<TupleType>>{});
	}
	
	int main() {
		std::tuple<int, double, std::string> my_tuple{ 10, 3.14, "hello" };

		auto tuple_of_refs = tie_as_tuple<int&, double, std::string&>(my_tuple);

		//std::get<0>(tuple_of_refs) = 20;
		//std::get<1>(tuple_of_refs) = 6.28;
		//std::get<2>(tuple_of_refs) = "world";
		
		std::cout << std::get<0>(my_tuple) << " " << std::get<1>(my_tuple) << " " << std::get<2>(my_tuple) << std::endl;

		return 0;
	}
	//*/
	void test_b(int&&, int&)
	{
	}
	void TFunc()
	{
		//test_a(1);
		int t = 1;
		std::forward_as_tuple(1, t);

		const int& t2 = t;
		//test_b(t2, t);
		int&& t1 = std::forward<int>(t);
		test_a(std::forward<int&>(t));

		std::tuple<int&> in = t;
		std::tuple<int&&, int&>  in2{ std::forward<int>(std::get<0>(in)), std::get<0>(in) };
		//auto in3 = std::forward_as_tuple(in);
		//std::apply(test_b, std::forward<std::tuple<int, int>>(in));
		
		//As long as the parameters match, the tuples will be able to be forwarded. The only issue now, is loading the tuple with the expected types.
		std::apply(test_a, std::forward<decltype(in)>(in));
		std::apply(test_b, std::forward<decltype(in2)>(in2));
		//std::apply(test_b, in2);

		Test test;
		test._1::_2::Test::foo();
	}

	/*
	AbstractTypePolicy* VariableType<double>::operator()()
	{
		//I could just make this numeric
		static AbstractTypePolicy* result = nullptr;

		if (!result) {

			//offset
			constexpr auto setting = LEX::Number::Settings::CreateFromType<double>();

			auto buffer = LEX::IdentityManager::instance->GetTypeByOffset("NUMBER", setting.GetOffset());

			result = buffer->FetchTypePolicy(nullptr);

			logger::info("id? {}", (int)result->FetchTypeID());
		}

		return result;
	}
	//*/


	
	//*/



	/*
	template<numeric T>
	AbstractTypePolicy* GetVariableType<T>()
	{
		static AbstractTypePolicy* result = nullptr;

		if (!result) {

			constexpr auto setting = LEX::Number::Settings::CreateFromType<T>();


			auto buffer = LEX::IdentityManager::instance->GetTypeByOffset("NUMBER", setting.GetOffset());

			result = buffer->FetchTypePolicy(nullptr);

			logger::info("id? {}", (int)result->FetchTypeID());
		}

		return result;
	}
	//*/
	//TODO: This should be moved.



	//This is so temporary I hate that I'm doing it like this.
	


//#requires /:Option




	void TestForm()
	{
		float $ = 1;

		$ += 1;


		{
			std::string_view routine = "a + b + print_str.size()";

			auto formula = Formula<double(double, double, String)>::Create("a", "b", "print_str", routine);

			if (!formula) {
				logger::critical("Formula didn't take");
				return;
			}

			logger::info("{} = {}", routine, formula(15.0, 10.0, "fifth"));
			
			logger::info("2nd: {} = {}", routine, formula(15, 15, "fifthfourth"));
		}

		Formula<void(void)>::Create("otherTest() => otherTest() => otherTest() => otherTest()")();
		

		Formula<void>::RunDefault("otherTest() => otherTest() => otherTest()");
		
		std::system("pause");

		Variable result = Formula<Variable>::Run("(25).CouldBeLiterallyAnything()");

		logger::critical("{}", result.PrintString());
#ifdef KILL
		if (result.IsVoid())
		{
			logger::critical("is void");
		}
		else
		{
			logger::critical("value is {}", result.AsNumber());
		}
#endif

		IObjectVTable;
	}



	namespace FunctionalInlining
	{
		

		//This will need a fair few things from the compiler in order to make this rigt. So I'm not dealing with it for right now.
		inline InstructType preserve = InstructType::Invalid;
		inline InstructType restore = InstructType::Division;

		static void ReserveRegister(RuntimeVariable&, Operand a_lhs, Operand a_rhs, InstructType instruct, Runtime* runtime)
		{
			auto mode = a_lhs.Get<Index>();

			Register exception = a_rhs.Get<Register>();

			//thinking about it, I may want to put these on the argument register. Mainly because the argument register is more likely to have less permenant increments

			Register reg = (Register)(Register::Total - 1);

			if (preserve) {
				size_t size = runtime->AdjustStackPointer(StackPointer::Argument, Register::Total);

				size_t it = size - Register::Total;

				for (; reg < Register::Total; reg--) {
					//Register::Total

					if (reg != exception)
						runtime->GetArgument(it + reg) = runtime->GetRegister(reg);
				}
			} else {
				size_t size = runtime->GetStackPointer(StackPointer::Argument);

				size_t it = size - Register::Total;

				//mode should equal it.

				//When this loops before 0 stack overflow will happen
				for (; reg < Register::Total; reg--) {
					//Register::Total

					if (reg != exception)
						runtime->GetRegister(reg) = runtime->GetArgument(it + reg);
				}
			}
		}

		inline void InlineRoutine(RoutineCompiler* compiler, RoutineBase& routine)
		{
			//compiler->ModParamCount(routine.argCapacity);
			//compiler->
			//routine.

			//The arguments should already be expected to go into variable slots rather than call sites by this point.
		}
	}

	//Calling function helper constructs. 
	//Needs names other than target. It's called a ThisVariable

	namespace detail
	{
		
		struct ThisHelperImpl
		{
			using _Base = ThisHelperImpl;

			//This is the thing that actually does the calling. This is given based on whether the target is a reference
			// type, or a pointer type.

			//The callable functions will have a few different versions, depending on one's faith in the system/preference.
			//For now, I'm not going to care about the alternate versions of this. BUT here is the low down on what I want
			//-A central and somewhat raw function for how I want everything packaged and sent to the Runtime
			//-A function that has doesn't the given references as variables
			//-A function that does use the given references as variables.
			//^ For this, one could just forward instead. Also, const should always be taken by value some how.




			//bool Execute(Variable& out, std::vector<RuntimeVariable> args)

			Variable Call(ICallableUnit* call_unit, std::vector<Variable> args)
			{
				args.insert(args.begin(), _this);


				return {};
			}

			template <typename... Ts>
			Variable Call(ICallableUnit* unit, Ts&&... ts)
			{
				std::vector<Variable> args{};

				args.resize(sizeof...(Ts));

				return Call(unit, args);
			}


			//void* _this = nullptr;
			Variable _this;
		};



		//*

	//std::convertible_to<Variable>//I want to use this, but I cannot because it doesn't yet account for pointer
	// types. Readdress another time.
		template <typename T, typename = void>
		struct ThisHelper {};



		//This is for value and references
		template <typename T>
		struct ThisHelper<T, std::enable_if_t<!PointerType<T>>> : public detail::ThisHelperImpl
		{
			using _Self = ThisHelper<T>;

			//Should be convertible to a variable?

			//This is only to fulfill what someone would normally use


			ThisHelper(const T& t)
			{

			}

			ThisHelper(const T&& t)
			{

			}
		};

		//this is for pointers
		template <typename T>
		struct ThisHelper<T, std::enable_if_t<PointerType<T>>> : private ThisHelperImpl
		{
			using TypeC = int;
			using _Self = ThisHelper<T>;

			_Base* operator->()
			{
				//This gets the TargetBase, the thing that actually has the run function, to resume the 
				// syntax that you're accessing a member function (which you kinda are).

				return this;
			}


			ThisHelper(const T& t)
			{

			}

			ThisHelper(const T&& t)
			{

			}
		};


		template <typename T>
		ThisHelper(T) -> ThisHelper<T>;



		//this helper should be moved to something else.
	//maybe name METHOD?
#define METHOD(mc_tar) ThisHelper{ mc_tar }



		void test_method()
		{
			std::string* string_thing;
			std::vector testname{ string_thing };

			METHOD(string_thing)->Call(nullptr, 1, 3, 4);

		}
#undef METHOD
		//*/
	}
	
	

	namespace NumberNew
	{



		INITIALIZE()
		{
			return;
#undef max
#undef min
			uint8_t r = 0;
			bool tests[60]{};
			for (NumeralType a = (NumeralType)0; a < NumeralType::Total; a++)
			{
				for (Signage c = (Signage)0; c < Signage::Total; c++)
				{
					for (Size b = (Size)0; b < Size::Total; b++)
					{
						for (Limit d = (Limit)0; d < Limit::Total; d++)
						{
							//report::info("offset test {}-{}", GetNumberOffset(a, b, c, d), ++r);

						}

					}
				}
				logger::debug("end");
			}

			report::info("final {}", r);

			logger::critical("starting process now");

			Number testA = std::numeric_limits<int32_t>::max();
			Number testB = 266;


			auto lambda = [](auto lhs, auto rhs) { auto res = lhs * rhs; return res; };

			//auto data = testB.OperateData(4, [](auto lhs, auto rhs) { auto res = lhs * rhs; return res; });
			//report::debug("data is {}", data.sInteger);


			Number testResult = testA + testB;

			Number unsigned_int8 = (uint8_t)1;

			Number test_inf = infinity::ufloat64();


			report::debug("inf test {}", test_inf.string());

			unsigned_int8.Assign(testB);
			report::debug("uint8 {}", unsigned_int8.string());

			report::debug("{} + {} = {} ?", testA.string(), testB.string(), testResult.string());
			constexpr auto fill = std::numeric_limits<int8_t>::max();

			unsigned_int8.Visit([](auto& it) { it += 1; });

			report::debug("uint8 later {}", unsigned_int8.string());
		}


	}


	




	//Creator function for numbers

	inline void TestCreateAllSettings()
	{
		return;
		//This is the name of the unique policy for numbers, that can handle the conversions between things itself.
		using NumericPolicy = ConcretePolicy;

		std::vector<ConcretePolicy*> results;

		NumeralType type{};
		Size        size{};
		Signage     sign{};
		Limit       limit{ Limit::Overflow };

		IdentityManager::instance->GenerateID("NUMBER", Number::Settings::length);

		ConcretePolicy* primary_policy = new ConcretePolicy{ "NUMBER", 0 };

		for (int a = 0; a < NumeralType::Total; a++)
		{
			for (int b = 0; b < Size::Total; b++)
			{
				for (int c = 0; c < Signage::Total; c++)
				{
					for (int d = 1; a < Limit::Total; d++)
					{
						Number::Settings settings{ (NumeralType)a, (Size)b, (Signage)c, (Limit)d };
						ConcretePolicy* number_policy = new ConcretePolicy{ "NUMBER", settings.GetOffset() };
						Variable defaultValue{ settings, number_policy };
						number_policy->EmplaceDefault(defaultValue);
						results.emplace_back(number_policy);


					}
				}
			}
		}

		//static TypePolicy* float64 = new TypePolicy{ "NUMBER", Number::Settings::GetOffset(NumeralType::Floating) };

		//do something with values.
	}

	
	class IntrinsicPolicy : public ConcretePolicy
	{
		//The concept of an intrinsic policy is first simply that intrinsic policies are the only 
		// policies that can Claim a specific space. Think like how void would or something like that.
			
		//Second, further branches of it will be able to present default versions of things such as strings, numbers
		// arrays, etc. This prevents us from having to carry the variable around in every single type despite not needing it.

		//There will likely have to be one for generic, but every generic/concrete one won't need new satelites, as they
		// will just assume that it is a concrete policy. Which it is.





	};












	

		inline std::unique_ptr<ITemplatePart> temp_EncapTypes(std::vector<ITypePolicy*>& list)
		{
			//The point of this function would be to accept a number of types turning them into a GenericParameter, or a generic argument.

			std::vector<AbstractTypePolicy*> possible;

			auto size = list.size();

			for (auto& it : list)
			{
				if (it->IsResolved() == false)
				{
					GenericPartArray* _array = new GenericPartArray;
					std::unique_ptr<ITemplatePart> result{ _array };
					
					_array->_types = std::move(list);

					return result;
				}

				auto back = it->GetTypePolicy((ITemplateBody*)nullptr);
				possible.push_back(back);
			}

			GenericBodyArray* _array = new GenericBodyArray;
			
			std::unique_ptr<ITemplatePart> result{ _array };

			_array->_types = std::move(possible);

			return result;
		}








		//Maybe use spans instead of vectors.



		//Both special part and special body will likely become ITemplate somethings. Thinking about it, if  I have a function or type that needs 
		// to be specialized, it fits that this would be specializing it right?
		//If that happens get argument will not be required any longer.




		
		

		//These 2 can largely be boiler place tbh.
		









	




		INITIALIZE()
		{
			return;
			report::info("starting. . . .");

			GenericType to_spec;
			
			GenericType specifier1;//Will partially specialize
			GenericType specifier2;//will completely partually specialize
			GenericType specifier3;//will completely specialize.


			to_spec._template = { TemplateType{"T1", 0}, TemplateType{"T2", 1}, TemplateType{"T3", 2} };
			specifier1._template = { TemplateType{"T1", 0}, TemplateType{"T2", 1}, TemplateType{"T3", 2} };
			specifier2._template = { TemplateType{"T1", 0}, TemplateType{"T2", 1}, TemplateType{"T3", 2} };
			specifier3._template = { TemplateType{"T1", 0}, TemplateType{"T2", 1}, TemplateType{"T3", 2} };


			ITypePolicy* floatSmall = IdentityManager::instance->GetTypeByOffset("NUMBER", 42);
			ITypePolicy* floatBig = IdentityManager::instance->GetTypeByOffset("NUMBER", 45);
			ITypePolicy* stringB = IdentityManager::instance->GetTypeByOffset("STRING", 0);

			std::vector<ITypePolicy*> group1{ 3 };
			std::vector<ITypePolicy*> group2{ 3 };
			std::vector<ITypePolicy*> group3{ 3 };


			group1[0] = floatSmall;
			group1[1] = stringB;
			group1[2] = &specifier1._template[0];
			

			group2[0] = &specifier2._template[2];
			group2[1] = &specifier2._template[0];
			group2[2] = &specifier2._template[1];

			group3[0] = stringB;
			group3[1] = floatSmall;
			group3[2] = stringB;



			//auto 

			auto part1 = temp_EncapTypes(group1);
			auto part2 = temp_EncapTypes(group2);
			auto part3 = temp_EncapTypes(group3);

			//Nothing new was created in any of these, and all of these are the same object.

			auto type1 = to_spec.CheckTypePolicy(&specifier1, part1.get());
			auto type2 = to_spec.CheckTypePolicy(&specifier2, part2.get());
			auto type3 = to_spec.CheckTypePolicy(&specifier3, part3.get());
			specifier1.GetTypePolicy(part3->TryPromoteTemplate());
			specifier2.GetTypePolicy(part3->TryPromoteTemplate());
			report::info("exists? {} {} {}", (uintptr_t)type1, (uintptr_t)type2, (uintptr_t)type3);

			

			//This isn't supposed to make 3 new bodies

			//auto a2 = to_spec.GetTypePolicy(part3->TryPromoteTemplate());
			//auto a3 = to_spec.GetTypePolicy(part3->TryPromoteTemplate());

			auto a1 = type1->GetTypePolicy(part3->TryPromoteTemplate());
			auto a2 = type2->GetTypePolicy(part3->TryPromoteTemplate());
			auto a3 = type3->GetTypePolicy(part3->TryPromoteTemplate());

			auto a4 = to_spec.GetTypePolicy(part3->TryPromoteTemplate());


			report::info("exists? {} {} {} -> {}?", (uintptr_t)a1, (uintptr_t)a2, (uintptr_t)a3, (uintptr_t)a4);




			report::info("ending. . . .");
		}

	





















	struct Array_
	{
		int8_t size = 0;
		std::vector<Variable> stuff;

		Array_()
		{
			logger::info("Creating Array");
		}

		~Array_()
		{
			logger::info("Deleting Array");
		}


		std::string PrintString(std::string_view context) const
		{
			auto size = stuff.size();
			
			std::vector<std::string> entries{ size };
			std::string result = "[";
			
			for (int i = 0; i < size; i++)
			{
				if (i)
					result += ", ";

				result += stuff[i].PrintString();
				

			}
			result += "]";
			
			//Ypu've got all these fancy ways to do this, but I'm just gonna do this for now and see if that works.
			return result;
			
			/*
			std::string result{ reserve };
			
			result.reserve(reserve);

			result = std::accumulate(std::begin(entries), std::end(entries), result,
				[](std::string& ss, std::string& s)
				{
					return ss.empty() ? s : ss + "," + s;
				});


			std::stringstream  stream;
			
			std::copy(entries.begin(), entries.end(), std::ostream_iterator<std::string>(stream, ", "));
			
			return std::format("[{}]", stream.str());
			//*/
		}
	};

	template <>
	struct LEX::ObjectInfo<Array_> : public QualifiedObjectInfo<Array_>
	{
		using Typer = int;

		bool IsPooled(ObjectData&) override
		{
			return true;
		}

		TypeOffset GetTypeOffset(ObjectData&) override
		{
			return 0;
		}
	};


	template <>
	decltype(auto) ToObject<std::vector<int>>(std::vector<int>& obj)
	{
		logger::info("hitting");
		Array_ result{};

		//const std::vector<void*> test;

		//void* other = test[1];


		std::transform(obj.begin(), obj.end(), std::back_inserter(result.stuff), [&](auto it) {return it; });

		result.size = obj.size();
		return result;
	}


	//Possible concept for logger implementation
	


	
	INITIALIZE()
	{
		RegisterObjectType<Array_>("ARRAY");
		
		return;
		if constexpr (0)
		{
			Object _array1 = GetObjectPolicy<Array_>()->CreateObject(0);
			Object _array2 = GetObjectPolicy<Array_>()->CreateObject(0);

			_array1 = _array2;
		}
		if constexpr (0)
		{
			Object _array1 = GetObjectPolicy<Array_>()->CreateObject(0);
			Object _array2 = GetObjectPolicy<Array_>()->CreateObject(0);

			_array1 = _array2;
		}


		//return;
		// To walk through functionality, how would this work ?
		std::vector<int> vars{ 1, 2, 3, 4, 5, 6, 7 };

		//auto* var_ptr = &vars;

		//Object test_array = MakeObject(var_ptr);


		Object _array = MakeObject(vars);
		bool policy = _array.policy;
		//_array.data.fastValue
		//logger::critical("Object check: {} , {} (+info {})", reinterpret_cast<Array_*>(_array.data.ptrVal)->size, policy, policy ? !!_array.policy->base : false);
		logger::critical("Object check: {} , {} (+info {})", _array.get<Array_>().size, policy, policy ? !!_array.policy->base : false);


		std::string str;

		//Object str_obj = MakeObject(str);





		//for (auto& variable : reinterpret_cast<Array_*>(_array.data.ptrVal)->stuff)
		//for (auto& variable : _array.get<Array_>().stuff)
		//{
		//	logger::critical("var: {}", variable.AsNumber());
		//}

		logger::info("~Testing PrintString for Arrays: \"{}\"", _array.PrintString());



		//Array_ new_array = *reinterpret_cast<Array_*>(_array.data.ptrVal);
		Array_ new_array = _array.get<Array_>();

		for (auto& variable : new_array.stuff)
		{
			logger::critical(" new var: {}", variable.AsNumber());
		}


		//logger::critical("GenericBuilt check: {} ", reinterpret_cast<Array_*>(GenericDataBuilder<Array_>().ptrVal)->size);
		logger::critical("GenericBuilt check: {} ", GenericDataBuilder<Array_>().get<Array_>().size);

		//return;
		//static_assert(false, "The object pooling system isn't doing so hot. Confirm that it's not stupid busted later.");

		Object first = MakeObject(vars);

		logger::info("index {}", first._data.idxVal);
		Object second = first;

		first = second;

		second.Destroy();


		first = second;
		

		logger::info("end of test");
	};


	//The idea of these structs are to present as tags to be used when having a centralized object class.
	struct use_unvariable {};
	struct use_variable_type {};
	struct use_obj_translator {};




	namespace detail
	{
		template<typename T, size_t Size>requires(sizeof(size_t) >= Size)
		struct alignas(bool) compact;


		template <typename Prefered, typename Levied>
		using prefer_t = std::conditional_t<sizeof(Prefered) == sizeof(Levied), Prefered, Levied>;

		//This helps control what type the given stored integral will be. handling change if it's 1/2/4/8 etc etc.
		//Basically the concept is the choosen storage type will pick an unsigned integer (or signed if they are equal and the prefered is signed) and will use this.
		// But, in the event it's not one of these straight more clear cut types, it will instead use an array for it's allocation.
		template<std::integral T , size_t Size, typename Outlier = std::array<uint8_t, Size>>
		using compact_rep = std::conditional_t<
			Size == 1, prefer_t<T, uint8_t>, std::conditional_t<
			Size == 2, prefer_t<T, uint16_t>, std::conditional_t<
			Size == 4, prefer_t<T, uint32_t>, std::conditional_t<
			Size == 8, prefer_t<T, uint64_t>, Outlier>
			>>>;
		
		template<size_t Size>
		using align_size = compact_rep<uint64_t, Size, bool>;


		//template<size_t Size>
		//using align_size = std::conditional_t<
		//	Size == 2, uint16_t, std::conditional_t<
		//	Size == 4, uint32_t, std::conditional_t<
		//	Size == 8, uint64_t, bool>>>;

		//template<typename T>
		//struct compact<T, sizeof(T)>;
	}

	
	namespace Compact
	{

		template<typename T, size_t Size>requires(sizeof(size_t) >= Size)
			struct alignas(detail::align_size<Size>) compact
		{
			std::array<uint8_t, Size> data;
		};

		compact<uint64_t, 5> test_compact;

		using UsingType = size_t;

		//To get the bits that shouldn't be accessed, get the maximum value of T, 
		// and bit shift it by the amount of bytes allowed to be had * 8 and that should do it. If an integer, simply the maximum value.

		//This is how you get the bits that shouldn't be active. If any process produces these, then that's an error.
		constexpr auto other = (2);
		constexpr UsingType cant_bits = max_value<UsingType> << (other * 8);

	}

	namespace ObjectWorksheet
	{


		class ClassStruct
		{
		
			AbstractTypePolicy* type = nullptr;

		private:
			union
			{
				uintptr_t			_raw = 0;
				Variable*			memberList;
				RuntimeVariable*	runtimeList;
			};
			///I might use some extra flags for this, allowing it to easy denote things like having a bind class, or having a state at a later point.
			size_t size = 0;

			//I'm thinking this is how I'm going to handle this. A union that helps contro it being a variable pointer and a runtime pointer. I can then 
			// switch what type it's percieved as.

			//This might make it a pain however.



			
		};
		

	}
	
	template<typename T, typename = void>
	struct any_value;

	template <typename T>
	struct any_value<T>
	{
		T& _value;

		any_value(const T& t) : _value{ t } {}
		any_value(T&& t) : _value{ t } {}
	};

	template <typename T>
	struct any_value<const T>
	{
		const T& _value;

		any_value(const T& t) : _value{ t } {}
		any_value(T&& t) : _value{ t } {}

	};

	void function_nil(any_value<std::string> t)
	{

	}

	void other_func()
	{
		std::unordered_map<std::string, int> t;

		auto it = t.begin();
		auto str = it->first;
		ReportManager;

		//function("nothing");
	}




	namespace SingletonWorksheet
	{
		template <typename T>
		struct Singleton
		{


			T* operator*()
			{
				//If the return of GetSingleton is a pointer give pointer.
				// if the result is a reference, turn reference into a pointer.

				decltype(auto) singleton = T::GetSingleton();

				using _Res = decltype(singleton);

				if constexpr (std::is_reference_v<_Res>)
				{
					return &singleton;
				}
				else// if constexpr (std::is_pointer_v<_Res>)//I'd like it if it doesn't return either you can't use this. Concept for that probs.
				{
					return singleton;
				}

			}

			const T& operator*() const
			{

			}
		};

		template <typename T>
		Singleton<T> singleton = Singleton<T>{};
	}


	//TODO: Move to RGL
	template <class T> struct extract_class { using type = T; };
	template <class T> struct extract_class<T&> { using type = T; };
	template <class T> struct extract_class<T const&> { using type = T; };
	template <class T> struct extract_class<T&&> { using type = T; };
	template <class T> struct extract_class<T*> { using type = T; };
	template <class T> struct extract_class<T const*> { using type = T; };

	static_assert(std::is_same_v<int const, const int>);

	
	template<typename T>
	struct failure;

	constexpr bool is_thing = std::is_polymorphic_v < std::function<void()>>;
	
	
	


}
struct A {};
struct B : public A {};

void test(B&) {}
void test(const A) {}

void foo()
{
	A a;

	test(a);
}




namespace LEX
{
	int& function(int t)
	{
		std::vector<int> m{ 1, 23, 4, 5, 6 };
		return m[1];

		//static int v = 5;
		//return v;
		//So when using a function like this as a procedure, it will check the address of the returned against the addresses of all the 
		// entries in the tuple, if it matches no entries in the tuple, and/or falls within a range
		
		
		//Some notice should be given, I think I'll largely make it ill advised to use such a thing.
		
		return t;

		
	}

	int idiot1;

	struct TEST_REF
	{
		static local_ref<int> TestTing(scoped_ref<int> do_it)
		{
			return do_it;
		}




		template <typename T>
		static void* PullAddress(T& value)
		{
			if constexpr (std::derived_from<T, detail::wrapper_settings>)
			{
				//maybe I can make a function that does this bit manually
				return value.reference();
			}
			else
			{
				return std::addressof(value);
			}
		}

		template <class TParam, class TResult, typename TArgs, size_t I = 0>
		static void ValueExport(RuntimeVariable& out, TResult& result, TArgs& tuple, Variable* target, std::vector<Variable*>& args)
		{
			/*

			Export proceedings
			Preparing: We'll need the original parameters of the given function to know what we should really do, so template will be a tuple of the raw types,
			 he return type or NI for void, and the argument types for the tuple to be loaded. And of course the loading index.
			The actual parameters are the ReturnType, should be a reference maybe?, the RuntimeVariable for return (so no move has to take place and better for NI too),
			 the args given, and the VariableArray. Note, maybe that should be using a ref_wrapper instead.


			For each step, there's a simple question, is this a referencable type? First we check it to be one of the types plainly (this is genuinely more important),
			 then we check if it's a reference. If either is true, we dereference the variable and stick it into that. If not no activation is handled here.

			Second application. If the return runvar is empty and if the proposed pointer (this will need a dedicated function too) from the return value (if it has one,
			 if it doesn't it will be null) is equal to the current step, put the coresponding variable pointer into a closed RuntimeVariable.
			 This bit too requires the queries in the first actually.

			*Actually I just realized it's also going to care about what the return type is too. I'll be including that in the tuple as well.
			 Third application, go next.

			When we finally reach the end, what should we do?

			Probably handle raw returns. There are a few situations to account for, but at the very least, we know that it isn't ever going to be one of the parameters,
			 which means it must have come from the outside.

			So if the param return type is a reference (same rules as before) and is currently holding a reference, it creates an extern ref,
			 but not before making a check to make sure it follows the rules of engagement.

			Local must not be before this stack, and if Global must not be from any stack that the runtimes hold.

			If either not a pointer type or not holding a reference, we send it back with as a regular filled variable.

			Of course, if the return type is not_implemented, none of this will occur. This being the literal return type rather than the stored one,
			 but either could work.

			//*/

			using RetElem = detail::simplify_wrapper_t<TResult>;

			constexpr auto k_ret_type = detail::reference_type_v<std::tuple_element_t<0, TParam>, true>;

			constexpr auto k_ignore_ret = std::is_same_v<detail::not_implemented, TResult>;

			constexpr auto _I = I;//For visibility in debugging


			if constexpr (I < std::tuple_size_v<TArgs>) {
				using Elem = std::tuple_element_t<I + 1, TParam>;
				using ElemArg = std::tuple_element_t<I, TArgs>;

				constexpr auto k_param_type = detail::reference_type_v<Elem, false>;


				//For each step, there's a simple question, is this a referencable type? First we check it to be one of the types plainly 
				// (this is genuinely more important), 
				// then we check if it's a reference. If either is true, we dereference the variable and stick it into that. If not no activation is handled here.


				if constexpr (!std::is_same_v<StaticTargetTag, Elem>)//This should use args
				{
					if constexpr (k_param_type != detail::ref_type::kNoRef)
					{
						Variable* arg;
						
						if constexpr (I)
							arg = args[I - 1];
						else
							arg = target;
						
						if (arg)
						{
							auto& entry = std::get<I>(tuple);

							//Needs to check for const before doing this.
							//TODO: Make this another move, I don't need what we got here.
							arg->Assign(static_cast<detail::simplify_wrapper_t<ElemArg>&>(entry));

							auto* res = PullAddress(result);
							auto* en = PullAddress(entry);

							if constexpr (!k_ignore_ret && k_ret_type != detail::ref_type::kNoRef)
							{
								if (out.IsEmpty() && PullAddress(result) == PullAddress(entry))
								{
									out = RuntimeVariable::CreateTempRef(arg);
								}
							}
						}
						
					}
				}

				ValueExport<TParam, TResult, TArgs, I + 1>(out, result, tuple, target, args);
			}
			else if constexpr (!k_ignore_ret && I == std::tuple_size_v<TArgs>)
			{
				if (out.IsEmpty() == true)
				{
					if constexpr (k_ret_type != detail::ref_type::kNoRef)
					{
						bool local_check = 1;
						bool global_check = 1;

						void* ptr = PullAddress(result);

						//Do checks right here.

						if (ptr) {//This check first accounts for maybe ref btw, important.
							out = extern_ref(*reinterpret_cast<detail::simplify_wrapper_t<TResult>*>(ptr));
							return;
						}

						//This doesn't work like this, I know this. However
						//


						//out = RuntimeVariable::CreateTempRef(*arg);
					}

					if constexpr (k_ret_type == detail::ref_type::kNoRef || k_ret_type == detail::ref_type::kMaybeRef)
					{
						static_assert(k_ret_type != detail::ref_type::kLocalRef);
						out = Variable{ static_cast<RetElem&>(result), GetVariableType<RetElem>() };
					}
				}
			}
		}

		//This is probably what I should use here. A laundered reference. The point of this is to inherit the expectations of the thing that came
		// previous.

		//Largely the idea is that we use this version, and everything else uses the public version.
		
		inline static int testThing = 42;

		static local_ref<int> TestRefDispatch(StaticTargetTag, local_ref<int>& a2, int& a3)
		{
			
			//In hindsight, I'm actually unsure how this worked given the way it does it's business.
			a3 = a2;

			a2 = 47;
			return a2;
			return testThing;
		}

		template<typename T, typename R, typename... Args>
		static decltype(auto) HandleDispatch(R(*func)(Args...), T& tuple)
		{
			if constexpr (std::is_same_v<R, void>) {
				std::apply(func, tuple);
				return detail::not_implemented{};
			}
			else {
				return std::apply(func, tuple);
			}
		}

		template<typename T, typename R, typename... Args>
		static decltype(auto) LaunderDispatch(R(*func)(Args...), T& tuple)
		{
			return HandleDispatch(func, tuple);
		}

		static void PrintArgs(std::vector<RuntimeVariable>& args)
		{
			logger::info("Parameters of Dispatch");
			for (int i = 0; i < args.size(); i++)
			{
				logger::info("|    a{} => {}", i + 2, args[i]->PrintString());
			}
		}

		static void PsuedoDispatch()
		{
			constexpr auto is_const = std::is_const_v<const int*>;

			RuntimeVariable ref_var = 2;

			RuntimeVariable out;
			std::vector<RuntimeVariable> back_args;//This would actually be 
			back_args.emplace_back(ref_var.AsRef());
			back_args.emplace_back(3);
			PrintArgs(back_args);
			std::vector<Variable*> front_args;

			front_args.push_back(back_args[0].Ptr());
			front_args.push_back(back_args[1].Ptr());

			using Type = detail::try_wrap_param_t<const local_ref<int>&>;
			using TheLook = std::tuple<local_ref<int>, StaticTargetTag, local_ref<int>&, int&>;
			using TheCook = std::tuple<StaticTargetTag, detail::try_wrap_param_t<const local_ref<int>&>, int>;
			
			detail::try_wrap_param_t<const local_ref<int>&> start = Unvariable<int>{}(front_args[0]);

			TheCook tuple{ StaticTargetTag{}, Unvariable<int>{}(front_args[0]), Unvariable<int>{}(front_args[1]) };
			decltype(auto) result = LaunderDispatch(TestRefDispatch, tuple);
			ValueExport<TheLook>(out, result, tuple, nullptr, front_args);
			
			//This is a resolving function, it should be done whenever a procedure is completed, to make sure that the return type is proper.
			if (out.IsRefNegated())
			{
				logger::debug("triggered");
				void* ptr = out.Ptr();

				for (auto& rvar : back_args)
				{
					if (rvar.Ptr() == ptr)
					{
						if (rvar.IsReference() == false) {
							report::fault::critical("Non-reference argument referenced in return.");
						}

						out = rvar.AsRef();

						break;
					}
				}

			}

			//Here, we attempt to update
			for (auto &rvar : back_args){
				rvar.TryUpdateRef();
			}

			PrintArgs(back_args);

			if (!out.IsEmpty())
				logger::info("Result => {} (ptr: {:X})(tar: {})", out->PrintString(), (uintptr_t)out.Ptr(), testThing);

			out->Assign(69);
			//out.TryUpdate(true);

			if (!out.IsEmpty())
				logger::info("Result => {} (ptr: {:X})(tar: {})", out->PrintString(), (uintptr_t)out.Ptr(), testThing);

			testThing = 420;

			if (!out.IsEmpty())
				logger::info("Result => {} (ptr: {:X})(tar: {})", out->PrintString(), (uintptr_t)out.Ptr(), testThing);
		}


		
		//So, function calls will actually forward everything it can, and then use that in order to return something
		//I think the soft calls will be unable to return references (due to the function signature not being the same as the native types,
		// regular invoke (the fancy one?) doesn't know what the rules of the function are.

		//However formulas are able to handle it properly



	};
	

	


	//Need a copy of this called value export.
	template <class T1, size_t I = 1>
	static inline void ValueImport(T1& tuple, std::vector<Variable*>& args)
	{
		if constexpr (I < std::tuple_size_v<T1>) {
			using _Elem = std::tuple_element_t<I, T1>;

			std::get<I>(tuple) = Unvariable<_Elem>{}(args[I - 1]);

			//if constexpr (std::is_same_v<EntryType>) {
			//The actual version will test for the parameters.
			//}

			ValueImport<T1, I + 1>(tuple, args);
		}
	}
	
	/*
	void Dispatch(RuntimeVariable& result, Variable* target, std::vector<Variable*>& args, ProcedureData& data)
	{
		//Unload that shit.
		//using Arg1 = std::tuple_element_t<0, std::tuple<Args...>>;

		constexpr size_t arg_size = std::tuple_size_v<std::tuple<Args...>>;

		if (auto list_size = args.size(); list_size != arg_size) {
			//Shit isn't the same fucking size I'm losing my mind.
			report::apply::error("Dispatch args and expected args do not match.");
		}

		//typename function_traits<std::remove_pointer_t<decltype(T)>>::arguments args;

		//I'll want to remove the references due to being unable to load them like this.
		std::tuple<T, std::remove_reference_t<Args>...> tuple;
		//static_assert(std::is_same_v<std::tuple<Target, int, int, int>,
		//	function_traits<std::remove_pointer_t<decltype(T)>>::arguments>, "GN  NE NIP");

		if constexpr (!std::is_same_v<T, StaticTargetTag>)
		{
			std::get<0>(tuple) = Unvariable<T>{}(target);
		}

		ValueImport(tuple, args);
		//Here we get the return type and switch up on it.

		//Confirm if the types are correct.

		if constexpr (std::is_same_v<R, void>) {
			std::apply(_callback, tuple);
		}
		else {
			R to_result = std::apply(_callback, tuple);

			//under more normal situations, vary this by whether it's a ref or not.

			result = Variable{ to_result, GetVariableType<R>() };
		}
	}
	//*/


	void TestRun()
	{
		
		do
		{
			logger::info("\nType out a formula you'd like to launch. Type ~exit or qqq to leave.");

			std::string path;

			
			std::getline(std::cin, path);


			if (path == "~exit" || path == "qqq") {
				break;
			}
			
			auto result = Formula<Variable>::RunDefault(path);



			logger::info("result >> {}", result.PrintString());

		} while (true);

		return;
		//

		bool exit = false;

		IElement* element = nullptr;

		do
		{
			std::string path;

			logger::info("\nEnter the path of an element. \nUse glob/type/func to specify which type.\nUse @ to use the previous element. \nUse ~elem to view last element. \nUse ~exit to leave.");

			std::cin >> path;

			

			if (path == "~exit"){
				break;
			}

			if (path == "~elem") {
				if (element)
					logger::info("Current element is {}.", element->GetFullName());
				else
					logger::info("No element currently loaded.");

				continue;
			}


			ElementType  elem_type;

			if (std::strncmp(path.c_str(), "type", 4) == 0)
			{
				elem_type = kTypeElement;
			}
			else if (std::strncmp(path.c_str(), "func", 4) == 0)
			{
				elem_type = kFuncElement;
			}
			else if (std::strncmp(path.c_str(), "glob", 4) == 0)
			{
				elem_type = kGlobElement;
			}
			else
			{
				elem_type = kNoneElement;
			}

			IElement* result;

			if (path.contains("@::") == true)
			{
				if (!element) {
					logger::error("No element currently loaded.");
					continue;
				}

				path = path.substr(3);


				result = element->GetElementFromPath(path, elem_type);


				path = std::format("{}::{}", element->GetFullName(), path);
			}
			else
			{
				result = ProjectManager::instance->GetElementFromPath(path, kTypeElement);
			}
			

			logger::info("{} -> {}", path, !!result);

			if (result) {
				logger::info("Full name: {}", result->GetFullName());
				element = result;
			}


		} while (true);
	}

}




namespace fmt
{
	//Really want this to work

	template <std::convertible_to<std::string> T>
	struct formatter
	{

		template <class ParseContext>
		constexpr auto parse(ParseContext& a_ctx)
		{
			return a_ctx.begin();
		}

		template <class FormatContext>
		auto format(const T& v, FormatContext& a_ctx) const
		{
			return static_cast<std::string>(v);
			//auto* info = RE::ActorValueList::GetSingleton()->GetActorValue(a_actorValue);
			//return fmt::format_to(a_ctx.out(), "{}", info ? info->enumName : "None");
		}
	};

}
