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

namespace LEX
{
	
	/////////////////////////
	//Implementations
	////////////////////////


	struct APIResult_
	{
		IssueCode code;
		//Should the given issue always be in english? maybe i just store both and let them choose which they want.
		uint64_t messageID;

		[[nodiscard]] std::string Redeem(bool useTranslation)
		{
			//The point of redeem is to get the message as printed (mostly) to the logger. This can be useful if you want to print it to your
			// own logs or something.

			//After redeemed the given message id is killed. Similar things will happen when the API result dies.
			return {};
		}


		constexpr operator bool() const
		{
			return !code;
		}
	};



	
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

	

	namespace NEW
	{
		struct IElement
		{
			virtual void* As(std::string_view name) = 0;

			//Each of these muse derive from the same type that IElement does.
			template<typename T>
			T* As()
			{
				if (!this)
					return nullptr;

				return reinterpret_cast<T*>(As(typeid(T).name()));
			}

			template <typename T>
			operator T& ()
			{
				return *As<T>();
			}

			virtual IProject* GetProject() = 0;
		};
		
		
		struct IElementProxy : public Interface
		{
			virtual IElement* base() = 0;
			virtual const IElement* base() const = 0;

		};




		struct IElementImpl : public IElement
		{
			//This basically allows for some jank ass covariance equivalent so the interface version of these functions are never invited into the main
			// code

			virtual IProject* GetProject()
			{
				return GetProject({});
			}


			virtual Project* GetProject(int = {}) = 0;
		};

		struct FakeElement : public IElementImpl
		{
			Project* proj = nullptr;

			Project* GetProject(int = {}) override
			{
				return proj;
			}
		};
		
	}



	void TestingTheorem()
	{

		Project test;
		//StructC test;

		//test.Foo();

		//Instead of an as function for each, I will just make a single one. This will return a void pointer, and that void pointer will be reinterpreted 
		// to a different class

		//Only question though, how to make sure these 2 types actually mix properly

		//Instead of deriving interface, they will derive from a special class that will point to the next object back.

		//So project becomes an element, script can become an environment, except environment will not exist, due to environment being a part of type
		// and not all ITypes will be real elements.


		//I think I'll just resolve the differences manually.


		//The last addition, the IElement buffers. These basically will take some difficult to use types and sorta launder them for engine use.
		// IElement becomes Element, IProject just project, etc. This is only needed for return types
	}



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



