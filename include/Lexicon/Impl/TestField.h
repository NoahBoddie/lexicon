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
		ITypePolicy* policy = IdentityManager::instance->GetTypeByOffset(NUMBER_SET_NAME, CoreOffset::Number);

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


//A mac
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
	
	namespace Trash
	{

		int* const test_ptr = nullptr;

		struct TestOp
		{
			inline static std::string _;

		};


		void testRef(int&)
		{
			int const  t[5]{};

			//int& t = t[1];

			const int i{};
			//i = 1;
		}

		void testPtr()
		{
			testRef(*test_ptr);
		}





		void foo1(int& i)
		{

		}


		int& foo2(int& i)
		{
			return i;
		}

		int& foo()
		{
			int i = 0;

			return foo2(i);
		}


		struct StructA;


		struct StructB
		{
			StructB() = default;

			StructB(StructA&) {}



		};

		struct StructA
		{
			operator StructB()
			{
				return StructB{};
			}


		};

		void control()
		{
			StructA a;
			//StructB b = a;
			const int u = 0;

			const int y = 0;

			//u = y;

			int& i = foo();
		}
	}


	namespace OverloadRework
	{
		enum struct ConvertResult 
		{ 

			QualError4 = -7,
			QualError3 = -6,
			QualError2 = -5,
			QualError1 = -4,
			Inaccessible = -3,
			Ineligible = -2,
			IllegalIntern = -1,
			//Might rearrange these to be greater
			Exact = 0,
			TypeDefined,
			ImplDefined,
			UserDefined,
			UserToImplDefined,



			Failure = -1,//Anything under or equal to failure doesn't need it's value recorded for anything, it's just a conversion error code.
		};

		struct Conversion
		{
			struct
			{
				union
				{
					uint64_t		raw = 0;
					IFunction*		userDefined;
					ICallableUnit*	implDefined;
				};
				
				//When a user defined conversion can be converted this is what is used.
				ICallableUnit* userToImpl = nullptr;

			};

			ConvertResult type = ConvertResult::Ineligible;

			//Merge convert results with this shit btw.

			//This should tell how you jumped from one value to the next. Approx 3 pointers long.

			operator bool() const
			{
				return raw;
			}
		};

		struct OverloadEntry
		{
			//I'd care about the padding and stuff here but its so small I do not care.

			QualifiedType type;//Only qualified because the going to place isn't important for 
			Conversion convert;



			std::array<uint32_t, 2> hash{};

			uint32_t distance = 0;

			bool initialized = false;//If not initialized


			size_t index;
		};


		enum struct OverloadSection
		{
			//Exact order might change
			Required,
			Suggested,
			Optional,
			Default,
		};

		ENUM(OverloadFlag)
		{
			None,
			ReqFilled = 1 << 0,		//Once required gets filled, this gets ticked so that the overload doesn't use the improper overload.
			UsesDefault = 1 << 1,	//If it uses 
			UsesOptional = 1<< 2,	//Manually defining an optional will result in no optionals being able to be implicitly used.
			NoConvert = 1 << 3,		//If no conversions are used in one, converting ones are no longer valid.


			FinalEntry = 1 << 5,	//Used if this is the final entry, allowing the clause to declare there's more required.
		
			Failure = 1 << 6,
			Ambiguous = 1 << 7,
		};

		struct OverloadClause;

		struct Overload
		{
			//This is now a container that shows the given options for an overload.

			OverloadClause* clause;

			QualifiedType target;//

			//These align perfectly with arguments given.
			// This has to be loaded with a properly sorted default by this point.
			// Defaults have to go here because 
			std::vector<OverloadEntry> given;


			std::unordered_map<std::string, OverloadEntry> defaults;


			//TODO: These should be 3 way comparisons, stemming from the arg type to both parameters.
			//Remember that this function should be handled by something such as qualified type rather than OverloadEntry.

			int SafeCompare(QualifiedType& other, QualifiedType& arg)
			{
				//This version is abandoned now


				if (!this)
					return -1;

				if (target && !other)
					return -1;
				else if (!target && other)
					return -1;
				else if (!target && !other)
					return 0;
				if (target.IsConvertToQualified(other) == true)
					return -1;
				else if (other.IsConvertToQualified(target) == true)
					return 1;
				
				return 0;
			}


			int SafeCompare(std::string name, OverloadEntry& entry, QualifiedType& arg)
			{
				if (!this)
					return -1;

				return 0;
			}

			int SafeCompare(size_t i, OverloadEntry& entry, QualifiedType& arg)
			{
				if (!this)
					return -1;

				if (i >= given.size())
					return 1;

				auto& tar = given[i];


				if (tar.type && !entry.type)
					return -1;
				else if (!tar.type && entry.type)
					return -1;
				else if (!tar.type && !entry.type)
					return 0;

				return 0;

				//Currently there is no actually way to compare. it is or it isn't Implement properly in the merger.

				bool left = tar.type.IsConvertToQualified(entry.type);
				bool right = entry.type.IsConvertToQualified(tar.type);

				if (left  == true)
					return -1;
				else if (right == true)
					return 1;

				return 0;
			}
		};

		struct OverloadKey
		{//The argument side of the overload.

			virtual Overload Match(OverloadClause*, Overload* prev, OverloadFlag& flag) = 0;
			
			
		};

		struct OverloadClause : public OverloadKey
		{//The parameter side of the overload.
			

			virtual Overload Match(OverloadClause*, Overload* prev, OverloadFlag& flag)
			{
				flag |= OverloadFlag::Ambiguous;
				return {};
			}



			//A preeval might be good. Just something to say Thing expects 8 groups,
			virtual bool PreEvaluate(size_t suggested, size_t optional, OverloadFlag) = 0;
			
			//Fuck it, these return non-booleans and use something else to denote their failures.

			//I'm thinking of having an extra parameter to show if I mean generic args or not.
			virtual OverloadEntry EvaluateEntry2(QualifiedType, size_t offset, size_t index, OverloadFlag& flags) = 0;
			virtual OverloadEntry EvaluateDefault2(QualifiedType, std::string name, OverloadFlag& flags) = 0;



			//This should only be called on the winnning one.
			//This should actually accept the Overload comparison object, as that can represent optional parameter pack values as well.
			virtual std::vector<OverloadEntry> GetRemainingEvals(Overload& entries, OverloadFlag& flags) = 0;
			//After this is used, it should be resized.


			//This is missing the ability to process default stuff.

			//Basically it needs the information to be able to edit
			// OverloadEntry can possibly be instructions as overload stuff. The idea being that it also outputs where a given portion of data is supposed to go.
			// To do this, I could use solutions as to not corrupt what is given.
			//But having it like this also allows for more freedom in terms of loading into shit.


			//Also, the place where OverloadInputs get loaded shouldn't try to move register data around. Instead, it should have vectors of instruction lists.
			// Then, I should free each individually.

			//virtual Void EvaluateDefault(std::vector)
		};
		
		//The flow.
		//OverloadKey::Match, if yes back of the line. If not, toss results.
		//-Keep going until no more clauses are available.
		//Next, each overload is comparied to each other.
		//<!> What if I could compare the newest entry to the previous overload at the same time?


		struct FakeFunction : public OverloadClause
		{//The parameter side of the overload.

			std::string name;

			ParameterInfo thisInfo{ nullptr, "", (uint32_t)-1};
			std::vector<ParameterInfo> params;
			//A preeval might be good. Just something to say Thing expects 8 groups,

			bool PreEvaluate(size_t suggested, size_t optional, OverloadFlag flag) override
			{
				if (flag & OverloadFlag::UsesDefault)
					return false;

				if (optional)
					return false;

				if (params.size() != suggested)
					return false;

				return true;
			}

			std::vector<OverloadEntry> GetRemainingEvals(Overload& entries, OverloadFlag& flags) override
			{
				return {};
			}


			//Fuck it, these return non-booleans and use something else to denote their failures.

			OverloadEntry EvaluateEntry2(QualifiedType type, size_t offset, size_t index, OverloadFlag& flags) override
			{
				OverloadEntry result;
				
				//TODO: This is very temp, the index can exceed the param size please remove this when params keyword is implemented
				if (index != -1 && index >= params.size())
				{
					logger::critical("Failure to evaluate");
					flags |= OverloadFlag::Failure;
					return {};
				}

				ParameterInfo* subject = index == -1 ? &thisInfo : &params[index];
				
				QualifiedType sub_type = subject->GetQualifiedType();
				
				if (type)
				{
					LEX::Conversion* out = nullptr;//Is entries if it's not the thing. Currently, not setting this up.
					//TODO: This returns the wrong value rn.
					if (type.IsConvertToQualified(sub_type, out) == false)
					{
						flags |= OverloadFlag::Failure;
						return {};
					}


					result.convert.type = ConvertResult::TypeDefined;
					result.index = subject->GetFieldIndex();
					result.type = sub_type;
				}
				else
				{
					result.convert.type = ConvertResult::Ineligible;
					result.index = -1;
				}
			
				return result;
				
			}
			OverloadEntry EvaluateDefault2(QualifiedType type, std::string name, OverloadFlag& flags) override
			{
				flags |= OverloadFlag::Failure;
				return { };
			}




			//This is missing the ability to process default stuff.

			//Basically it needs the information to be able to edit
			// OverloadEntry can possibly be instructions as overload stuff. The idea being that it also outputs where a given portion of data is supposed to go.
			// To do this, I could use solutions as to not corrupt what is given.
			//But having it like this also allows for more freedom in terms of loading into shit.


			//Also, the place where OverloadInputs get loaded shouldn't try to move register data around. Instead, it should have vectors of instruction lists.
			// Then, I should free each individually.

			//virtual Void EvaluateDefault(std::vector)
		};





		class OverloadInput : public OverloadKey
		{
		public:
			
			Overload MatchFailure(OverloadFlag& flag, Overload* prev = nullptr)
			{
				logger::critical("Force failure");
				
				//This should only be used when it's an ambiguous match I think.
				flag |= OverloadFlag::Failure;
				//This feels very unnecessary and possibly unused.
				if (prev)
					return *prev;

				return {};
			}

			Overload MatchAmbiguous(OverloadFlag& flag)
			{
				logger::critical("Force ambiguous");

				//This should only be used when it's an ambiguous match I think.
				flag |= OverloadFlag::Ambiguous;

				return {};
			}


			//This boolean needs to say if this failed to match, failed to be better, or resulted in ambiguity.
			virtual Overload Match(OverloadClause* clause, Overload* prev, OverloadFlag& a_flag) override
			{
				if (clause->PreEvaluate(paramInput.size(), defaultInput.size(), a_flag) == false) {
					logger::info("pre-eval fail");
					return MatchFailure(a_flag);
				}


				//I want to phase out of function. Maybe combine it with prev in some way.

				//Make a copy as to not completely mutate this.
				OverloadFlag flag = a_flag;

				if (defaultInput.empty() == false) {
					flag |= OverloadFlag::UsesDefault;
				}

				Overload overload;

				QualifiedType type;

				if (object && object->target)
					type = *object->target;


				OverloadEntry tar = clause->EvaluateEntry2(type, -1, -1, flag);


				if (flag & OverloadFlag::Failure)
					return MatchFailure(a_flag, prev);


				auto new_winner = prev->SafeCompare(-1, tar, type);

				if (0)//If other is prefered, return a failure.
					return *prev;

				//Also a thought, [this] is a parameter too. Granted a hidden one.

				overload.target = tar.type;
				overload.clause = clause;

				constexpr auto offset_placeholder = 0;

				int winner = 0;

				for (int i = 0; i < paramInput.size(); i++)
				{
					QualifiedType input = paramInput[i];

					OverloadEntry entry = clause->EvaluateEntry2(input, offset_placeholder, i, flag);

					if (flag & OverloadFlag::Failure)
						return MatchFailure(a_flag, prev);

					

					//entry.funcs = conversion;
					//entry.type = input;
					//entry.index = index;
					
					//Compare should this input-> prev vs entry
					
					auto new_winner = prev->SafeCompare(i, entry, input);

					if (!winner)
						winner = new_winner;
					else if(winner < 0 && new_winner > 1 || winner > 0 && new_winner < 1)
					{// shit isn't valid anymore.
						return MatchAmbiguous(a_flag);
					}


					overload.given.push_back(entry);
				}

				auto it = defaultInput.begin();

				for (int i = 0; it != defaultInput.end(); i++, it++)
				{
					QualifiedType input = it->second;

					OverloadEntry entry = clause->EvaluateDefault2(input, it->first, flag);

					if (flag & OverloadFlag::Failure)
						return MatchFailure(a_flag, prev);


					//entry.funcs = conversion;
					//entry.type = input->second;
					//entry.index = index;


					auto new_winner = prev->SafeCompare(it->first, entry, input);

					if (!winner)
						winner = new_winner;
					else if (winner < 0 && new_winner > 1 || winner > 0 && new_winner < 1)
					{// shit isn't valid anymore.
						return MatchAmbiguous(a_flag);
					}


					overload.defaults[it->first] = entry;
				}

				if (winner > 0) {
					logger::info("right winner");
					return *prev;
				}
				else if (winner < 0) {
					logger::info("left winner");
					return overload;
				}
				else {
					logger::info("no winner");
					return MatchAmbiguous(a_flag);
				}

			}


		 


			TargetObject* object = nullptr;
			//std::vector<ITypePolicy*>			genericInput;
			std::vector<Solution>				paramInput;
			std::map<std::string, Solution>		defaultInput;//rename
		};


		INITIALIZE()
		{
			ConcretePolicy type1{};
			ConcretePolicy type2{};
			ConcretePolicy type3{};

			Solution sol1{ &type1, OperandType::None, 0 };
			Solution sol2{ &type2, OperandType::None, 0 };
			Solution sol3{ &type3, OperandType::None, 0 };


			OverloadInput input;

			input.paramInput.push_back(sol1);
			input.paramInput.push_back(sol1);
			input.paramInput.push_back(sol2);
			input.paramInput.push_back(sol3);
			input.paramInput.push_back(sol3);

			FakeFunction func1;
			FakeFunction func2;
			FakeFunction func3;

			{
				func1.name = "Func1";
			
				func1.params.emplace_back(QualifiedType{ &type1 }, "var_1", 0);
				func1.params.emplace_back(QualifiedType{ &type2 }, "var_2", 1);
				func1.params.emplace_back(QualifiedType{ &type3 }, "var_3", 2);
			}
			{
				func2.name = "Func2";

				func2.params.emplace_back(QualifiedType{ &type1 }, "var_1", 0);
				func2.params.emplace_back(QualifiedType{ &type1 }, "var_2", 1);
				func2.params.emplace_back(QualifiedType{ &type2 }, "var_3", 2);
				func2.params.emplace_back(QualifiedType{ &type3 }, "var_4", 3);
				func2.params.emplace_back(QualifiedType{ &type2 }, "var_5", 4);
			}
			{
				func3.name = "Func3";

				func3.params.emplace_back(QualifiedType{ &type1 }, "var_1", 0);
				func3.params.emplace_back(QualifiedType{ &type1 }, "var_2", 1);
				func3.params.emplace_back(QualifiedType{ &type2 }, "var_3", 2);
				func3.params.emplace_back(QualifiedType{ &type3 }, "var_4", 3);
				func3.params.emplace_back(QualifiedType{ &type3 }, "var_5", 4);
			}

			std::vector<FakeFunction*> clauses{ &func1, &func2, &func3 };

			FakeFunction* final_func = nullptr;
			Overload out;

			Overload* last = nullptr;

			for (auto clause : clauses)
			{
				OverloadFlag flags = OverloadFlag::None;

				Overload overload = input.Match(clause, last, flags);
				
				bool _continue = false;
				bool _break = false;

				if (flags & OverloadFlag::Failure) {
					logger::info("Failure"); continue;
				}
					


				if (flags & OverloadFlag::Ambiguous){
					logger::info("Ambiguous");
					last = nullptr;
					break;
				}

				if (0)
				cycle_switch (flags)
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

				
				out = std::move(overload);

				last = &out;
			}

			if (last && last->clause)
				logger::info("FUNC CHOOSEN: {}", dynamic_cast<FakeFunction*>(last->clause)->name);
			else
				logger::info("NO FUNC CHOOSEN");


			//The testing zone.
		};


	}

	namespace Inheritance
	{
		struct FakeType;

		struct NewOverload
		{
			size_t par;
			size_t exactHash;
			size_t convertHash;
		};

		struct InheritData
		{
			static constexpr size_t virtual_distance = -1;
			
			//The hash is a value

			//The hash is a value that represents memory wise, where said object can be considered within memory.
			std::array<uint32_t, 2> hash{};


			//Distance is -1 if virtually inherited
			uint32_t distance = 0;
			FakeType* type = nullptr;//AbstractTypePolicy* type;

			uint32_t _id = 0;
			
			//This can possibly be unionized, with the sign bit being able to tell if one or the other.
			uint32_t ownerIndex = 0;
			
			uint32_t memberIndex = 0;

			Access access;//

			
			bool isGeneric = false;//This differs if the id is an instance id or a type id.
			bool virtInherited = false;

			constexpr bool operator<(const InheritData& other) const
			{
				if (isGeneric != other.isGeneric)
					return !isGeneric < !other.isGeneric;

				return  distance < other.distance;
			}

			//Need to figure out how to move these.

			//These are deprecated, as methods
			//uint32_t memberRange [2];//This should be where this entries stuff starts, and then where it ends. same deal with the other.
			//uint32_t methodRange [2];

			Access GetAccess() const
			{
				return access & ~Access::Internal;
			}

			void SetAccess(Access a_acc)
			{
				auto pop = access & Access::Internal;
				a_acc &= ~Access::Internal;
				access = a_acc;
				access |= pop;
			}

			//I want to merge a bunch of flags into where access is. Stuff like is generic or is internal, virtual inherited all can probably be shoved in there.

			void SetInternal(bool v)
			{
				if (v) {
					access |= Access::Internal;
				}
				else {
					access &= ~Access::Internal;
				}
			}

			bool IsInternal() const
			{
				return access & Access::Internal;
			}

			bool IsVirtualInherited() const
			{
				return virtInherited;
			}

		};


		struct OverloadEntry
		{
			FakeType* type;

			std::array<uint32_t, 2> hash{};

			uint32_t distance = 0;

			bool initialized = false;//If not initialized

			//Right here include how the conversion is being carried out. With that, I can 

			OverloadEntry FinalizeOld(FakeType* other);
			bool Finalize(OverloadEntry& other);
		};


		struct HierarchyData
		{
			//FakeType will become a part of something called Hierarchy data. TypeBases have this, but so do Specializations of generic types.

			//Id like it if in a test of ambiguity this always wins, but that might not be possible in this set up.
			FakeType* extends = nullptr;
			std::vector<InheritData> inheritance;

			std::vector<FakeType*> members;


			uint32_t hashRange = 0;//Range is equal to zero to this number.
			
			uint32_t memberCount = 0;//Should bind classes increase this value any? Nah, probably handle in post.
			std::string name;

			bool hasInternal = false;


			HierarchyData() = default;

			HierarchyData(std::string n, std::vector<FakeType*> m = {}) : name{ n }, members{ m }, memberCount{ (uint32_t)m.size() } {}
			HierarchyData(std::string n, uint32_t num) : name{ n }, members{ num }, memberCount{ num } {}

		};



		enum struct ConvertResult
		{
			QualError4				= -7,
			QualError3				= -6,
			QualError2				= -5,
			QualError1				= -4,
			Inaccessible			= -3,
			Ineligible				= -2,
			IllegalIntern			= -1,
			Exact					=  0,
			TypeDefined,
			ImplDefined,
			UserDefined,
			UserToImplDefined,

			

			Failure = -1,//Anything under or equal to failure doesn't need it's value recorded for anything, it's just a conversion error code.
		};

		struct FakeType : public HierarchyData
		{
			using HierarchyData::HierarchyData;



			InheritData* _InheritData(InheritData& data)
			{
				if (data.IsInternal() == true) {
					return nullptr;
				}
				
				//Should search with an inherit data with the same type.
				auto end = inheritance.end();

				auto it = std::find_if(inheritance.begin(), end, [&](InheritData& other) {return data.type == other.type && !other.IsInternal(); });

				if (it != end)
					return &*it;

				return nullptr;
			}


			InheritData* GetInheritData(FakeType* type)
			{
				//This ends up pulling intera

				//Should search with an inherit data with the same type.

				InheritData* result = nullptr;

				//By default, it will exit automatically once when it finds something unless has internal is switched.
				bool exit = !hasInternal;

				for (InheritData& other : inheritance)
				{
					//Ignore internal check if something doesn't personally have an internal.

					if (type != other.type)
						continue;


					if (other.IsInternal())
					{
						if (!exit){
							if (_IsInternalParent(&other) == false){
								continue;
							}

							//Otherwise we're exiting here
							exit = true;
						}
						else {
							//Cant allow an internal to end up here.
							continue;
						}
					}

					result = &other;

					if (exit)
						break;
				}

				return result;



				auto end = inheritance.end();

				auto it = std::find_if(inheritance.begin(), end, 
					[&](InheritData& other) 
					{
						return type == other.type; 
					});

				if (it != end) {
					InheritData* result = &*it;
					
					
					return result;
				}

				return nullptr;
			}


			void FinalizeAndSort()
			{
				//std::sort(inheritance.begin(), inheritance.end(), std::less<InheritData>());return;

				for (int x = 0; x < inheritance.size(); x++)
				{
					InheritData& left = inheritance[x];

					for (int y = x + 1; y < inheritance.size(); y++)
					{
						InheritData& right = inheritance[y];

						if (right < left) {

							InheritData buffer = right;

							right = std::move(left);
							left = std::move(buffer);

							//Do a check of each here, switching 
							for (int z = x; z < inheritance.size(); z++)
							{
								auto& index = inheritance[z].ownerIndex;

								if (index - 1 == x) {
									index = y + 1;
								}
								else if (index - 1 == y) {
									index = x + 1;
								}
							}
						}
					}
				}

				uint32_t count = members.size();

				for (int x = 0; x < inheritance.size(); x++)
				{
					auto& entry = inheritance[x];
					logger::info("before {} {} {}", entry.memberIndex, count, entry.type->members.size());

					entry.memberIndex = count;
					count += (uint32_t)entry.type->members.size();

					logger::info("after {} {} {}", entry.memberIndex, count, entry.type->members.size());
				}

			}

			std::vector<InheritData> GetInheritFrom(uint32_t hashMin, uint32_t idxInc)
			{
				//This should include access type, and who it's from. This way internal inheritance can be better handled.

				std::vector<InheritData> result;

				result.reserve(inheritance.size() + 1);

				InheritData& self = result.emplace_back();

				//uint32_t largestHash = hashMin;

				self.type = this;

				self.hash[0] = hashMin;
				self.hash[1] = hashRange + hashMin;

				self.access = Access::Public;

				//self.memberIndex = members

				self.distance = 1;
			
				//We'll want to make a copy, because we're gonna rehash it by the index.
				for (InheritData data : inheritance)
				{

					data.hash[0] += hashMin;
					data.hash[1] += hashMin;
					data.ownerIndex += idxInc;
					//I still have to check against this due to virtual inheritance.
					//if (largestHash < data.hash[1])
					//	largestHash = data.hash[1];
					

					switch (data.access)
					{
					case Access::Private:
						data.access = Access::None; break;
					case Access::PrivateInternal:
						data.access = Access::NoneInternal; break;
					}
					

					data.distance++;
					result.push_back(data);
				}

				//self.hash[1] = largestHash;

				return result;
			}

			
			void SetDerivesTo(FakeType* other, bool intern = false, Access a_access = Access::Public)
			{
				//I need to carry around a set of types not allowed to be submitted for this when things need to handle their own inheritance.
				//Basically, for every item that SetDerivesTo is used on, that's another entry that SetDerives shouldn't be used on.
				// The nature of this means there's really no good time to be able to use set derives to, that being said, I need to handle it like a function
				// that need only run once. And if it relies on things that haven't run once, then it needs to run it once. For now, direct line of action.

				//Additionally, you cannot directly add stuff that you already have in, only when inherited from something else.

				//thread lock this, and remember a change of everything that set derived is processing.
				uint32_t idxInc = (uint32_t)inheritance.size() + 1;

				std::vector<InheritData> inherits = other->GetInheritFrom(hashRange + 1, idxInc);

				bool second = false;

				if (intern)
					a_access |= Access::Internal;

				//Later
				Access access = a_access & ~Access::Internal;

				bool intern_ = a_access & Access::Internal;
				
				if (!hasInternal)
					hasInternal = intern;


				for (auto& data : inherits)
				{
					bool _second = second;
					second = true;

					if (data.GetAccess() > access){
						data.SetAccess(access);
					}
					
					if (intern_) {
						data.SetInternal(true);
					}


					if (data.IsInternal() == false) {
						if (auto* prev_data = _InheritData(data); prev_data)
						{


							//We'll want to pretend like it's a new object without disturbing it. Making it 0 is fine, as it needs
							// to pull results from somewhere else to be valid and if it can't either something else is better or it's ambiguous.


							//having these use the hash of 0 makes it valid for comparison for nothing else (if not corrected by it's comparison) other than
							// the derived class.

							prev_data->hash[0] = 0;
							prev_data->hash[1] = 0;
							prev_data->ownerIndex = 0;
							prev_data->virtInherited = true;

							//Inherits whichever access from here is greater.
							if (auto data_access = data.GetAccess(); data_access > prev_data->GetAccess()) {
								prev_data->SetAccess(data_access);
							}

							//I think the distance will actually be 2 if it's not the first.
							prev_data->distance = 1 + _second;
						
							continue;
						}
					}


					if (data.hash[1] > hashRange)
						hashRange = data.hash[1];

					auto old = memberCount;

					data.memberIndex = old;//This is the place where this starts.
					memberCount += (uint32_t)data.type->members.size();
					//logger::info("add {} {} {}", old, data.memberIndex, memberCount);
					//This doesn't get hit at times. I need to inspect why.
								
					inheritance.push_back(data);
				}
			}

			OverloadEntry CreateEntry(FakeType* target)
			{
				auto data = !target || target == this ? nullptr : GetInheritData(target);


				if (data)
					return { data->type, data->hash, data->distance, !data->virtInherited };
				else
					return { this, {0, hashRange}, 0, true };
			}

			void PrintInheritance()
			{
				RGL_LOG(info, "Class: {}: Ranged: (0/{}), Members: ({}/{})", name, hashRange, memberCount, members.size());

				for (auto& basis : inheritance)
				{
					std::string access;

					switch (basis.access)
					{
					case Access::Public:
						access = "public"; break;
					case Access::Protected:
						access = "protected"; break;
					case Access::Private:
						access = "private"; break;
					case Access::None:
						access = "no access"; break;

					case Access::PublicInternal:
						access = "public internal"; break;
					case Access::ProtectedInternal:
						access = "protected internal"; break;
					case Access::PrivateInternal:
						access = "private internal"; break;
					case Access::NoneInternal:
						access = "internal"; break;
					}

					logger::info("|	Name: {}, Hash: {}/{}, Dist: {}, Ownr: {}, intern: {}, access: {}, Mbrs: ({}+{})", 
						basis.type->name, basis.hash[0], basis.hash[1], basis.distance, basis.ownerIndex, basis.IsInternal(), access, basis.memberIndex, basis.type->members.size());
				}
			}


			int CompareType(OverloadEntry left, OverloadEntry right)
			{
				if (!left.initialized && !right.initialized)
				{
					left = left.FinalizeOld(right.type);
					right = right.FinalizeOld(left.type);

					//OverloadEntry left = a_lhs.FinalizeOld(a_rhs.type);
					//OverloadEntry right = a_rhs.FinalizeOld(a_lhs.type);
				}

				logger::info("left({}/{}), right({}/{})", left.hash[0], left.hash[1], right.hash[0], right.hash[1]);


				if (left.hash[0] <= right.hash[1] && left.hash[1] >= right.hash[0]) {
					return left.distance - right.distance;
				}
				if (left.hash[0] >= right.hash[1] && left.hash[1] <= right.hash[0]) {
					return left.distance - right.distance;
				}

				return 0;
			}

			int CompareType(FakeType* a_lhs, FakeType* a_rhs)
			{
				OverloadEntry left = CreateEntry(a_lhs);
				OverloadEntry right = CreateEntry(a_rhs);

				return CompareType(left, right);
			}

			bool _IsInternalParent(InheritData* intern)
			{
				if (!intern || intern->IsInternal() == false) 
					return false;

				bool cont = intern->ownerIndex && intern->IsInternal();

				while (cont)
				{
					intern = &inheritance[intern->ownerIndex - 1];

					cont = intern->ownerIndex && intern->IsInternal();
				}

				return intern->IsInternal() && intern->ownerIndex == 0;

			}

			
			//This needs some form of conversion result.
			ConvertResult IsConvertible(FakeType* other, FakeType* asking)
			{
				if (this == other) {
					return ConvertResult::Exact;
				}
				
				//First we check if the entry exists. Based on what its access is, what we do changes.
				//Public: Conversion is allowed.
				//Protected: asking type must other must derive from
				//Private: asking type must have other's distance be 1.

				//Internal: The trail of internals within asking must lead to an internal that has an owner index of 0. Otherwise, it is too seperated to be converted.
				//Will need to make a collection to measure this.
				// Actually, simplier way (though the above is still required) this and asking need to be the same. Actually this isn't right either, internal is about
				// this, not who's asking.


				//Whether it can convert should basically be solved here, this should never return an internal it does not own.
				InheritData* convert_data = GetInheritData(other);
				



				//Not gonna worry about function conversions for a while.
				if (!convert_data) {
					//TODO: If this is internal, I might perform some saving throw with the asking type.
					return ConvertResult::Ineligible;
				}

				//To do this saving throw, we'd need to get if asking has that inheritdata from other first (check for internal first)
				// then, if it does, ask if asking is viewable this is convertible to asking.
				


				Access access = convert_data->access & ~Access::Internal;
				//Should this ever actually be internal?
				bool is_internal = convert_data->IsInternal();//convert_data->access & Access::Internal;

				if (is_internal) {
					logger::info("Owner should be 0 => {}", convert_data->ownerIndex);
				}


				switch (access)
				{
				//No access is given, thus conversion is not possible from here.
				//*This has to be tested later.
				//case Access::None:
				//	return false;//No access between types

				case Access::Public:
					return ConvertResult::TypeDefined;//Type converted
				}

				if (!asking) {
					return ConvertResult::Inaccessible;//No access from here
				}
				
				if (asking == other){
					return ConvertResult::TypeDefined;
				}

				//By this point, please note that internal should not even be a thought here.
				//Being able to get someone's inheritdata that belongs to a specific class might be valuable.
				//*Might need to recant this.
				InheritData* access_data = asking->GetInheritData(other);


				
				if (!access_data || access_data->GetAccess() == Access::None){
					return ConvertResult::Inaccessible;//No access from here
				}

				//if (access_data->GetAccess())
				

				
				return ConvertResult::TypeDefined;//Should have access
			}

			//These should use weak ordering I think instead. Afterwords, once when the types are removed it should concern itself with the final pass.
		};

		//All this finalization seems to only matter and need to happen if both left and right are virtually inherited.
		//Problem is using the perspective from the arg class is a no go. It has to be from itself, or from the other.
		OverloadEntry OverloadEntry::FinalizeOld(FakeType* other)
		{



			auto other_data = other->GetInheritData(type);

			if (!other_data) {
				return type->CreateEntry(nullptr);
			}
			//Even if it's virtually inherited, that's ok.

			return other->CreateEntry(type);
		}


		bool OverloadEntry::Finalize(OverloadEntry& other)
		{
			//if (initialized)
			//	return *this;



			auto other_data = other.type->GetInheritData(type);

			if (!other_data) {

			}
			//Even if it's virtually inherited, that's ok.

			*this = other.type->CreateEntry(type);
			other = other.type->CreateEntry(nullptr);

			return true;
		}

		struct PrivA{};

		struct PrivB : private PrivA
		{
			void Test();

		};


		struct PrivC : public PrivB
		{
			void Test()
			{
				//Asking: PrivC
				//Target: PrivB
				//Other: PrivA

				//To know if this is permitted, we must see if access of other within asking is not none.
				PrivB* b = this;
				//PrivA* a = this;
			}

		};

		struct ImplTest2;

		struct ImplTest1
		{
			operator ImplTest2();

			operator int()
			{
				return 1;
			}
		};

		struct ImplTest2
		{

			ImplTest2(int)
			{

			}
		};

		ImplTest1::operator ImplTest2()
		{
			return 1;
		}

		void PrivB::Test()
		{
			PrivC* test = nullptr;
			PrivA* a = test;
		}

		void ImplFunc(float)
		{

		}

		INITIALIZE()
		{
			return;
			ImplTest1 implA;
			ImplFunc(implA);

			ImplTest2 implB = implA;

			PrivB b; 

			//PrivA& a = b;

			FakeType MiscBase{ "MiscBase" };
			FakeType SingBase{ "SingleBase" };


			FakeType IBase{ "IBase" };
			FakeType IBase2{ "IBase2" };
			FakeType DiffBase{ "DiffBase" };

			FakeType IntBase{ "IntBase" };
			
			FakeType ClassA{ "ClassA", 2 };// };//
			FakeType ClassB{ "ClassB", 3 };//};//
			FakeType Derived{ "Derived", 5 };// };//
			FakeType Derived2{ "Derived2" };
			FakeType Derived3{ "Derived3" };


			IBase.SetDerivesTo(&IBase2);

			DiffBase.SetDerivesTo(&IBase2);

			MiscBase.SetDerivesTo(&IBase, true);


			IntBase.SetDerivesTo(&MiscBase);
			IntBase.SetDerivesTo(&IBase, true);

			ClassA.SetDerivesTo(&IBase);
			ClassB.SetDerivesTo(&IBase);

			Derived.SetDerivesTo(&IntBase, false, Access::Private);
			Derived.SetDerivesTo(&SingBase, true);
			Derived.SetDerivesTo(&ClassA);
			Derived.SetDerivesTo(&ClassB);

			//Test soething like miscBase being here now.
			//Derived2.SetDerivesTo(&ClassA);
			Derived2.SetDerivesTo(&DiffBase);
			Derived2.SetDerivesTo(&Derived);
			
			Derived3.SetDerivesTo(&SingBase);
			Derived3.SetDerivesTo(&Derived2);
			
			//Derived3.FinalizeAndSort();

			Derived3.PrintInheritance();
			Derived2.PrintInheritance();
			Derived.PrintInheritance();
			ClassA.PrintInheritance();
			ClassB.PrintInheritance();
			IntBase.PrintInheritance();
			IBase.PrintInheritance();
			DiffBase.PrintInheritance();
			IBase2.PrintInheritance();

		
			//These comparisions will be created by the given conversions, and at a later point will largely be the Qualified type the argument (which is the target) of CmpType
			// So at this point, no need


			//Within the conversions, to find your conversion to the given type, you find an entry that is internal, and allowed to be read from this zone.
			// Then after that you walk through it's indices to see if any of them are the same as our current one.

			//All in all, I need a better way to facilitate the comparison between conversions.

			logger::critical("Derived3 conversion to IBase from Static: {}", Derived3.IsConvertible(&IBase, nullptr) > ConvertResult::Failure);
			logger::critical("Derived3 conversion to IntBase from Static: {}", Derived3.IsConvertible(&IntBase, nullptr) > ConvertResult::Failure);
			logger::critical("Derived3 conversion to IntBase from Derived: {}", Derived3.IsConvertible(&IntBase, &Derived) > ConvertResult::Failure);
			logger::critical("Derived3 conversion to IntBase from Derived2: {}", Derived3.IsConvertible(&IntBase, &Derived2) > ConvertResult::Failure);
			logger::critical("Derived3 conversion to IntBase from IntBase: {}", Derived3.IsConvertible(&IntBase, &IntBase) > ConvertResult::Failure);


			logger::critical("DiffBase vs IBase from Derived2 {}", Derived2.CompareType(&DiffBase, &IBase));
			logger::critical("Derived vs IBase from Derived2 {}", Derived2.CompareType(&Derived, &IBase));
			logger::critical("Derived vs Derived2 from Derived2 {}", Derived2.CompareType(&Derived, &Derived2));
			logger::critical("IBase2 vs IBase from Derived2 {}", Derived2.CompareType(&IBase2, &IBase));
			logger::critical("SingBase vs IBase from Derived2 {}", Derived2.CompareType(&SingBase, &IBase));



			logger::critical("Derived3 conversion to SingBase from Static {}", Derived3.IsConvertible(&SingBase, nullptr) > ConvertResult::Failure);
			logger::critical("Derived2 conversion to SingBase from Static {}", Derived2.IsConvertible(&SingBase, nullptr) > ConvertResult::Failure);
			logger::critical("Derived conversion to SingBase from Static {}", Derived.IsConvertible(&SingBase, nullptr) > ConvertResult::Failure);


		};

		struct ParData
		{
		//private://I don't quite know how I'll assign these. So they'll be open for a bit.
			union
			{
				uint64_t raw = 0;
				
				struct
				{
					//Important part of par data, if this type here is 0, then regardless of hash it wins.
					uint32_t type;
					//I'd like to break this down into finer pieces.
					// Namely, figuring out which order I'd like refs and constants to go, and other qualifiers.
					uint32_t qual;
				};
			};

		public:

			bool IsUserConversion() const
			{
				//If conversion is detected, and the other isn't a conversion, the non-conversion wins.
				//If conversion is detected, and the other is too, it comes down to the par + hash.
				return type & (1 << 31);
			}


			operator uint64_t() const
			{
				return raw;
			}

			

		};


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
		auto format(const T& v, FormatContext& a_ctx)
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
