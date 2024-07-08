#pragma once



//#include "Object.h"
//#include "ObjectID.h"
//#include "ObjectIDMapper.h"

#include "Lexicon/Engine/Operand.h"


#include "Lexicon/Interfaces/Script.h"
#include "Lexicon/Interfaces/Project.h"
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
#include "Runtime.h"
#include "RuntimeVariable.h"
#include "IVariable.h"
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
#include "RuntimeVariable.h"
#include "MemberPointer.h"
#include "Lexicon/Engine/RoutineCompiler.h"



#include "TypeID.h"


#include "Lexicon/Engine/ConcretePolicy.h"


#include "Lexicon/VariableType.h"

#include "Lexicon/Engine/ConcreteFunction.h"

#include "Lexicon/Engine/OverloadClause.h"




#include "Object.h"
#include "ObjectData.h"
#include "ObjectInfo.h"
#include "ObjectPolicy.h"
#include "ObjectPolicyHandle.h"
#include "Lexicon/Interfaces/ObjectPolicyManager.h"


#include "ObjectPoolData.h"





#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/InterfaceManager.h"
#include "Lexicon/Interfaces/InterfaceSingleton.h"
#include "Versioning.h"

#include "DeclareSpecifier.h"

#include "Lexicon/Engine/OverloadInput.h"

#include "Lexicon/Engine/Declaration.h"

#include "Lexicon/Interfaces/ProcedureHandler.h"
#include "Dispatcher.h"

#include "String.h"

#include "Lexicon/Engine/Signature.h"

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
	struct SourceObject
	{
		//This is an object that is used to represent foreign C++ objects attempting to interface with
		// the lexicon system

		void* object = nullptr;
		std::type_info* info = nullptr;


		//Might these need to decay?
		template <typename T>
		SourceObject(T& self) : object{ &self }, info{ &typeid(T) }
		{
		}

		template <typename T>
		SourceObject(T&& self) : info{ &typeid(T) }
		{
		}
	};


	//I'll need to be able to tell a type by value (this is basically the vtable like side of things)
	// and the return type which is the type you can see.

	//So for example, nothing will ever be a MagicItem, literally does not exist so you'll get its variable type. BUT, you can get it's return type




	struct CONCEPT__TypeID
	{
		//will be redesigning the new range based type id system that should allow for much less wasted space.

		//Type ids will not be the raw id anymore
	};

	struct CONCEPT__TypeCode
	{
		//Type code will continue to exist, this is what is sent along with a name in order to actually find a proper type
		// So that a proper object can be created.
		//This is 16 bit when it comes to that. But hold off on.
	};


	using __TypeID = uint32_t;


	/////////////////////////
	//Implementations
	////////////////////////



	struct CoreOffset
	{
		//CORE isn't a thing yet, but when it is this is what it'd be using.
		constexpr static TypeOffset Void = 0;
		constexpr static TypeOffset Number = 0;
		constexpr static TypeOffset String = 0;
		constexpr static TypeOffset Array = 0;
		constexpr static TypeOffset Function = 0;//FunctionHandle
		constexpr static TypeOffset Object = 0;//ExternalHandle
		constexpr static TypeOffset Delegate = 0;
	};