	void TFunc()
	{
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
		
		Formula<void>::Run("otherTest() => otherTest() => otherTest()");
		
		 

		Variable result = Formula<Variable>::Run("(0.0).CouldBeLiterallyAnything()");

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

	















	struct TestingInheritance
	{

		struct TypeA1 {};
		struct TypeA2 :public TypeA1 {};

		struct TypeB1 {};
		struct TypeB2 : public TypeB1 {};

		struct TypeA : public TypeA2 {};
		struct TypeB : public TypeA, public TypeB2 {};
		struct TypeC : public TypeB {};

		int foo(TypeA2&)
		{
			return 1;
		}

		int foo(const TypeA2&)
		{
			return 1;
		}



		int foo(TypeB2&)
		{
			return 1;
		}



		int bar(TypeA1&)
		{
			return 1;
		}


		int bar(TypeB1&, TypeB1&)
		{
			return 1;
		}
		

		int bar(const TypeC&, TypeB1&)
		{
			return 1;
		}

		struct IBase2 {
			static void Foo() {};

			virtual void Bar()
			{

			}
		};

		struct IBase :public virtual IBase2
		{
			void Bar2() const {}
			void Bar() override{}
		};

		struct DiffBase : public virtual IBase2 { static void Foo() {}; };


		struct BaseA : public virtual IBase { void Bar() override {} };
		struct BaseB : public virtual IBase {};

		struct Derived : public BaseA, public BaseB {};
		struct Derived2 :public Derived, public DiffBase { void Bar2() {} };




		int pie(const IBase&, float, int)
		{
			return 1;
		}

		int pie(const IBase2&, int, float)
		{
			return 1;
		}

		//int pie(const Derived&){return 1;}


		void Test()
		{

			TypeA a;
			TypeB b;
			TypeC c;
			foo(a);
			//First: arg1 par 3, arg2 par 3 (or actual 5 if we consider multiple inherit)
			//Second call: arg1 par 7, arg2 par 4

			//foo(c);

			bar(c, c);

			Derived2 d;
			const Derived2 d2;
			d.Bar();
			//d2.Bar2();

			Derived2::Foo();
			//pie(d, 1, 1);
		}
	};









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
		namespace NewString
		{


			INITIALIZE()
			{
				std::string testA = "l;";

				//report::info(testA.ToString());

				std::string testB = std::move(testA);

				//report::info("{} vs {}", testA.ToString(), testB.ToString());
			}

		}



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


#include "Lexicon/Engine/ConcreteGlobal.h"

namespace LEX
{

	//the associated should maybe be a bool or just reject any other than include and import.

	static SyntaxRecord& GetPath(SyntaxRecord& path, bool right)
	{
		SyntaxRecord* ret = path.FindChild(right ? parse_strings::rhs : parse_strings::rhs);

		if (!ret)
			ret = &path.GetFront();

		return *ret;
	}



	static std::vector<Environment*> GetAssociates(Element* a_this, RelateType = {})
	{
		return {};
	}



	static Environment* GetEnvironmentTMP(Environment* a_this, Record* path, bool& search_scripts)
	{
		Environment* result;
		get_switch(path->SYNTAX().type)
		{
		case SyntaxType::Scopename:
		case SyntaxType::Typename:
		{
			result = nullptr;
			//Script name means nothing basically.

			if (result) {
				search_scripts = false;
				break;
			}
		}

		if (switch_value == SyntaxType::Typename)
			break;

		[[fallthrough]];
		case SyntaxType::Scriptname://This should be script name.
			if (search_scripts)
			{
				result = nullptr;
				//Script name means nothing basically.

				break;
			}

		default:
			result = nullptr; break;
		}


		return result;
	}

	static Environment* WalkEnvironmentPath(Environment* a_this, SyntaxRecord*& path, bool search_scripts = true)
	{
		//If record is empty afterwords, this means that it has completed iteration.
		if (path->IsPath() == false) {
			path = nullptr;
			return a_this;
		}
		auto left = path->FindChild(parse_strings::lhs);

		if (!left) {

			auto result = GetEnvironmentTMP(a_this, &path->Demote(), search_scripts);

			path = nullptr;

			return result;
		}

		if constexpr (1 == 1)//If not generic.
		{
			Environment* _this = GetEnvironmentTMP(a_this, &left->Demote(), search_scripts);

			path = path->FindChild(parse_strings::rhs);

			return WalkEnvironmentPath(a_this, path, search_scripts);
		}
		else
		{

		}
	}

	static Environment* SpecializeEnvironments(std::vector<Environment*>& generics)
	{
		//Specialize environment brings everything down to a single point.
		return {};
	}

	static std::vector<Environment*> GetEnvironments(Element* a_this, SyntaxRecord* step, RelateType a, std::set<Element*>& searched)
	{
		/*
		make variable that stores temp environment here.
		overloop here
		for each environment in the temp var:

		Check for environ being in the set.
		Add environ to the set.
		Call function.
		move temp var restart overloop.

		Overloop keeps going until absolutely nothing is created from GetAssociate
		//*/

		//This could be cleaner, but it works for now.

		std::vector<Environment*> result{ };

		std::vector<Environment*> out{ a_this->GetEnvironment() };

		//maybe there's a better way to do this, but whatever innit.
		while (out.size() != 0)//Overloop
		{
			std::vector<Environment*> buffer{};


			for (auto env : out)
			{
				if (!env || !searched.emplace(env).second) {
					continue;
				}
				buffer.emplace_back(env);

				if (a != RelateType::None)
					buffer.insert_range(buffer.end(), GetAssociates(env, a));
			}
			result.insert_range(result.end(), buffer);
			out = std::move(buffer);
		}

		bool specialize = false;

		if (step)
		{
			//out.clear();
			//out.reserve(result.size());

			for (auto& env : result)
			{
				auto _step = step;
				env = WalkEnvironmentPath(env, _step);

				if (_step)
					specialize = true;
			}

			result.erase(std::remove(std::begin(result), std::end(result), nullptr), std::end(result));
		}

		if (specialize) {
			auto new_this = SpecializeEnvironments(result);

			if (new_this)
				return GetEnvironments(new_this, step, a, searched);

			return {};
		}
		else
		{
			return result;
		}
	}


	using ElementSearch = bool(std::vector<Environment*>&);

	bool HandlePath(Element* focus, SyntaxRecord* rec, std::function<ElementSearch>& func, std::set<Element*>& searched)
	{
		if (!focus)
			return false;


		RelateType ship = RelateType::Imported;

		Environment* target = focus->GetEnvironment();

		if (focus != target) {
			target = focus->GetCommons();
		}


		for (; ship != RelateType::None; ship--)
		{


			std::vector<Environment*> query = GetEnvironments(target, rec, ship, searched);


			bool success = func(query);

			if (success)
				return true;
		}

		return false;
	}




	//Move this out, I'd like a way to force project state. What it is would be determined if it's run on an element or not.
		// So that actually means this version of the function stays, to make it less confusing.
		// or is getting moved to the project manager
	enum ProjectState
	{
		kNone,
		kFind,
		kShared,
		kCurrent,
	};

	//I think the reutnr of this function should probably be why it failed if anything.
	static bool SearchPathBase(Element* a_this, SyntaxRecord& rec, std::function<ElementSearch> func)
	{
		//Needs a function to handle these.
		/*
		this element
		up element until none (loop)
		Search from project
		Move onto commons
		move onto shared commons
		search for script in current project
		search for project
		//*/

		//So there has to be something that prevents it from using the main one, notably if someone is incredibly specific about which version they're using.

		//This needs to be proper at some point.
		SyntaxRecord* path = rec.FindChild(parse_strings::path);

		auto left = path ? path->FindChild(parse_strings::lhs) : nullptr;
		auto right = left ? path->FindChild(parse_strings::rhs) : nullptr;

		//This is what should be submitted to functions
		auto next = right ?
			&GetPath(*right, true) : path ?
			&path->GetFront() : &rec;



		Element* target = a_this;

		std::set<Element*> searched{};

		bool is_shared = true;

		ProjectState path_state;


		if (path)
		{
			switch (path->GetFront().GetEnumFromRecord<LEX::Syntax>().type)
			{
			case SyntaxType::ProjectName:
				path_state = kFind;
				break;
			case SyntaxType::Scriptname:
				target = nullptr;
				[[fallthrough]];
			default:
				path_state = kCurrent;
				break;
			}
		}

		ProjectState state = a_this ? path_state : kFind;//Force to be find depending on what heads path.



		//Here's how it works, if there is no this element, it will use find. if there is a this element it will differ based on what
		// path is.


		while (target || state != kNone)
		{
			auto _next = next;

			if (!target)// && left) //Must have a left side
			{
				Project* project = nullptr;

				Record* tar = left ? &left->Demote() : &path->GetFront().Demote();

				switch (state)
				{
				case kCurrent:
					project = a_this->GetProject();
					goto _find;

				case kShared:
					project = ProjectManager::instance->GetShared();
					goto _find;

				case kFind:
					//Find is when project has to be found first, so move the finding of the script down here.
					if (!right)
						break;

					project = ProjectManager::instance->GetProject(tar->GetFront().GetView());

					tar = &GetPath(*right, false).Demote();
					//Revert to this to test for a crash
					//_next->GetView() == parse_strings::rhs || _next->GetView() == parse_strings::lhs
					if (_next->GetSyntax().type != SyntaxType::Path) {
						_next = nullptr;
					}



					if (!tar)
					{
						return false;
						state = kNone;
						//If target is no longer able to be found.
						//We break, nothing to do.
						break;
					}
					//_next = nullptr;//Unsure how to deal with this bit.
				_find:
					//state--;
					if (project) {
						bool is_path = tar->SYNTAX().type == SyntaxType::Path;

						if (auto script = project->FindScript(is_path ? tar->GetFront().GetView() : tar->GetView()); script) {
							target = script;
							break;
						}
					}

				case kNone:
					break;
				}
			}


			bool success = HandlePath(target, _next, func, searched);

			if (success)
				return true;

			if (state == kCurrent)
				target = target->FetchParent();

			if (!target)
				state--;

		}

		//ProjectManager::


		return false;
	}



	static PolicyBase* SearchTypePath(Environment* a_this, Record& _path)
	{
		PolicyBase* result = nullptr;

		SearchPathBase(a_this, _path.Transform<SyntaxRecord>(), [&](std::vector<Environment*>& query) -> bool
			{
				for (auto env : query)
				{

					std::vector<PolicyBase*> types = env->FindTypes(_path.GetView());

					//There's no situation where multiple can be observed, so it only needs the one.

					auto size = types.size();



					//TODO: VERY temporary idea. No pattern matching, no checking. This is basically the same that we did before
					if (size > 1) {
						report::compile::critical("mulitple types of same name detected.");
						throw nullptr;
					}
					else if (size)
					{
						if (!result)
						{
							result = types[0];
						}
						else
						{
							report::error("path no work. Make better error later");
						}

						return true;
					}

				}

				return false;
			});


		return result;
	}


	PolicyBase* SearchTypePath(/*Environment* this,*/ Record& path)
	{
		return nullptr;
	}
	
	PolicyBase* SearchTypePath(/*IElement* this,*/ std::string_view path)
	{
		return nullptr;
	}

	static void PrintAST_(Record& tree, std::string indent = "")
	{
		constexpr std::string_view __dent = "| ";

		std::string log = tree.Print<LEX::Syntax>();

		RGL_LOG(info, "{}{}", indent, log);

		indent += __dent;

		for (auto& child_rec : tree.children())
		{
			PrintAST_(child_rec, indent);
		}
	}

	namespace control
	{
		struct PathParser : public LEX::Impl::ParseModule, public LEX::Impl::IdenDeclBoilerPlate
		{
			//The idea of this is it's a simple parser that acts like the identifier parser, but will handle this in a way that can handle
			bool CanHandle(LEX::Impl::Parser* parser, Record* target, LEX::Impl::ParseFlag flag) const override
			{
				return true;
			}

			bool IsAtomic() const override
			{
				return true;
			}


			Record _HandleThis(LEX::Impl::Parser* parser)
			{
				RecordData next = parser->next();
				next.GetTag() = parse_strings::this_word;
				return LEX::Impl::Parser::CreateExpression(next, SyntaxType::Field);
			}


			Record HandleToken(LEX::Impl::Parser* parser, Record*) override
			{

				return _HandlePath(parser, SyntaxType::ProjectName);

			}
			// Need some boilerplate resolver.
			std::string_view GetModuleName() override
			{
				return typeid(PathParser).name();
			}

			bool ContextAllowed(LEX::Impl::ProcessContext*, LEX::Impl::ProcessChain*) override
			{
				//This prevents anything from following it up for the most part. This shit is a one man show!
				// If this causes any issues with parsers I may use later, feel free to make a variable to help handle when this is allowed be handled.
				return false;
			}
		};


	}


	void TestRun()
	{

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

		return;

		auto path = "Shared::Commons::__float32";

		if constexpr (1)
		{
			IElement* floatType = ProjectManager::instance->GetElementFromPath(path, kTypeElement);
			logger::info("Testing -> {}", !!floatType);

			if (floatType)
			{
				logger::info("Full name: {}", floatType->GetFullName());

				auto testType = floatType->GetElementFromPath("::OtherScript::TestType", kTypeElement);



				logger::info("Testing -> {}", !!testType);

				if (testType)
					logger::info("Full name: {}", testType->GetFullName());

			}



			//std::cin << 


			return;
		}

		Record record = LEX::Impl::Parser__::CreateSyntax<control::PathParser>(path);

		PrintAST_(record);


		auto test = SearchTypePath(nullptr, record);


		logger::info("Testing -> {}", !!test);
		if (test)
		{
			logger::info("Full name: {}", test->GetFullName());


			Record record = LEX::Impl::Parser__::CreateSyntax<Impl::IdentifierParser>("OtherScript::TestType");

			PrintAST_(record);


			auto test2 = SearchTypePath(test, record);


			logger::info("Testing -> {}", !!test2);

			if (test2)
				logger::info("Full name: {}", test2->GetFullName());

		}
	}


	INITIALIZE()
	{
		logger::info("Name {}", SettingManager::GetSingleton()->language);
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

	void Test()
	{
		LEX::Number num{ 1.5f };

		logger::info("{} test", num);
	}
}
