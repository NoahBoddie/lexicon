#pragma once




//#include "Object.h"
//#include "ObjectID.h"
//#include "ObjectIDMapper.h"

#include "Lexicon/Impl/Operand.h"


#include "Lexicon/Impl/Script.h"
#include "Lexicon/Impl/Project.h"
#include "Lexicon/Impl/ProjectManager.h"

#include "Lexicon/Impl/Exception.h"
//#include "Parser.h"
#include "Lexicon/Impl/ParserTest.h"


#include "Compiler.h"

#include "Lexicon/Impl/Variable.h"



//#include "SignatureManager.h"//suppressed for now

//This is a test space for this. I'd like to move it to a more fitting location.

//New
//*
#include "Instruction.h"
#include "InstructionType.h"
#include "OperatorType.h"
#include "Runtime.h"
#include "RuntimeVariable.h"
#include "IVariable.h"
#include "GlobalVariable.h"
#include "Literal.h"
#include "LiteralManager.h"
#include "Scope.h"
#include "Solution.h"
#include "RoutineBase.h"
#include "Operation.h"
#include "Operand.h"
#include "OperandType.h"
#include "Target.h"
#include "RuntimeVariable.h"
#include "MemberPointer.h"
#include "RoutineCompiler.h"

#include "External.h"
#include "ExternalHandle.h"
#include "ExternalManager.h"

#include "TypeID.h"


#include "ConcretePolicy.h"


#include "Lexicon/Impl/VariableType.h"

#include "ConcreteFunction.h"

#include "OverloadClause.h"




#include "Object.h"
#include "ObjectData.h"
#include "ObjectInfo.h"
#include "ObjectPolicy.h"
#include "ObjectPolicyHandle.h"
#include "ObjectPolicyManager.h"


#include "ObjectPoolData.h"





#include "Interface.h"
#include "InterfaceManager.h"
#include "InterfaceManagerImpl.h"//At this point object will load first, and this shit hasn't even instantiated yet. So it ends up bugging.
#include "InterfaceSingleton.h"
#include "Versioning.h"

#include "DeclareSpecifier.h"

#include "OverloadInput.h"