#define NUMBER_SET_NAME "NUMBER"
#define STRING_SET_NAME "STRING"
#define ARRAY_SET_NAME "ARRAY"
#define FUNCTION_SET_NAME "FUNCTION"
#define OBJECT_SET_NAME "OBJECT"
#define DELEGATE_SET_NAME "DELEGATE"

	//TODO: This should be moved.

	AbstractTypePolicy* VariableType<Void>::operator()(const Void*)
	{
		return nullptr;
	}



	AbstractTypePolicy* VariableType<Number>::operator()(const Number* it)
	{
		ITypePolicy* policy = IdentityManager::instance->GetTypeByOffset(NUMBER_SET_NAME, !it ? 0 : it->GetOffset());

		//Should already be specialized, so just sending it.
		return policy->FetchTypePolicy(nullptr);
	}


	AbstractTypePolicy* VariableType<String>::operator()(const String*)
	{
		ITypePolicy* policy = IdentityManager::instance->GetTypeByOffset(STRING_SET_NAME, CoreOffset::String);

		//Should already be specialized, so just sending it.
		return policy->FetchTypePolicy(nullptr);
	}


	AbstractTypePolicy* VariableType<Delegate>::operator()(const Delegate*)
	{
		return nullptr;
	}


	AbstractTypePolicy* VariableType<Object>::operator()(const Object* it)
	{
		//Currently there is no type policy, but at a later point yes.
		//it->GetType()

		return nullptr;
	}

	AbstractTypePolicy* VariableType<FunctionHandle>::operator()(const FunctionHandle*)
	{
		return nullptr;
	}


	AbstractTypePolicy* VariableType<Array>::operator()(const Array*)
	{
		return nullptr;
	}

	AbstractTypePolicy* VariableType<Variable>::operator()(const Variable* it)
	{
		if (it)
		{
			return it->Policy();
		}

		return nullptr;
	}

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





	//This is so temporary I hate that I'm doing it like this.
	AbstractTypePolicy* Variable::_CheckVariableType()
	{
		AbstractTypePolicy* result = std::visit([&](auto&& lhs) {
			return FetchVariableType(lhs);
			}, _value);

		return result;
	}

	



	void test()
	{


	}




	class IFormula : public ICallableUnit
	{
		//IFormula simply should provide some clarification that this indeed is a formula

	};
	
	template <typename T>
	struct BasicFormula : public IFormula
	{
		//The idea of this is you cast an IFormula into this forcibly, and this type will then manage all of the function calls for the type.
		// Basically, think of this as a std::function. it should then translate all the rest of the bullshit around it.
	};


	class Formula : public IFormula, public BasicCallableData
	{
		//This version is obscured for the user. It should help inline functions and such into the code, or that can be given to others to run.
		//Formula rules. 
		// They don't have default parameters, 
		// they don't have procedures. 
		// They don't have names.
		// They don't have targets



		RuntimeVariable Invoke(std::vector<RuntimeVariable>& args, RuntimeVariable* def)
		{
			//TODO: Once arrays and the params keyword gets introduced, this will need to be implemented in other ways. Further more, could just bake this into the call.

			Runtime* runtime = nullptr;
			Variable* target = nullptr;

			if (args.size() != parameters.size())
				report::apply::critical("Arg size not compatible with param size ({}/{})", args.size(), parameters.size());

			size_t size = parameters.size();

			
			for (int i = 0; i < size; i++) {
				//Cancelling this for now.
				break;
				int j = i;

				AbstractTypePolicy* expected = parameters[i].GetType()->FetchTypePolicy(runtime);
				if (!expected)
					throw nullptr;
				RuntimeVariable check = args[j]->Convert(expected);

				if (check.IsVoid() == true)
					report::apply::critical("cannot convert argument into parameter {}, {} vs {}", parameters[i].GetFieldName(), i, j);

				args[i] = check;
			}
			
			{
				RuntimeVariable result;

				Runtime runtime{ *GetRoutine(), args };

				result = runtime.Run();

				return result;
			}
		}


	};


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




			//bool Invoke(Variable& out, std::vector<RuntimeVariable> args)

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
	}
	
	

	namespace NumberNew
	{



		INITIALIZE()
		{
			
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


			report::debug("inf test {}", test_inf.ToString());

			unsigned_int8.Assign(testB);
			report::debug("uint8 {}", unsigned_int8.ToString());

			report::debug("{} + {} = {} ?", testA.ToString(), testB.ToString(), testResult.ToString());
			constexpr auto fill = std::numeric_limits<int8_t>::max();

			unsigned_int8.Visit([](auto& it) { it += 1; });

			report::debug("uint8 later {}", unsigned_int8.ToString());
		}


	}


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
	struct ThisHelper<T, std::enable_if_t<PointerType<T>>> : private detail::ThisHelperImpl
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











	struct TypeA1 {};
	struct TypeA2 :public TypeA1 {};

	struct TypeB1 {};
	struct TypeB2 : public TypeB1 {};

	struct TypeA : public TypeA2 {};
	struct TypeB : public TypeA, public TypeB2 {};
	struct TypeC : public TypeB {};
	//TypeC hash to TypeB2
	//i:0, 1
	//TypeC hash to TypeB1
	//i:0, 1, 0
	//TypeC hash to TypeA1
	//i:0, 0, 0, 0

	//So here's a thing of note, as a hash 0 hash to be non-modifiying.
	
	namespace NewSpecials
	{

		//Things and structures I need to replicate to continue vs ones I do not need to relicate.
		struct BasicGeneric;


		//Dont
		class Specialization
		{
			//virtual ~Specialization() {}


			BasicGeneric* _parent = nullptr;
		};

		//Dont
		//These are both bases to a template class that handles it. I think.
		class ResolvedVariant : public Specialization
		{
			//this is abstract


			std::vector<AbstractTypePolicy*> _types;
		};

		//Dont
		class UnresolvedVariant : public Specialization
		{
			//this is interface

			//I dunno what to call this, but basically it's an "AdjustPointer" to remove the need
			// of a dynamic cast. Unsure if I'll do this though, probably will not. A dynamic cast is not expensive
			// after all.
			void* AsSpecial() { return nullptr; }

			std::vector<ITypePolicy*> _types;


		};


		//Dont
		struct BasicGeneric
		{

		};

		//Dont
		template <typename T1, typename T2>
		struct Generic : public BasicGeneric, public virtual T1
		{


			using Interface = T1;		//The interface is the type used to symbolize the idea of being the type.
			using Abstract = T2;		//The abstraction is the type used to say something is ready to be used as the type.

			struct ResolveInterface : public Interface, public ResolvedVariant
			{

			};

			//The existence of this is not so that the interface can become the abstract, rather it's so this
			// has a way of communicating with it's later version on how to do that very thing.
			virtual Abstract* ResolveVariant(Interface*, IGenericArgument*) = 0;

			//So the idea I think I'll have is instead you'll be forced to make a resolve interface and resolve

			//size_t _expectedArgs = 0;					//args
			//std::vector<GenericParamPolicy> _params;	//params
			//std::vector<UnresolvedVariant*> _clients;	//partial
			//std::vector<ResolvedVariant*> _servers;		//complete
		};


		//Do
		struct Interface
		{
			virtual ~Interface() = default;
		};

		//Do
		struct Abstract : public virtual Interface
		{

		};



		//Do
		//Advisable base structure that should be the pivot between generic and concrete classes.
		struct Base : public virtual Interface//, FunctionData, ICallableUnit
		{

		};


		//Dont
		//This is what a generic class would look like.
		struct GenericFoo : public Base, public Generic<Interface, Abstract>
		{
			//This is generic
			//void a() override {}
			//int c;

			Abstract* ResolveVariant(Interface* tar, IGenericArgument*)
			{
				return nullptr;
			}
		};

		//Do
		struct ConcreteFoo : public Base, public Abstract
		{
			//This is concrete.
			//int d;
		};
		//The above structure seems to work, most importantly, it doesn't take undue size for the concrete type.
		// At least, not much.


		int main()
		{
			ConcreteFoo objA{};
			GenericFoo objB{};

			Abstract* test1 = &objA;
			Interface* test2 = &objB;

			//objA.a();
			//objB.a();
			std::cout << "Hello World << " << sizeof(ConcreteFoo);

			return 0;
		}

	}

	struct T
	{

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




	
	INITIALIZE()
	{
		RegisterObjectType<Array_>("ARRAY");
		
		return;
		{
			Object _array1 = GetObjectPolicy<Array_>()->CreateObject(0);
			Object _array2 = GetObjectPolicy<Array_>()->CreateObject(0);

			_array1 = _array2;
		}
		{
			Object _array1 = GetObjectPolicy<Array_>()->CreateObject(0);
			Object _array2 = GetObjectPolicy<Array_>()->CreateObject(0);

			_array1 = _array2;
		}


		return;
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
		for (auto& variable : _array.get<Array_>().stuff)
		{
			logger::critical("var: {}", variable.AsNumber());
		}



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

		logger::info("index {}", first.data.idxVal);
		Object second = first;

		first = second;

		second.Destroy();


		first = second;
		logger::info("end of test");
	};



#define EXTEND(mc_object) 1

template <typename T>
struct Extension : public T
{
	//the idea is this is a struct that either takes a reference derives from it with no vtable and attempts to perform some new function on it.

	//The only problem is making additional increases to the extension methods.
};


#define TO_XVALUE(mc_expr) true ? mc_expr : static_cast<decltype(mc_expr)&&>(*(int*)0)

	namespace Fields
	{




		template <typename T>
		struct Derived : public T, public Void
		{
			//How do we handle T?
			//First


			void Foo(int i)
			{
				//We cannot do this, because we don't know from these unresolved types if these are compatible.
				i = *this;
			}


			//What we learned.
			//-First off, that the inheritence list will be completely different between concrete and generic types.
			//  From a generic standpoint, it inherits from all the things it knows it can inherit. More than likely,
			//  it will inherit from these FIRST, regardless order. The main reason why is if I try to unpack them in
			//  the order given, what if something that we inherit from later is inherited from sooner?
			// -Such as classC derives from Interface, so it's member offset is like 4, but our generic type 
			//  also derives from it, making it a lot sooner, making the member offset inaccurate between specializations.
			//  This is still problematic however. Mainly because of member access to non-existent parts.
			// <!>Ah, I know what will resolve this. if the generic has typed constraints that allow it to derive from concrete types, it will use those concrete types
			//  within the generic class. Note, this will also result in those being sooner than thought.

			//The question operating with this is how to handle the adjustments when calling functions that expect something different?


			//Inherit list
			//Concrete:
			// Void

			//Specialization
			//T...

			//Total


		};


	}



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

	void function(any_value<std::string> t)
	{

	}

	void other_func()
	{
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

	namespace
	{
		struct TestInterface : public Interface, InterfaceSingleton<TestInterface>
		{
			uintptr_t Version() const override
			{
				return 0;
			}

			virtual void Printshit()
			{
				logger::critical("I'm printing stuff");
			}
		};

		INITIALIZE()
		{
			TestInterface* singleton = nullptr;
			if (auto result = InterfaceManager::RequestInterface(singleton, 0); result != Update::Match) {
				logger::info("failure {}", (int)result); return;
			}

			singleton->Printshit();


			TestInterface::instance->Printshit();



			ObjectPolicyManager* singleton2 = nullptr;
			if (auto result = InterfaceManager::RequestInterface(singleton2, 0); result != Update::Match) {
				logger::info("failure 2 {}", (int)result); return;
			}
			logger::info("obj_policy ver {}", singleton2->Version());

		};
	}
	

	//TODO: Move to RGL
	template <class T> struct extract_class { using type = T; };
	template <class T> struct extract_class<T&> { using type = T; };
	template <class T> struct extract_class<T const&> { using type = T; };
	template <class T> struct extract_class<T&&> { using type = T; };
	template <class T> struct extract_class<T*> { using type = T; };
	template <class T> struct extract_class<T const*> { using type = T; };

	static_assert(std::is_same_v<int const, const int>);

	
	enum struct SignatureEnum
	{
		Result,
		Target, 
		Argument,
	};

	template<typename T>
	struct failure;

	using INT = std::remove_const_t<const int&>;
	template <SignatureEnum T, typename E, typename... Next>
	bool ProcessEntry(Signature& sign)
	{
		//In order to handle specific reference types, or specific qualifiers, there will be a wrapper type to handle it.
		// I'll just unpack it when the time comes. This would mostly be used for the target of a function.
		// under these situations, it will use the suggested  qualifiers rather than the the true ones.

		//If an the last entry is the callInfo, ignore it and move on. If it's callinfo but not last, do not compile.


		if constexpr (std::is_same_v<E, Void> && T == SignatureEnum::Argument) {
			//Do not compile actually.
			return false;
		}


		//Need to save this for later.
		//if constexpr (Tar && !std::is_reference_v<E>) {
		//	return false;
		//}

		constexpr auto next_size = sizeof...(Next);

		using _Pointless = std::remove_pointer_t<E>;
		using _Refless = std::remove_const_t<std::remove_reference_t<E>>;
		using _Naked = std::remove_pointer_t<_Pointless>;//Might be better to get the underlying value of the thing.

		QualifiedType& entry = T == SignatureEnum::Result ?
			sign.result : T == SignatureEnum::Target ?
			sign.target : sign.parameters.emplace_back();
			
		logger::info("increase? {}", sign.parameters.size());
		
		entry.policy = FetchVariableType<_Refless>();
		

		if constexpr (std::is_const_v<_Naked>){
			entry.flags |= Qualifier::Const;
		}

		//For right now it really doesn't matter.
		if constexpr (std::is_reference_v<E>) {
			entry.flags |= Qualifier::RefL;
		}



		if constexpr (next_size)
		{
			constexpr SignatureEnum Enum = T == SignatureEnum::Result ?
				SignatureEnum::Target : T == SignatureEnum::Target ?
				SignatureEnum::Argument : T;


			return ProcessEntry<Enum, Next...>(sign);
		}
		else
		{
			return true;
		}
	}
	
	constexpr bool is_thing = std::is_polymorphic_v < std::function<void()>>;
	
	/*
	struct TargetEnvironment_nixed
	{
		enum Flag : uint8_t
		{
			None = 0 << 0,
			Explicit = 1 << 0,
		};


		//Size isn't used to its fullest

		//So issue, this actually used to use a solution, and rightfully so. Target object
		// as itself 





		Environment* target = nullptr;
		TargetObject* prev = nullptr;
		Flag			flag = Flag::Explicit;

		//Purges the use of assigning TargetObjects. This is used when returning or when assigning for implicit
		// constuction knowledge.
		TargetObject* GetCallTarget()
		{
			if (this) {
				return flag & Flag::Assign ? prev->GetCallTarget() : this;
			}

			return nullptr;
		}

		//Reverse of call target, this gets the target of an assignment or return. Useful for ternary statements
		TargetObject* GetAssignTarget()
		{
			if (this) {
				return flag & Flag::Assign ? this : prev->GetAssignTarget();
			}

			return nullptr;
		}

		TargetObject(Solution& t, TargetObject* p, Flag f) : target{ &t }, prev{ p }, flag{ f }
		{

		}

		TargetObject(Solution& t, TargetObject* p = nullptr, bool b = false) : target{ &t }, prev{ p }, flag{ b ? Explicit : None }
		{

		}
	};
	//*/




	struct TestA {
		struct TestB {
			struct TheTest
			{
				static int last;
			};
			TheTest GetTest()
			{
				return {};
			}
		};
	};



	

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



	struct Nameless//This goes in the project manager. Make that shit an interface and give it a source
	{
		enum Enum
		{
			NoneElement,
			TypeElement,
			FuncElement,
			GlobElement,
		};




		inline static std::unordered_map<size_t, std::pair<Element*, Enum>> _lookupMap;



		static Element* LookupElement(size_t hash, Enum type)
		{
			auto it = _lookupMap.find(hash);

			if (_lookupMap.end() == it || it->second.second == type)
				return nullptr;

			return it->second.first;
		}

		static void SaveElement(Element* elem, size_t hash, Enum type)
		{

			_lookupMap[hash] = std::make_pair(elem, type);

		}

		static size_t HashPath(std::string& path)
		{
			//TODO: -Check: I think unordered map already hashes stuff so maybe not needed. Still will handle later
			std::hash<std::remove_reference_t<decltype(path)>> hasher{};

			return hasher(path);

		}

		static Element* GetElementFromPath(std::string path, Enum elem)
		{
			//right now, linking isn't really set up so you know.
			if (1 || Component::HasLinked(LinkFlag::Declaration) == false){
				//Tell that this is too early to be valid.
				return nullptr;
			}


			auto hash = HashPath(path);

			auto element = LookupElement(hash, elem);


			if (!element)
			{
				Record record = LEX::Impl::Parser__::CreateSyntax<PathParser>(path);

				//<!> After parsing is done, the top most type needs to be set to the standards of what it's expecting.

				//From here, use the path functions that are in environment.
				// The project should be found from the first part, and from there we should just keep the environ search should finish it out.

				switch (elem)
				{
					//Do the search for each type here.
				case Enum::FuncElement:
				case Enum::TypeElement:
				case Enum::GlobElement:
				default:
					element = nullptr; break;
				}


				if (element) {
					SaveElement(element, hash, elem);
				}

			}

			return element;
		}

		static FunctionBase* GetFunctionFromPath(std::string path)
		{
			if (auto elem = GetElementFromPath(path, FuncElement); elem)
				return static_cast<FunctionBase*>(elem);

			return nullptr;
		}

		static FunctionBase* GetFunctioFromPath(std::string path)
		{
			if (auto elem = GetElementFromPath(path, FuncElement); elem)
				return static_cast<FunctionBase*>(elem);

			return nullptr;
		}





		static PolicyBase* GetTypeFromPath(std::string path)
		{
			if (auto elem = GetElementFromPath(path, TypeElement); elem)
				return static_cast<PolicyBase*>(elem);

			return nullptr;
		}

		static Global* GetGlobalFromPath(std::string path)
		{
			if (auto elem = GetElementFromPath(path, GlobElement); elem)
				return static_cast<Global*>(elem);

			return nullptr;
		}

	};


	
	template <typename R, typename... Args>
	bool RegisterFunction(R (*func)(Args...), FunctionBase* base)
	{
		//Currently, the only way to properly handle the system is by making a unique lambda each time a new template is created. But needless to say,
		// cant do that.

		//I think something I can do is that optionally a 64 bit context code can be given when making a procedure. This allows me to just simply pull what's needed.
		// So I think that might be a proceedure thing idk. Something to make context of. But something fast. Definitely not something like what native function
		// would have been.

		//The context thing is a good idea, it can possibly sort out which overload is being set up, if someone is using it pure (which one shouldn't.
		// Long story short it's the address to use.

		Signature sign{};

		//bool processed = ProcessEntry<true, R, Args...>(sign);
		bool processed = ProcessEntry<SignatureEnum::Result, R, Args...>(sign);

		if (!processed)
			throw nullptr;
		
		OverloadFlag flag = OverloadFlag::AllAccess;

		auto overload = sign.Match(base, nullptr, nullptr, flag);

		if (flag & OverloadFlag::Failure) {
			logger::info("FAILED TO MATCH");
			return false;
		}
		else
			logger::info("SUCCESS TO MATCH");

		//static auto dispatch = new BasicDispatcher(func, base);
		auto result = BasicDispatcher<R, Args...>::Create(func, base);

		return result;
	}

	//thing to get function here.

	template <typename R, typename... Args>
	bool RegisterFunction(R (*func)(Args...), std::string path)
	{
		FunctionBase* base = Nameless::GetFunctionFromPath(path);

		return RegisterFunction(func, base);
	}

	bool RegisterFunction(Procedure procedure, std::string path)
	{
		FunctionBase* func = Nameless::GetFunctionFromPath(path);

		return ProcedureHandler::instance->RegisterFunction(procedure, func);
	}
}


void Test()
{
	LEX::ConcreteFunction t{};
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