#include "Declaration.h"

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


	//I need to figure out why these only work if being used by 1 source, and why it won't work if that source
	// is variable type
	ITypePolicy* StorageType<Void>::operator()()
	{
		//Should be returning the none type.
		return nullptr;
	}
	AbstractTypePolicy* ValueType<Void>::operator()(Void&)
	{
		return nullptr;
	}



	ITypePolicy* StorageType<Number>::operator()()
	{
		ITypePolicy* policy = IdentityManager::instance->GetTypeByOffset(NUMBER_SET_NAME, 0);

		//Should already be specialized, so just sending it.
		return policy->FetchTypePolicy(nullptr);
	}

	AbstractTypePolicy* ValueType<Number>::operator()(Number& it)
	{

		ITypePolicy* policy = IdentityManager::instance->GetTypeByOffset(NUMBER_SET_NAME, it.GetOffset());


		//Should already be specialized, so just sending it.
		return policy->FetchTypePolicy(nullptr);
	}



	ITypePolicy* StorageType<String>::operator()()
	{
		ITypePolicy* policy = IdentityManager::instance->GetTypeByOffset(STRING_SET_NAME, CoreOffset::String);

		//Should already be specialized, so just sending it.
		return policy;
	}

	AbstractTypePolicy* ValueType<String>::operator()(String& it)
	{
		return StorageType<String>{}()->GetTypePolicy(nullptr);
	}


	//The currently unused ones



	ITypePolicy* StorageType<Delegate>::operator()()
	{
		return nullptr;
	}
	AbstractTypePolicy* ValueType<Delegate>::operator()(Delegate& it)
	{
		return nullptr;
	}



	ITypePolicy* StorageType<Object>::operator()()
	{
		return nullptr;
	}

	AbstractTypePolicy* ValueType<Object>::operator()(Object& it)
	{
		return nullptr;
	}


	///

	ITypePolicy* StorageType<FunctionHandle>::operator()()
	{
		return nullptr;
	}

	AbstractTypePolicy* ValueType<FunctionHandle>::operator()(FunctionHandle& it)
	{
		return nullptr;
	}




	ITypePolicy* StorageType<Array>::operator()()
	{
		return nullptr;
	}



	AbstractTypePolicy* ValueType<Array>::operator()(Array& it)
	{
		return nullptr;
	}




	ITypePolicy* StorageType<Variable>::operator()()
	{
		return nullptr;
	}

	AbstractTypePolicy* ValueType<Variable>::operator()(const Variable& it)
	{
		return nullptr;
	}




	//This is so temporary I hate that I'm doing it like this.
	AbstractTypePolicy* Variable::_CheckVariableType()
	{

		AbstractTypePolicy* result = std::visit([&](auto&& lhs) {
			return GetValueType(lhs);
			}, _value);

		return result;
	}

	



	void test()
	{
		StorageType<void> t;
		ValueType<void> t2;


	}






	class Formula : public ICallableUnit, public RoutineBase
	{
		//This does not derive from functions, due to not wishing to be included into any place like those.
		// Formulas as such are validated differently
	};


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

	//I'd need 2 versions of these. Annoying.
	class IntrinsicPolicy : public ConcretePolicy, public ICallableUnit
	{
		//Intrinsic policies are core policies that are accessible via any script, even if it's not included in commons.
		// this usually means that you don't need to use it's actual name, like with arrays and such.

		//The idea of intrinsic policies is that their names are found by having a type name with @at the beginning.
		// This means that it was created via keyword and is findable via all locations.
	public:

		/*
		PolicyBase();

		PolicyBase(uint32_t i);

		PolicyBase(std::string_view name, TypeOffset offset);









		static IntrinsicPolicy*
		//*/





	private:
		std::unordered_map<std::string, IntrinsicPolicy*> _intrinsicTypes;







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



	namespace Inner
	{
	
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
struct Extension
{
	//the idea is this is a struct that either takes a reference derives from it with no vtable and attempts to perform some new function on it.
};


#define TO_XVALUE(mc_expr) true ? mc_expr : static_cast<decltype(mc_expr)&&>(*(int*)0)

	namespace Fields
	{

		//Both global data and a FunctionData need a bare bones version that sits without things that would be included in the field.
		// Kinda hard to do, given that one of these things comes with the qualified type.

		//Noting, that it seems almost as if the qualified data does not actually seem to get used when it comes down to membered fields.
		// Like, that sort of information would be stored no where for the function.

		struct Test
		{
			int a;
		};
		
		void testR(int&&)
		{
			

		}

		void testL(int&&)
		{


		}


		void testD(const double&)
		{


		}

		void testD(double&&)
		{


		}


		void test()
		{
			int d = 1;
			testD(d);

			int i = 1;

			const double& rcd2 = 2;
			Test t{};
			auto te = true ? t.a : int{};
			int&& tt = 1;
			
			testL(5);


			//This is something that can turn something into an xvalue
			testR(std::forward<int>(t.a));
		}
		//conversion function.

		//Unsure if this should be using a reference to the runtime variable. I guess not? This doesn't intend to
		// actually alter the register, just the value the register holds.
		using __Func = RuntimeVariable(*)(RuntimeVariable);
		

		template<__Func Func>
		struct Convert final : public ICallableUnit
		{
			using _Self = Convert<Func>;

			static _Self& GetSingleton()
			{
				static _Self singleton{};

				return singleton;
			}


			inline static _Self* instance = &GetSingleton();

			virtual RuntimeVariable Invoke(std::vector<RuntimeVariable>& args, RuntimeVariable* def)
			{

				//Convert should be real simple.
				//here, it's supposed to return the conversion.
				return Func(args[0]);
			}


			Convert(__Func func) {}

		protected:
			constexpr Convert() = default;
		
			Convert(const Convert&) = delete;
			Convert(Convert&&) = delete;
			Convert& operator=(const Convert&) = delete;
			Convert& operator=(Convert&&) = delete;

		};



		auto converting = Convert<nullptr>::instance;


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

		void TestConvert()

		{
			auto fill = std::vector{ 1 };


			constexpr auto lambda = [](RuntimeVariable it)->RuntimeVariable {return it; };
			auto val1 = Convert <lambda> ::instance;
		}
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

	namespace ObjectWorksheet
	{
		namespace NewString
		{

			void Accept(const char8_t*)
			{

			}


			constexpr wchar_t t = '™';

			void test()
			{
				const wchar_t* test_v = L"thge";


				//std::strdup(test_v);
				//wcsdup
				//Need more than this.
				/*
				char *_strdup(
				   const char *strSource
				);
				wchar_t *_wcsdup(
				   const wchar_t *strSource
				);
				unsigned char *_mbsdup(
				   const unsigned char *strSource
				);
				//*/

				//Accept(test_v);
				std::basic_string<wchar_t> wide_str;
				std::basic_string<char8_t> smol_str;


				constexpr auto testrr = sizeof(decltype(wide_str)::value_type) > sizeof(decltype(smol_str)::value_type);

				using BiggerType = std::conditional_t<testrr, decltype(wide_str), decltype(smol_str) >;
				//wide_str;
			   //BiggerType comp_str = BiggerType{ smol_str };



			   //bool testComp = wide_str == smol_str;

			}
			//Not needed anymore.
			enum struct CharType : uint8_t
			{
				Byte,
				Word,
				DWord,
			};


			static_assert(std::is_unsigned_v<wchar_t>, "test failure");
			static_assert(alignof(wchar_t) == alignof(char16_t), "test failure");

			template <typename C>
			struct ptr//this is place holder.
			{
				const C* loc = nullptr;
			};

			namespace rgl
			{
				template< class T >
				concept character =
					std::is_same_v<char, T> ||
					std::is_same_v<char8_t, T> ||
					std::is_same_v<char16_t, T> ||
					std::is_same_v<char32_t, T> ||
					std::is_same_v<wchar_t, T> ||
					std::is_same_v<signed char, T> ||
					std::is_same_v<unsigned char, T>;


				//Here's the concept, I think I'm going to turn char, unsigned char, signed char, and wchar_t
				// into whatever char they're matched as. I'll make a custom type trait for this sort of thing.
				// it should match between files, but it means the basic string type will depend on your compiler.
				// But I mean, it always has

				//Correction, signed char remains.
			}


			template <rgl::character C>
			struct true_char
			{
				using type = C;
			};

			template <>
			struct true_char<unsigned char>
			{
				using type = char8_t;
			};

			template <>
			struct true_char<wchar_t>
			{
				using type = std::conditional_t<alignof(wchar_t) == alignof(char16_t), char16_t, char32_t>;
			};

			template <>
			struct true_char<char>
			{
				using type = std::conditional_t<std::is_unsigned_v<char>, char8_t, signed char>;
			};





			static void Imagine()
			{
				//std::use_facet<std::ctype<char
			}


			//do is spec of varient.
			template <specialization_of<std::variant> T>
			static constexpr auto free_space = 8 - sizeof(std::_Variant_index_t<std::variant_size_v<T>>);

			//These are the 4 types I'm keeping track of. Anything else will merely get turned into this.
			//signed char
			//char8_t
			//char16_t
			//char32_t




			using CharTypes = std::variant
				<
				ptr<signed char>,
				ptr<char8_t>,
				ptr<char16_t>,
				ptr<char32_t>
				>;

			//static_assert(std::is_same_v<char, signed char>);
			struct StringData
			{
				uint32_t size = 0;
			};
			REQ_OR_LESS_SIZE(StringData, free_space<CharTypes>);


			struct StringHelper
			{
				static constexpr auto offset = sizeof(CharTypes) - sizeof(StringData);

			protected:
				StringData& GetData()
				{
					auto a_this = (uintptr_t)this;

					return *reinterpret_cast<StringData*>(a_this + offset);
				}

				const StringData& GetData() const
				{
					auto a_this = (uintptr_t)this;

					return *reinterpret_cast<StringData*>(a_this + offset);
				}

				uint32_t& size()
				{
					return GetData().size;
				}

			public:



				uint32_t size() const
				{
					return GetData().size;
				}


				StringHelper()
				{
					size() = 0;
				}

				StringHelper(const RunDataHelper& other)
				{
					size() = 0;

				}


				StringHelper(RunDataHelper&& other)
				{
					size() = 0;
				}


				StringHelper& operator=(const RunDataHelper& other)
				{
					size() = 0;
					return *this;
				}



				StringHelper& operator=(RunDataHelper&& other)
				{
					size() = 0;
					return *this;
				}
			};



			template <rgl::character _Elem>
			struct CharHandle
			{
				using value_type = _Elem;

				//value_type

			};



			struct String
			{
				//This sort of thing will help converting one set to another. For now, I don't really need this sort of thing, and miss matches will be
				// rejected outright.
				//#include <codecvt>
				//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				//std::string narrow = converter.to_bytes(wide_utf16_source_string);
				//std::wstring wide = converter.from_bytes(narrow_utf8_source_string);

				//Unlike with arrays and such, strings will be treated as completely seperate types. This also helps not encounter any issues with generic implementations later.



				//I could get away with using a variant for this.
				void* ptr = nullptr;
				uint32_t size = 0;
				CharType type = CharType::Byte;


				bool operator==(const String& other)
				{
					if (type != other.type)
						return false;

					if (size != other.size)
						return false;


				}

				bool operator==(String&& other)
				{
					return operator==(other);
				}


				template <rgl::character Char>
				String(const Char* str)
				{

				}

				template <rgl::character Char>
				String(std::basic_string<Char> str)
				{

				}

			};
			REQUIRED_SIZE(String, 0x10);
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
	
	
#define COMMENTING /*fafafa*/

	struct Signature : public OverloadKey
	{
		//The match for this should be aimed to be as very little in terms of ambiguity as possible.
		//Thus, the stated project, script, and of course, the path.
		//The path for generics will probably simply be controlled by the amount of generic parameters it has, rather than the entires.
		// So no generic, no generic notation. Is generic, has generic notation. If it's the only one you can



		//Make these 2 a single function.
		Overload MatchFailure(OverloadFlag& flag)
		{
			logger::critical("Force failure");

			
			flag |= OverloadFlag::Failure;
			
			return {};
		}

		Overload MatchAmbiguous(OverloadFlag& flag)
		{
			logger::critical("Force ambiguous");

			
			flag |= OverloadFlag::Ambiguous;

			return {};
		}


		//Please note, this kind of match is what a clause should be doing.

		//This boolean needs to say if this failed to match, failed to be better, or resulted in ambiguity.
		Overload Match(OverloadClause* clause, ITypePolicy*, Overload*, OverloadFlag& a_flag) override
		{
			a_flag |= OverloadFlag::AllAccess;


			if (clause->PreEvaluate(result, parameters.size(), 0, a_flag) == false) {
				return MatchFailure(a_flag);
			}


			//I want to phase out of function. Maybe combine it with prev in some way.

			//Make a copy as to not completely mutate this.
			OverloadFlag flag = a_flag;


			Overload overload;


			OverloadEntry tar = clause->EvaluateEntry2(target, nullptr, -1, -1, flag);


			if (flag & OverloadFlag::Failure || tar.convertType != ConvertResult::Exact)
				return MatchFailure(a_flag);

			overload.target = tar.type;
			overload.clause = clause;

			constexpr auto offset_placeholder = 0;

			int winner = 0;

			for (int i = 0; i < parameters.size(); i++)
			{
				QualifiedType input = parameters[i];

				OverloadEntry entry = clause->EvaluateEntry2(input, nullptr, offset_placeholder, i, flag);

				if (flag & OverloadFlag::Failure || tar.convertType != ConvertResult::Exact)
					return MatchFailure(a_flag);



				//entry.funcs = conversion;
				//entry.type = input;
				//entry.index = index;


				overload.given.push_back(entry);
			}

			return overload;

		}



		QualifiedType result;
		QualifiedType target;
		
		std::vector<QualifiedType>				parameters;
		
	};
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
		entry.policy = GetStorageType<_Refless>();

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

	template <>
	struct StorageType<double>
	{

		ITypePolicy* operator()()
		{
			//I could just make this numeric
			static ITypePolicy* result = nullptr;

			if (!result) {
				
				//offset
				constexpr auto setting = LEX::Number::Settings::CreateFromType<double>();
				
				result = LEX::IdentityManager::instance->GetTypeByOffset("NUMBER", setting.GetOffset());
				
				logger::info("id? {}", (int)result->FetchTypeID());
			}

			return result;
		}
	};

#define TESTNAME CONCAT(Test,__COUNTER__)

	void SCRAPNAME(void(*infoke)())
	{
		
	}
	void Scrapname()
	{
		
	}
	struct Callish
	{
		void operator () ()
		{

		}
	};


	struct Scrap
	{
		//inline static Callish name;
		static void name(int)
		{

		}
		static void name()
		{

		}
	};

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


	struct _SIZE
	{
		int a, b, c;
	};
	void TESTNAME()
	{
		//Scrap::name::(int);
		int n{ 1 };

		_SIZE size(1, 2, 3);

		//TestA::TestB::GetTest()::last;
		Scrapname();
		Record empty;
		GetPolicyFromSpecifiers(empty, nullptr);
	}



	struct Dispatcher
	{
		//This isn't really that important, the main use of it is to just keep track of all dispatchers. Instead though,
		// I might make a thing where you can switch out registered external functions similar to that of changing states.
		inline static std::vector<std::unique_ptr<Dispatcher>> _dispatchList{ 1 };

		virtual void Dispatch(RuntimeVariable& result, Variable* target, std::vector<Variable*> args, ProcedureData& data) = 0;

	protected:
		//A constant code that will serve as varification. Actually unsure what this does but I'll keep it in case I remember
		static constexpr uint64_t func_code = 1;


		bool registered = false;

		//Omega temp shit here, needs to actually have the path and stuff here.
		static bool Register(Dispatcher* dispatch, FunctionBase* dispatchee)
		{
			//This type can likely produce a name actually.

			//If this thing cannot take on a procedure is what this check is.
			if (false)
				return false;

			_dispatchList.emplace_back(std::unique_ptr<Dispatcher>{dispatch});

			auto reciever = [](RuntimeVariable& result, Variable* target, std::vector<Variable*> args, ProcedureData& data) -> void
				{
					Dispatcher* funcOld = _dispatchList[0].get();
					
					auto p_data = data.srcFunc->GetProcedureData();
					
					//check data here.
					
					Dispatcher* func = reinterpret_cast<Dispatcher*>(p_data);


					if (!func) {
						//Throw exception.
						return;
					}
					
					func->Dispatch(result, target, args, data);
				};

			dispatchee->SetProcedureData(reciever, reinterpret_cast<uint64_t>(dispatch));

			dispatch->registered = true;

			return true;
		}


		//static bool Register(Dispatcher* dispatch);
	};
	template <typename T>
	struct Unvariable 
	{
		
	};

	
	
	//Should be named var converter

	template <stl::castable_from<Variable> Type>
	struct Unvariable<Type>
	{
		Type operator()(Variable* var)
		{
			if (var->CanCastTo<Type>() == false)
				report::apply::fatal("Current value of Variable is unable to be cast to '{}'.", typeid(Type).name());

			return static_cast<Type>(*var);
		}
	};



	/*
	template <>
	struct Unvariable<double>
	{
		double operator()(Variable* var)
		{
			auto num = var->AsNumber();

			double result;

			if (num.As(result) == false)
				throw nullptr;
		
			return result;
		}
	};
	//*/

	
	struct OtherTest
	{

	};
	struct Test
	{
		operator int() { return 0; }
		template<std::floating_point T>
		explicit operator T() { return 0; }
		
		explicit operator T() { return {}; }
	};

	


	void TEST()
	{
#define unique_name CONCAT(test, __COUNTER__)
		constexpr bool test4 = specialization_of<VariableValue, std::variant>;
		constexpr bool test = is_any_convertible_v<float, int, int, int, int>;
		constexpr bool test2 = is_variant_convertible_v<VariableValue, float>;
		constexpr bool test3 = variant_convertible_to<VariableValue, double>;
		//This breaks despite the fact it should be hable to handle this. Please consult.
		//constexpr bool test5 = variant_convertible_to<double, VariableValue>;

		constexpr bool test226 = stl::convertible_from<double, Variable>;
		constexpr bool test6 = stl ::convertible_to<Variable, double>;

	

		constexpr bool test7 = std::convertible_to<Test, int>;
		constexpr bool test8 = std::convertible_to<Test, float>;
		constexpr bool test9 = std::convertible_to<Test, T>;
		constexpr bool test10 = std::is_convertible_v<Number, double>;
		
		constexpr bool _1 = __is_convertible_to(Number, double);
		constexpr bool _2 = stl::castable_to<Number, double>;
		constexpr auto test11 = convertible_variant_index<VariableValue, double>::value;
		
		
		//Test2<double>();
		//Test3<double>();
		Variable a;
		a.CanCastTo<double>();
		//GetStorageType<String>();
		Unvariable<double>{}(&a);
		static_cast<double>(a);


	}


	template <class R, class T, class... Args>
	struct BasicDispatcher : public Dispatcher//<R(Args...)>
	{
		using _Self = BasicDispatcher<R, T, Args...>;
		using _Function = R(*)(T, Args...);
		


		static bool Create(_Function func, FunctionBase* dispatchee)
		{
			std::unique_ptr<Dispatcher> test_dispatch = std::make_unique<_Self>(func);

			bool result = Register(test_dispatch.get(), dispatchee);

			if (result)//If it's successful, we don't need to delete the dispatcher.
				test_dispatch.release();

			return result;
		}


		

		R(*_callback)(T, Args...) = nullptr;

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



		void Dispatch(RuntimeVariable& result, Variable* target, std::vector<Variable*> args, ProcedureData& data) override
		{
			//Unload that shit.
			using Arg1 = std::tuple_element_t<0, std::tuple<Args...>>;

			constexpr size_t arg_size = std::tuple_size_v<std::tuple<Args...>>;

			if (auto list_size = args.size(); list_size != arg_size)
			{
				//Shit isn't the same fucking size I'm losing my mind.
				throw nullptr;
			}

			//typename function_traits<std::remove_pointer_t<decltype(T)>>::arguments args;

			//I'll want to remove the references due to being unable to load them like this.
			std::tuple<T, std::remove_reference_t<Args>...> tuple;
			//static_assert(std::is_same_v<std::tuple<Target, int, int, int>,
			//	function_traits<std::remove_pointer_t<decltype(T)>>::arguments>, "GN  NE NIP");

			std::get<0>(tuple) = Unvariable<T>{}(target);


			ValueImport(tuple, args);
			//Here we get the return type and switch up on it.

			//Confirm if the types are correct.

			if constexpr (std::is_same_v<R, void>) {
				std::apply(_callback, tuple);
			}
			else {
				R to_result = std::apply(_callback, tuple);

				//under more normal situations, vary this by whether it's a ref or not.

				result = to_result;
			}
		}

		constexpr BasicDispatcher(_Function func) : _callback{ func }
		{
		}
	};

	//Something like this needs to be an interfaceSingleton.
	struct ProcedureLoader
	{
		//ProcedureLoader is an object that exists to link


		//RegisterProcedure


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





	bool RegisterProcedure(Procedure procedure, FunctionBase* function)
	{
		function->SetProcedureData(procedure, 0);

		return 0;
	}

	bool RegisterProcedure(Procedure procedure, std::string path)
	{
		FunctionBase* function = Nameless::GetFunctionFromPath(path);

		return RegisterProcedure(procedure, function);
	}




	template <typename R, typename... Args>
	bool RegisterProcedure(R(*func)(Args...), FunctionBase* base)
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


		OverloadFlag flag = OverloadFlag::None;

		auto overload = sign.Match(base, nullptr, nullptr, flag);

		if (flag & OverloadFlag::Failure)
			logger::info("FAILED TO MATCH");
		else
			logger::info("SUCCESS TO MATCH");


		//static auto dispatch = new BasicDispatcher(func, base);
		auto result = BasicDispatcher<R, Args...>::Create(func, base);

		return result;
	}

	//thing to get function here.

	template <typename R, typename... Args>
	bool RegisterProcedure(R(*func)(Args...), std::string path)
	{
		FunctionBase* base = Nameless::GetFunctionFromPath(path);

		return RegisterProcedure(func, base);
	}

	

	namespace PathingTest
	{
		enum struct Vers
		{
			Script,
			Function, 
			Type
		};


		enum struct Prefer
		{
			None,
			Type,
			Script,
			Project,
		};

		struct OverloadGenericInput : public OverloadKey
		{
			//This struct provides the ability to fill out a generic argument without affecting the other.


			//The idea of the OverloadGenericInput

			OverloadKey* arguments = nullptr;
		};
		


		/*
		PolicyBase* SearchTypeImpl(Environment* a_this, std::string name, OverloadKey* key)
		{
			//No overload key means it has is expected to have no generic, if so it has one.
			return nullptr;
		}




		//All versions end up at these last 2 locations.
		PolicyBase* SearchTypeImpl(Environment* a_this, std::string name, OverloadKey* key)
		{
			//No overload key means it has is expected to have no generic, if so it has one.
			
			if (!key)
				return dynamic_cast<PolicyBase*>(a_this->TEMPSearchType(name));
			else
				return dynamic_cast<PolicyBase*>(a_this->SearchType(name, *key));
		}
		
		QualifiedField SearchFieldImpl(Environment* a_this, std::string name, OverloadKey* key)
		{
			//No overload key means it has is expected to have no generic, if so it has one.
			if (!key)
			return a_this->TEMPSearchField(name);
			else
				return a_this->SearchField(name, *key);

		}


		FunctionBase* SearchFunctionImpl(Environment* a_this, std::string name, OverloadKey& key, Overload& out)
		{
			//This requires a key, none optional
			
			//This needs a function above it that fills generic arguments. Possibly scratch that.


			auto funcs = a_this->FindFunctions(name);

			std::vector<FunctionBase*> clauses;

			//FunctionBase* test = functions[0]->Get();

			std::transform(funcs.begin(), funcs.end(), clauses.begin(), [&](auto it) {return it->Get(); });

			if (funcs.size() && CheckOverload(key, clauses, out) == true)
			{
				return dynamic_cast<FunctionBase*>(out.clause);
			}

			return nullptr;


		}



		FunctionBase* SearchFunction(Environment* a_this, std::string name, OverloadKey& key, Overload& out)
		{
			//This should prepare the generic arguments.

			return SearchFunctionImpl(a_this, name, key, out);
		}




		PolicyBase* SearchType(Environment* a_this, Prefer preference, std::string_view node, std::vector<std::string_view>* g_args = nullptr) { return nullptr; }
		PolicyBase* SearchType(Environment* a_this, Record& _path) { return nullptr; }

		QualifiedField SearchField(Environment* a_this, Record& path)
		{
			//Should do similar shit to search environment, creating generic args along the way or something like that.
			return SearchFieldImpl(a_this, path.GetTag(), nullptr);

		}

		Environment* SearchEnvironment(Environment* a_this, Record& path) {
			//A matter of preference here is that unless it's specifically referencing something, it will try all it's options.

			//So the order of checking goes from type -> script -> project

			SyntaxType type = path.SYNTAX().type;
			Prefer force;
			bool cont = false;
			Environment* result = nullptr;
			Element* stuff;

			//Instead, I will be getting elements and will refer to them as scopes.
			//If element, what can it not do?
			//Elements can getprojects so they can get scripts, projects need to become environments. At least in name. Kinda like a pivot.
			// An I environments.


			do
			{//the idea is that continue is set once when scope name is found once. After that, it's put into a cycle of which it prefers.

				switch (type)
				{
				case SyntaxType::Scriptname://This should be script name.
					type = SyntaxType::ProjectName;
					force = Prefer::Script; break;

				case SyntaxType::Scopename:
					cont = true;

					[[fallthrough]];
				case SyntaxType::Typename:
					type = SyntaxType::Scriptname;
					force = Prefer::Type; break;



				case SyntaxType::ProjectName:
					cont = false;
					force = Prefer::Project; break;
				default:
					return nullptr;//invalid
				}
				
				switch (force)
				{
				case Prefer::Script:
				{
					//Rule, no children.
					result = a_this->GetProject()->FindScript(path.GetTag());

					break;
				}
				
				case Prefer::Type:
				{
					//OverloadInput input{};

					OverloadKey* key = nullptr;

					

					result = SearchTypeImpl(a_this, path.GetTag(), key);

			
					break;
				}

				case Prefer::Project:
				{
					ProjectManager::GetProject(path.GetTag());
					result = nullptr;
					break;
				}



				}
				

				
			} while (cont && !result);



			return result;
		}

		Environment* SearchEnvironmentPath(Environment* a_this, Record*& _path)
		{
			//This should keep going as long as the left path is a binary. When it stops being so, it should return the last searching environment, and adjust the
			// path reference.


			if (_path->SYNTAX().type != SyntaxType::Binary || _path->GetTag() != "::") {
				//If this is not the operator, we return this
				return a_this;
			}

			//This simply needs environment.
			Environment* new_this = SearchEnvironment(a_this, *_path->FindChild("left"));

			_path = _path->FindChild("right");
			

			
			if (_path->SYNTAX().type != SyntaxType::Binary || _path->GetTag() != "::") {
				//This part can be made boilerplate
				return a_this;
			}
			else {
				return SearchEnvironmentPath(a_this, _path);
			}


		}






		//This is in Environment.cpp, needs to be moved out.
		bool CheckOverload(OverloadKey& input, std::vector<FunctionBase*> clauses, Overload& ret)
		{
			Overload out;

			Overload* last = nullptr;


			for (auto clause : clauses)
			{
				OverloadFlag flags = OverloadFlag::None;

				Overload overload = input.Match(clause, nullptr, last, flags);

				if (flags & OverloadFlag::Failure) {
					logger::info("Failure");
					continue;
				}



				if (flags & OverloadFlag::Ambiguous) {
					logger::info("Ambiguous");
					last = nullptr;
					break;
				}

				if (0) {
					bool _continue = false;
					bool _break = false;


					cycle_switch(flags)
					{
				case OverloadFlag::Failure:
					logger::info("Failure");
					_continue = true;
					continue;

				case OverloadFlag::Ambiguous:
					logger::info("Ambiguous");
					_break = true;
					break;

					}

					if (_continue)
						continue;

					if (_break)
						break;

				}


				out = std::move(overload);

				last = &out;
			}

			if (last)
				ret = *last;

			return last;
		}





		PolicyBase* SearchTypePath(Environment* a_this, Record& _path)
		{
			

			Record* path = &_path;

			Environment* env = SearchEnvironmentPath(a_this, path);

			return SearchType(env, *path);

		}

		//This actually probably will want to return the function info, because of something like generic functions.
		FunctionBase* SearchFunctionPath(Environment* a_this, Record& _path, OverloadKey& input, Overload& out)
		{
			//While this can be a field, it will not be treated like one.

			//Here, record is only for the path, not for anything else.

			//out is only used when we actually need it.

			//THIS is the way these functions should look.
			if (!a_this) {
				return nullptr;
			}

			Record* path = &_path;

			if (Environment* env = SearchEnvironmentPath(a_this, path); !env) {
				//This is mostly correct, but it should also use it's includes. Hm, that actually would be better suited in the search functions below
				// when compiling all viable named functions/types/fields.
				return SearchFunctionPath(env, _path, input, out);
			}
			else {
				return SearchFunction(env, path->GetTag(), input, out);
			}


		}

		QualifiedField SearchFieldPath(Environment* a_this, Record& _path)
		{
			//Similar to type, this doesn't quite need overloads.


			Record* path = &_path;

			Environment* env = SearchEnvironmentPath(a_this, path);


			return SearchField(env, *path);
		}

		//On top of all this, I think I'd like something called get name from path. Kinda does what a lot of the above does, but basically goes
		// through the motions until the entire path is mapped out.

		//When it comes down to the string path versions, I actually think I'll just parse the thing.

		//*/



		//Changes
		//-Project should be an environment.
		//-Return a function info for functions instead of a function base. We have no idea which it is, so give them that instead.
		//-Make function info a clause probably. Need to think on if I just want that to be the stand in or not.
		//  With the function info becoming something like that, it would now need to be an IFunction would it not? What if the thing gets specialized?
		//-These search functions have to be used from scope, and then climb their way out. So some of these should perhaps be located in an external place.
		// of course, they'll always be focused on environment. BUT for now I don't need to worry about that.
	}



	template<typename T, typename = void>
	detail::not_implemented UnusableFunc(char = {})
	{
		logger::info("I'm not implemented");
		return {};
	}

	template<>
	detail::not_implemented UnusableFunc<int>(char)
	{
		logger::info("Neither am I implemented");
		return {};
	}

	//The implementation one should be the one with the concept
	template<std::floating_point T, typename = void>
	void* UnusableFunc()
	{
		logger::info("but I am implemented");
		return {};
	}

	template<std::floating_point T, typename = void> requires(sizeof(T) > 0x4)
		void* UnusableFunc()
	{
		logger::info("but am I implemented?");
		return {};
	}


	INITIALIZE()
	{
		PointerType<int*>;
		UnusableFunc<char>();
		UnusableFunc<int>();
		UnusableFunc<float>();
		UnusableFunc<double>();
	}


	struct accepts_all
	{
		//This type helps resolve some ambiguity between 2 like functions by making the invalid one convert while the actual on takes a reference.
		constexpr accepts_all() = default;

		template <typename T>
		constexpr accepts_all(T) {}


		template <typename T>
		constexpr accepts_all(const T&) {}

		template <typename T>
		constexpr accepts_all(T&&) {}
	};

	//*
	namespace NewVariableType
	{
		class ITypePolicy;
		class AbstractTypePolicy;

		//tag to tell tha this is being used by a single type.
		struct single_type {};


		namespace detail
		{
			using not_implemented = ::LEX::detail::not_implemented;
			//move example
			struct example 
			{
				//The storage type function can be defined like this.
				// Function must be static, have no parameters, and return ITypePolicy.
				static ITypePolicy* GetStorageType()
				{
					return {};
				}



				//As the value type function can be defined like this,
				// Requiring the function to be const and membered (virtual allowed), no parameters, and return AbstractTypePolicy.
				AbstractTypePolicy* GetValueType() const
				{
					return {};
				}

				//Additionally, both storage and value can be defined like this. This denotes that the value type does not change based on runtime 
				// value. This will take precedence over other declared variables.
				// It's required the function must be static and return AbstractTypePolicy.
				static AbstractTypePolicy* GetVariableType()
				{
					return {};
				}
			};
		}

		struct wrong_type
		{
			static int GetStorageType()
			{
				return {};
			}

			ITypePolicy* GetValueType()
			{
				return {};
			}

			AbstractTypePolicy* GetVariableType()
			{
				return {};
			}
		};


		template <class _Derived, class _Base>
		concept pointer_derived_from = std::derived_from<std::remove_pointer_t<_Derived>, std::remove_pointer_t<_Base>>;

		//This is the sorta test version I wish to use in order
		template<typename T>
		constexpr bool exper_fail = requires(T t) { t.getname(); };


		//At some point I want to do something kinda like this.
		// The idea would be that we can use the deviation as something to feed a constexpr condition that would help identify which part exactly failed
		// mainly for the loser MSVC users.
		template<typename T>
		constexpr int _storage_type_deviation = requires(T t) { t.getname(); };



		//These will need supplemental version
		template<typename T> concept type_has_variable_type = requires()
		{
			{ T::GetVariableType() } -> pointer_derived_from<AbstractTypePolicy*>;
		};

		template<typename T> concept type_has_storage_type = requires()
		{
			{ T::GetStorageType() } -> pointer_derived_from<ITypePolicy*>;
		} || type_has_variable_type<T>;
		template<typename T> concept type_has_value_type = requires(const T& t)
		{
			{ t.GetValueType() } -> pointer_derived_from<AbstractTypePolicy*>;
		} || type_has_variable_type<T>;
		






		template <type_has_storage_type T>
		void foo1()
		{
			
		}

		template <type_has_value_type T>
		void foo2()
		{

		}


		template <type_has_variable_type T>
		void foo3()
		{

		}




		void TESTTHEOREM()
		{
			constexpr bool testit = std::derived_from<AbstractTypePolicy*, ITypePolicy*>;

			foo1<detail::example>();
			foo2<detail::example>();
			foo3<detail::example>();

			//foo1<wrong_type>();
			//foo2<wrong_type>();
			//foo3<wrong_type>();

			constexpr bool test = exper_fail<int>;
		}



		//This is storageType
		template <typename T>
		struct StorageType : public detail::not_implemented
		{
			//instead of void, I can perhaps use a different type.
			//No operator as we can see.

			//I still don't like this a ton because you need to implement the function still

		private:
			ITypePolicy* operator()()
			{

			}
		};

		//And this is value type.
		template <typename T, typename = void>
		struct ValueType : public detail::not_implemented
		{
		
			//This should never be used manually, so private so just in case it actually is used it will throw 
			// a compiler error.
			AbstractTypePolicy* operator()(const T&)
			{
				//static_assert
				return nullptr;
			}
		};

		//This is the non implemented version of GetStorageType
		template<typename T, typename = void>
		detail::not_implemented GetStorageType(detail::not_implemented = {})
		{
			static_assert(std::is_same_v<T, T>, "Unimplemented version of GetStorageType called.");
			return {};
		}

		template<typename T>
		detail::not_implemented GetValueType(const accepts_all, detail::not_implemented = {})
		{
			static_assert(std::is_same_v<T, T>, "Unimplemented version of GetValueType called.");
			return {};
		}

		//This is the implemented version of these functions. They need to pass a concept that states
		// the function itself implements what's required.
		
		template<type_has_storage_type T>
		ITypePolicy* GetStorageType()
		{
			return {};
		}


		template<type_has_value_type T>
		AbstractTypePolicy* GetValueType(const T&)
		{
			return {};
		}

		///////////////////////////////////////////////


		//To be defined later, this should be a concept that declares GetStorageType and GetValueType need to have an implementation
		// for each to be used.
		struct TBDL {};



		//These tell if a storage/value type is valid
		template<typename T> concept value_type_implemented = requires(const T& t)
		{
			{ ValueType<T>{}.operator()(t) } -> pointer_derived_from<AbstractTypePolicy*>;
		};
		template<typename T> concept store_type_implemented = requires()
		{
			{ StorageType<T>{}.operator()() } -> pointer_derived_from<ITypePolicy*>;
		};

		template <>
		struct StorageType<TBDL>
		{
			using T = TBDL;

			ITypePolicy* operator()()
			{
				static_assert(std::is_same_v<T, T>, "Example ReturnType<TBDL> cannot be used.");
			}

		};

		template <>
		struct ValueType<TBDL>
		{
			using T = TBDL;

			AbstractTypePolicy* operator()(const TBDL&)
			{
				
			}
		};


		//I could make another concept for this but Ill keep it like this for now

		//template<typename T> concept val_func_impl = !std::is_same_v<std::invoke_result_t<decltype(GetStorageType<T>)>, detail::not_implemented>;

		//template<typename T> concept stor_func_impl = !std::is_same_v<std::invoke_result_t<decltype(GetValueType<T>), const T&>, detail::not_implemented>;

		template<typename T> concept val_func_impl = requires(const T & t)
		{
			{ GetValueType<T>(t) } -> std::same_as<AbstractTypePolicy*>;
		};
		template<typename T> concept stor_func_impl = requires()
		{
			{ GetStorageType<T>() } -> std::same_as<ITypePolicy*>;
		};

		//*
		template <stor_func_impl T>
		struct StorageType<T>
		{
			ITypePolicy* operator()()
			{
				return GetStorageType<T>();
			}

		};

		template <val_func_impl T>
		struct ValueType<T>
		{

			AbstractTypePolicy* operator()(const T& arg)
			{
				return GetValueType<T>(arg);
			}
		};
		//*/


		template <value_type_implemented T>
		void bar1()
		{

		}

		template<typename T> concept bar2_constraint = requires(const T & t)
		{
			{ ValueType<T>{}.operator()(t) } -> pointer_derived_from<AbstractTypePolicy*>;
		};


		template <store_type_implemented T>
		void bar2()
		{

		}

		void TestBar()
		{
			ValueType<TBDL> t;

			bar1<TBDL>();
			bar2<TBDL>();
		}

		
		//These are spotty right now. Please redo.
		namespace detail
		{
			template<typename T> concept ret_impl = !std::is_base_of_v<detail::not_implemented, StorageType<T>> &&
				//!std::is_same_v<StorageType<example>, StorageType<T>> && 
				requires()
				{
					{ StorageType<T>{}.operator()() } -> pointer_derived_from<ITypePolicy*>;
				};

			template<typename T> concept var_impl = !std::is_base_of_v<detail::not_implemented, ValueType<T>> &&
				//!std::is_same_v<ValueType<example>, ValueType<T>> && 
				requires(const T& t)
				{
					{ ValueType<T>{}.operator()(t) } -> pointer_derived_from<AbstractTypePolicy*>;
				};

			//
			template<typename T> concept obj_impl = ret_impl<T> && var_impl<T>;


			template<typename T>
			using custom_decay = T;//std::decay_t<T>;//
		}


		template <detail::var_impl T>
		AbstractTypePolicy* GetValueTypeImpl(detail::custom_decay<T>& arg)
		{
			using _T = detail::custom_decay<T>;

			return ValueType<_T>{}(arg);
		}


		template <detail::ret_impl T>
		ITypePolicy* GetStorageTypeImpl()
		{
			using _T = detail::custom_decay<T>;

			return StorageType<_T>{}();
		}



		//When using a reference it will only get the value type. When getting it via pointer it may give you the storage type if null.
		//Direct functions can be used if one so chooses.

		//<!> Note, these args shouldn't be using custom T, instead that should onl be used to load the next GetValue/Storage function. 
		// This is so that the explicit specialization doesn't use the wrong type



		template <detail::obj_impl T>
		ITypePolicy* GetVariableType(detail::custom_decay<T>* arg)
		{
			using _T = detail::custom_decay<T>;


			ITypePolicy* type = nullptr;

			if (arg) {
				type = GetValueTypeImpl<_T>(*arg);
			}

			if (!type) {
				type = GetStorageTypeImpl<_T>();
			}

			return type;

		}

		
		template <detail::obj_impl T>
		AbstractTypePolicy* GetVariableType(detail::custom_decay<T>& arg)
		{
			using _T = detail::custom_decay<T>;

			 
			return GetValueTypeImpl<_T>(arg);
		}
		
		template <>
		struct StorageType<int>
		{
			using T = int;

			//Getting the ReturnType returns a ITypePolicy, an interface of a type, as a single C++ type could possibly
			// be able to represent a generic imaginary object.

			ITypePolicy* operator()()
			{
				return {};
			}

		};

		template <>
		struct ValueType<int>
		{
			using T = int;


			AbstractTypePolicy* operator()(const T&)
			{
				return {};
			}
		};

		/*

		template <typename T>
		struct FailTest;

		template <typename T>
		struct FailTest
		{
			FailTest() = delete;
			FailTest(const FailTest&) = delete;
			FailTest(FailTest&&) = delete;
		};
		
		//This seems to work best, but only if the function in question is being called within intellisense.
		//This mainly exists as a concept that I can use to tell me why I failed certain concepts if someone isn't using clang.
		//#if !defined(__INTELLISENSE__)
		template<typename T, template <typename> typename U = FailTest>
		void Failure(U<T> = {})
		{
			//Want to find a way to make intellisense forgive this, despite it being true.
			//What makes this failure function so powerful is the fact that when used, it will actually lead you to the place that is failing.
			
			//To stack on the "this isn't going to work signals, I can both make the function itself no discard, as well as making it's returns not_implemented.
			// This will prevent the expected result from falling in, and if I prevent not_implemented from being instantiable, I can prevent it from creating anything.


			//FailTest<T> {};
		}
		//#endif
		//*/
		void FULLtest()
		{
			
			using Example = detail::example;

			Example ex;
			ValueType<Example> v;
			StorageType<Example> s;

			constexpr bool _1 = detail::ret_impl<Example>;
			constexpr bool _2 = detail::var_impl<Example>;
			
			//ITypePolicy* test = GetStorageType<int>();

			int in = 1;
			AbstractTypePolicy* test3 = GetVariableType(in);

			ITypePolicy* test = GetStorageType<Example>();
			AbstractTypePolicy* test2 = GetValueType<Example>(ex);
			constexpr bool _3 = stor_func_impl<Example>;
			constexpr bool _4 = val_func_impl<Example>;
			constexpr bool _5 = detail::obj_impl<Example>;
			constexpr bool _6 = type_has_value_type<Example>;

			//Failure<int>();

			static_assert(_3, "3");
			static_assert(_4, "4");
			static_assert(_5, "5");
			static_assert(_6, "6");

			GetVariableType(ex);
		}
		
		





	//Note: Remove static_assert and "using T" in implementation.
	



	}
	//*/
	//This kind of set up allows for type based specialization. I think I may actually change my mind and centralize the main thing more
	// as types no longer have the advantage when it comes to explicit specialization.
	template <typename T, typename = void>
	struct VTest
	{

	};

	template <typename T>
	struct VTest<T>
	{

	};

	template <>
	struct VTest<int>
	{

	};

	template <typename T>
	void FTest() {}


	template <std::integral T>
	void FTest() {}


	void FunctionTest()
	{
		VTest<int>();
		FTest<int>();
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
