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



	//At a later point a single function and probably handle all of these.



	template <>
	struct StorageType<Void>
	{

		ITypePolicy* operator()()
		{
			//Should be returning the none type.
			return nullptr;
		}
	};

	template <>
	struct ValueType<Void>
	{

		AbstractTypePolicy* operator()(Void&)
		{
			return nullptr;
		}
	};



	template <>
	struct StorageType<Number>
	{

		ITypePolicy* operator()()
		{
			ITypePolicy* policy = IdentityManager::GetTypeByOffset(NUMBER_SET_NAME, CoreOffset::Number);

			//Should already be specialized, so just sending it.
			return policy->GetTypePolicy(nullptr);
		}
	};

	template <>
	struct ValueType<Number>
	{

		AbstractTypePolicy* operator()(Number& it)
		{

			ITypePolicy* policy = IdentityManager::GetTypeByOffset(NUMBER_SET_NAME, it.GetOffset());


			//Should already be specialized, so just sending it.
			return policy ? policy->GetTypePolicy(nullptr) : nullptr;
		}
	};



	template <>
	struct StorageType<String>
	{

		ITypePolicy* operator()()
		{
			ITypePolicy* policy = IdentityManager::GetTypeByOffset(STRING_SET_NAME, CoreOffset::String);

			//Should already be specialized, so just sending it.
			return policy;
		}
	};

	template <>
	struct ValueType<String>
	{

		AbstractTypePolicy* operator()(String& it)
		{
			return StorageType<String>{}()->GetTypePolicy(nullptr);
		}
	};


	//The currently unused once

	template <>
	struct StorageType<Delegate>
	{

		ITypePolicy* operator()()
		{
			return nullptr;
		}
	};

	template <>
	struct ValueType<Delegate>
	{

		AbstractTypePolicy* operator()(Delegate& it)
		{
			return nullptr;
		}
	};


	///Need to handle the regular versions too.

	template <>
	struct StorageType<ExternalHandle>
	{

		ITypePolicy* operator()()
		{
			return nullptr;
		}
	};

	template <>
	struct ValueType<ExternalHandle>
	{

		AbstractTypePolicy* operator()(ExternalHandle& it)
		{
			return nullptr;
		}
	};


	///

	template <>
	struct StorageType<FunctionHandle>
	{

		ITypePolicy* operator()()
		{
			return nullptr;
		}
	};

	template <>
	struct ValueType<FunctionHandle>
	{

		AbstractTypePolicy* operator()(FunctionHandle& it)
		{
			return nullptr;
		}
	};



	///

	template <>
	struct StorageType<Array>
	{

		ITypePolicy* operator()()
		{
			return nullptr;
		}
	};

	template <>
	struct ValueType<Array>
	{

		AbstractTypePolicy* operator()(Array& it)
		{
			return nullptr;
		}
	};



	template <>
	struct StorageType<Variable>//
	{
		//This wouldn simply return the object type, as a 
		ITypePolicy* operator()()
		{
			return nullptr;
		}
	};

	template <>
	struct ValueType<Variable>// : single_type
	{
		//this should visit its data, but this doesn't have permission. Likely best to test the implementation 
		// inside fo the thing.
		AbstractTypePolicy* operator()(const Variable& it)
		{
			return nullptr;
		}
	};

	//This is so temporary I hate that I'm doing it like this.
	AbstractTypePolicy* Variable::_CheckVariableType()
	{

		AbstractTypePolicy* result = std::visit([&](auto&& lhs) {
			return GetVariableType(lhs);
			}, _data);

		return result;
	}


	void test()
	{
		StorageType<void> t;
		ValueType<void> t2;


	}






	class Formula :public ICallableUnit, public RoutineBase
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

		IdentityManager::GenerateID("NUMBER", Number::Settings::length);

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

		int foo(TypeA1&)
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


		int bar(TypeB1&)
		{
			return 1;
		}


		void Test()
		{
			TypeA a;
			TypeB b;
			TypeC c;

			//First: arg1 par 3, arg2 par 3 (or actual 5 if we consider multiple inherit)
			//Second call: arg1 par 7, arg2 par 4

			//foo(c);

			//bar(c);
		}
	};







	/*
	class Signature : public OverloadClause
	{
		//Signature is a very temporary thing, at later point, most of this information will be viewable loose
		// with the function

		template <typename T1, typename T2>
		using _OrderedMap = std::vector <std::pair<T1, T2>>;

		enum Usage
		{
			Required,	//Theres no possible deduction
			Optional,	//It comes with a default value
			Implied		//It's use isn't default, but is deducible from a
		};

		struct GenericReq
		{
			ITypePolicy*	policy = nullptr;
			Usage			usage = Required;
		};

		std::vector<GenericReq>				genericInput;//This can be handled through the genericpolicie
		std::vector<Solution>					parameterInput;//This is handled in full via the parameter stuff
		_OrderedMap<std::string, Solution>		defaultInput;//This can be handled the same as the above.



	};
	//*/






	

	struct Object;

	template <typename T>
	//using pointer = std::unique_ptr<T>;
	using pointer = T*;

	//In the future, object builds have the same requirements as a regular native function. Actually, maybe not the best idea. Have different functions handle that.
	using ObjectBuilder = pointer<Object>(*)();



	struct ObjectInfo
	{
		const std::type_info* type;//This is made with create, to denote source.
		HMODULE program;



		ObjectBuilder ctor;


		//All ideas from the claimed id to  the end of the offset are a part of the registered type.
		// So think of from form all the way
		TypeIndex index;
		TypeOffset offset;


		pointer<Object> BuildObject();
	};

	struct Object
	{
		friend ObjectInfo;

		virtual ~Object() = default;
		ObjectInfo* GetInfo()
		{
			return _info;
		}
		
	private:
		ObjectInfo* _info;


	};
	pointer<Object> ObjectInfo::BuildObject()
	{
		pointer<Object> obj = ctor();

		obj->_info = this;

		return obj;
	}



	HMODULE GetCurrentModule()
	{ // NB: XP+ solution!
		HMODULE hModule = NULL;
		GetModuleHandleEx(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			(LPCTSTR)GetCurrentModule,
			&hModule);

		return hModule;
	}


	void NewProject()
	{

	}
	
	//Shit should probably use some const
	std::unique_ptr<ObjectInfo> NewObjectType(const std::type_info& type, std::string_view category, TypeOffset range, ObjectBuilder builder, HMODULE source)
	{
		IdentityManager::GenerateID(category, range);

		std::unique_ptr<ObjectInfo> info = std::make_unique<ObjectInfo>();

		info->type = &type;
		info->index = IdentityManager::GetIndexFromName(category);
		info->program = source;
		info->offset = 0;
		info->ctor = builder;

		return info;
	}

	template <std::derived_from<Object> T>
	std::unique_ptr<ObjectInfo> NewObjectType(std::string_view category, TypeOffset range = 0)
	{
		const std::type_info& type = typeid(T);


		HMODULE source = GetCurrentModule();
		
		//This should probably go in an non-template function to reduce the amount of times something like this is created.
		ObjectBuilder builder = []() -> pointer<Object> {
			return { new T() };
		};

		return NewObjectType(type, category, range, builder, source);
	}

}


namespace Banishment
{
	/*

	namespace
	{
		//I think I'll actually be putting the external interface IN external. Nah, that would increase size. But 
		// It should contain a type alias to it's current interface type as convience.

		struct External;

		using ExternalCtor = External * (*)();

		namespace detail
		{
			struct ExternalInterface
			{
				virtual ~ExternalInterface() = default;

				enum struct Version
				{
					V1,

					Current = V1
				};

				enum struct Update
				{
					Invalid,//Query is invalid
					Match,//Update versions match
					Library,//Library is out of date
					Engine//Engine is out of date

				};

			};
		}

		namespace Version1
		{
			//This answers questions
			struct ExternalInterface : public detail::ExternalInterface
			{

				//Largely, this doesn't need to be changed between implementations, as someone should always be
				// using Current, and whatever version they're on will sort it out.
				virtual Version GetVersion() final { return Version::Current; }

				//For all functions within the external interface, I could possibly make it so there are rules on
				// operating on new interfaces and such. This may allow one to use newer plugins with older engines.
				// however, this is likely, a bad idea.


				//The Non-virtual Functions

				//This call will use the version given from the 
				Update CheckUpdate()
				{
					//Likely will rename, this function also might be moved to a more static environment.
					if (!this)
						return Update::Invalid;

					auto version = GetVersion();

					if (version == Version::Current)
						return Update::Match;
					else if ((int)version < (int)Version::Current)
						return Update::Library;

					else
						return Update::Engine;

				}
			};
		}

		using ExternalInterface = Version1::ExternalInterface;


	}

	struct ExternalInterfaceImpl : public LEX::ExternalInterface
	{
		//Basically never needs to occur
		//ExternalInterface::Version GetVersion() override { return ExternalInterface::Version::Current; }


		//This contains the original type_info and module the object came from.
		Origin _origin{};

		//To all things that will submit and try to request an object that's submitted, this is what will handle
		// different interpretations of that object. (Such as being in a different namespace, a different version
		// or biggest of all, SKSE's TESForm vs RE::TESForm
		//May delegate this to the object manager.
		std::vector<std::string_view> _sourceNames;

		//Unsure if this would even be needed, but it would make the looking up a bit faster.
		//This should be as large as id offset.
		//^//std::vector<ITypePolicy*> _typeRange;

		//This function carried from another project is a constructor that allows an object to be able to
		// create new versions of itself, or allow one to request a new object merely by knowing its type even
		// if the object didn't come from that very project.
		ExternalCtor _ctor;


		//All ideas from the claimed id to  the end of the offset are a part of the registered type.
		// So think of from form all the way
		uint32_t claimedID;
		uint32_t idOffset;

	};


	struct External {
		virtual ~External() = default;

		//This is the core function of the object type. In it's implementation,
		virtual void Invoke(std::string_view func_name, Variable& ret, ArgContainer<Variable>& args)
		{
			//The above should be const.

			//Extra point should go toward notifying one that this literaly is unimplemented.
			// A specific function should be the thing that does this btw.
			"No implementation of [Put type name here]::Invoke exists. ";
			throw nullptr;

		}

		virtual constexpr const std::type_info& GetTypeInfo() const
		{
			return typeid(*this);
		}

		virtual constexpr std::string_view GetName()
		{
			//This cannot work properly for the const version. So I may make 2 versions

			//Returns the top level name of this polymorphic type. Can be changed to get the current name
			// in order to have a central version be the registered type.
			return typeid(*this).name();
		}

		//Need to make sure they're of the same type.
		virtual External& operator=(const External& a_rhs) = 0;

		virtual std::strong_ordering operator<=>(const External& a_rhs) = 0;




		template <typename Self>
		constexpr std::string_view GetCurrentName(this Self&& self)
		{
			//The name of the type this was called at, regardless of what it inherits.
			return typeid(Self).name();
		}

		template <std::derived_from<External> Self>
		constexpr Self& MatchType(this Self&& self, const External& a_rhs)
		{
			//Also maybe use that type name implementation if you can. Just so different versions don't get caught on each other.

			if (_registry != a_rhs._registry)
			{
				//Types were not matched. Reporting failure.
				//Complete and utter failure.
			}

			return static_cast<Self&>(a_rhs);
		}


		External()
		{

			_registry = ExternalManager::GetInterface(std::string{ GetName() });

			//I'mma do this differently else where. Rather, I want to return an API result.
			if (!_registry) {
				"Object Type is unregistered or malformed. Confirm registration before use.";
				throw nullptr;
			}
		}

		//Basically, the object's job is to find the type.
		virtual AbstractTypePolicy* GetType() = 0;



		//This would be a virtual function that is required to be implemented by the derived class.
		//virtual TypeCode TypeCodeOf() = 0;

		ExternalInterface* _registry = nullptr;
	};

	//*/
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




//TODO: Most if not all todos past this point are invalid



//This shouldn't be deleted, has some valuable things within.
/*
namespace TestField__Types
{

	//Just a note about parsing now, I think I want to move away from toml, for a bit, but have an area specifically for toml, like once it's done, it's done.
	//I additionally want some stuff that can signify special roles, like

	//#use DEBUG
	//or
	//#use CASE SENSETIVE
	//Another one,
	//#use return <Type>, this will set the default type for functions in the project. So making something just 
	// function(){} in someting like AVG will automatically be a float. In other projects that are more about doing, it will likely be void.
	//#use this <Type> could be another idea, setting the default call from function, which is normally determined via other methods.

	//Lastly, a thing that I could do, is have certain sections of code only loaded if another file is detected


	struct Void {};

	//Just a fucking example
	using ObjectVar = std::variant<Void, void*, int*, char*>;

	//TODO:struct is called Construct
	struct Struct;


	//This is generally the type of setup I'm looking to have.
#define TYPES_MACRO void*
#define HEAD_TYPES_MACRO 1
	//This might not work though, because here's something possibly stupid. What if someone has 2 different APIs like ToolUI set up, 
	// and actor value generator both.
	using TruerObjectVar = std::variant<
		Void,
		TYPES_MACRO,
		Struct			//No matter where this index is, the outputted region should always be 7F if regular, and FF ir array.
	>;


	//I think it would possibly be for the best that the variant is exposed as something that can be mallible as a template, but the hard cast type when working
	// in it from this side. 
	//The idea would be to allow me to have the API version of API to apply different types (which you'd juse like AVG::Object or TUI::Object).

	//I can probably do something like this, and have which one makes it as the "defacto" type macro. So it will check if it's own source is enabled
	// and if not, it doesn't set a "core". Most importantly, if no LEX source is used, it's not relevant.

	//So something like this

	template <class... Ts>
	using APIObject = int;


	//This goes in lexicon, given it's  macros and the idea of types macro is made there first (designed to be overwritten or perhaps pushed)
#define TYPES() Void, TYPES_MACRO, Struct
#define HEAD_TYPES() Void, HEAD_TYPES_MACRO, Struct
	//This goes in XXX
#define TYPES_MACRO std::string, int, void*, float
	//This too goes in XXX, which looks for it's own source value.
#ifndef IS_XXX_SOURCE
	//HEAD_TYPES is what the actual LEX::Object class uses to populate it's values.
#define HEAD_TYPES_MACRO TYPES_MACRO
#endif

	namespace XXX
	{

		//Using this version of object is vastly different than using the actual version of object. What this version will do is send you
		// an index and a void pointer. The point being that depending on what API version you're requesting from type you're requesting from will cause it to be different.
		// additionally, to tell the difference between which API is supposed to be gotten, I think it would possibly be best to seperate uses of the API source by
		// templates or something. Because only one API use wins, and getting and storing the location of them all might be a pain.
		//So make it so
		using Object = APIObject<TYPES()>;
	}


	//The more and more I see this, the more I think it might be prudent to make lexicon redistributables. These redistributables would store the main "OBJECT"
	// and a lot of the virtual machine related behaviour.
	//Then the individual plugins can have their own implementations of Object

	//Alright, so that idea is fucking insane, instead, include the API inside of a namespace. DO NOT USE RE namespace, that shit will fuck some stuff up,
	// but other than that, this will allow us to include the same API twice. As long as you gate it with a namespace. The large part is, don't use the API directly
	// I'll need to convey that.

	//Also, important note. No pragma once, so I can include it in the namespace. To take up some less space, I can make some common API headers
	// that contain my ABIContainer and such.

	//If someone to use it directly, should make something for that.
	
#undef TYPES_MACRO



	






	//A specialization like this is required when
	// LookingUp an object to which a lookup can be invalid
	// when getting the LEX::ObjectID of a given object
	//
	template<uint8_t N> struct speck
	{
		virtual LEX::ObjectID GetType(ObjectVar& object)
		{
			return {};
		}
	};




	template<> struct speck<0> {
		//Speck zero is invalid

		virtual LEX::ObjectID GetType(ObjectVar& object)
		{
			//Report that there's no valid specialization of the type here.
			return {};
		}
	};

	//needs a class above it.
	//Also, may make a macro for this, just so all the classes conform properly.
	//Main reason for the macro, is it would be nice to just use the type to get the number. I guess all this is fine. For now.
	template<> struct speck<1>
	{
		//using DefinedClass = T;

		//Should return only the 7 bits that matter.
		LEX::ObjectID operator()(ObjectVar& object)
		{
			return GetType(object);
		}

		LEX::ObjectID GetType(ObjectVar& object)
		{
			//Report that there's no valid specialization of the type here.
			return {};
		}
	};
S
	template <uint8_t I>
	struct speck_user : public speck<I>
	{
		//Should return only the 7 bits that matter.
		static LEX::ObjectID TESTType(ObjectVar& object)
		{
			speck<I> speck;

			return speck(object);

			return GetType(object);
		}
	};
	



	namespace RE
	{
		using TESForm = int;
	}



	template <int I, int Max>
	LEX::ObjectID _GetType(ObjectVar& obj, uint8_t index)
	{
		if (index != I)
		{

			if constexpr (I != Max)
				return _GetType<I + 1, Max>(obj, index);

			return LEX::ObjectID::Invalid();
		}

		using ClassType = speck<I>;

		speck<uint8_t(I)> speck;

		return speck.GetType(object);
	}

	LEX::ObjectID _GetType(ObjectVar obj)
	{
		uint8_t index = obj.index();

		if (!index)
			return LEX::ObjectID::Invalid();

		if (index == structIndex)
		{
			//Get obj as a struct
			//Get prototype
			//Get Type ID
			return LEX::ObjectID::Invalid();
		}

		
		//Depending on what the index is, I can give it certain starting points. Kinda shortens the searching I guess.

		if (index < 128)
			return _GetType<0, 127>(obj, index);
		else
			return _GetType<128, 255>(obj, index);
	}

	//So the idea with object type ids

	//Having conversion functions might be interesting.
	// Like say for forms and strings. Explicitly used when 

	
	
}
//*/

//This is the old version, I'm gonna just be playing it off the guide for now.
/*
namespace LEX
{
	//The toml handler will have a function map, that handles different types.
	//Of it, it handles 2 sorts things on 2 different inputs. The name, or the member called
	// type.

	//It can probably store string hashes, as nothing about this needs to be stored specifically.
	// 

	



	enum struct ParseQueryResult
	{
		No,
		Yes,
		Cancel
	};


	struct DataCollection
	{
		RecordDataIterator& it;
		RecordDataIterator begin;
		RecordDataIterator last;
		RecordDataIterator end;


		DataCollection(RecordDataIterator& a_it, RecordDataIterator a_end) :
			it(a_it),
			begin(a_it),
			end(a_end)
		{}

		DataCollection(RecordDataIterator& a_it, RecordDataIterator a_begin, RecordDataIterator a_last,  RecordDataIterator a_end) :
			it(a_it),
			begin(a_begin),
			last(a_last),
			end(a_end)
		{}
	};



	struct Parser;

	//TODO:Extend functionality of this a bit, I'd like this to keep record of the syntax's that parse this.
	// For example what if an '=' is detected where arguments go? Syntax should be able to beget syntax, but it should know where
	// such things came from.
	using ParseVisitor = ParseQueryResult(*)(Parser*, Record*, DataCollection);

	ENUM(ParseType)
	{
		Keyword,
		Syntax,
		Error,
		Total
	};

	struct Syntax
	{
		struct Sort
		{
			constexpr bool operator()(const Syntax& lhs, const Syntax& rhs) const
			{
				if (lhs.type != rhs.type)
					return lhs.type < rhs.type;

				return lhs.GetPriority() < rhs.GetPriority();
			}

		};

		ParseVisitor _func;

		std::string_view category;

		bool invert = false;//If invert it will only work if the choice category is not within.
		
		//Invert should be the sign bit. Make getting priority a function.
		int16_t _priority = 0;

		int16_t GetPriority() const
		{
			constexpr int16_t sign_bit = ~(1 << 15);
			return _priority & sign_bit;
		}

		bool IsInverted() const
		{
			return _priority & (1 << 15);
		}

		ParseType type = ParseType::Syntax;

		//ParseQueryResult operator()(Parser* parser, Record* record, DataCollection collection)
		//{
		//	_func(parser, record, collection);
		//}
	};

	

	//Entries are sorted by type, and if equal, by priority
	//TODO: Rename Syntax. The operator handler is the actual syntax. This is just the tokenizer?
	std::set<Syntax, Syntax::Sort> syntaxList;
	

	//Known 



	struct Parser
	{
		//virtual ~Parser() = default;//Required why?

		//Note, for memory, adding a character at the front might be a good idea to differentiate.
		std::list<std::string> memory;
		
		//If the context of a syntax is not found within the context of the parser, it will not be evaluated to run.
		// A possible setting would be the inversion of this.
		std::multiset<std::string_view> context;
		//Expandable context might be a good idea. Like to say, we are in a function right now. And have it seperate so it doesn't clash or something.
		//std::multiset<std::string_view> expandingContext;
		Record* _target;


		//TODO: Make data collection struct a function instead of a parameter. Not all types will need the set up. After, just have gets for each one. Status: Easy
		// Additionally, it can be something within collection that does it, lastly, collection can be a template class.

		RecordDataIterator it;	//The current iterator
		RecordDataIterator last;//The iterator parsed at the start of the last process
		RecordDataIterator begin;//Where it's started
		RecordDataIterator end;	//Where it will no longer be valid.

		const Syntax* lastSyntax = nullptr;

		Record& GetTarget()
		{
			return *_target;
		}


		//The might replace crew
		bool SyntaxMatch(const Syntax* syntax)
		{
			if (!syntax)
				return false;

			auto end = context.end();

			return std::find(context.begin(), end, syntax->category) != end;
		}
		bool SyntaxMatch(const Syntax& syntax)
		{
			return SyntaxMatch(&syntax);
		}

		void SyntaxHandle()
		{
			//Above this, needs to be a parsing function of it's own.
			for (auto& syntax : syntaxList)
			{
				if (SyntaxMatch(syntax) == false) {
					continue;
				}
				//TODO: Address, should the collection for syntax(? Or Tokenizer?) be able to push the actual parsing iterator?
				DataCollection collection{ it, begin, last, end };

				auto result = syntax._func(this, _target, collection);

				//Check validity?

				switch (result)
				{
				case ParseQueryResult::Yes://It did meet requirements. No more parsing.
					lastSyntax = &syntax;
					return;
				case ParseQueryResult::No://Means didn't match requirements.
					continue;
				case ParseQueryResult::Cancel://It met requirements but an error occured. Going to error parsing.
					//TODO: Make parsing error from cancel bail to the error section instead.
					throw ParsingError("Replace");
				}
			}
		}

		void HandleData(Record& tar, std::list<std::string> l_mem, std::multiset<std::string_view> txt)
		{
			//I'm just gonna manually visit for now. I sorta wish that I could do this in some other way.
			//TODO: suggestion, maybe I should use the actual visitor.

			//I want something to deal with this parser
			//{ DataType::Invalid, OperatorType::Invalid };
			//_lhs.second = OperatorType::Invalid;
			while (it != end) {
				auto old_it = it;

				SyntaxHandle();

				last = old_it;

				//If it didn't move, that's kinda an issue. Should this then throw?
				if (old_it == it) it++;
			}

		}

		template<class... Ts>requires(RGL::AllTrue<std::convertible_to<Ts, std::string_view>...>::value && sizeof...(Ts) > 0)
		static void Handle(Record& tar, Ts... txts)
		{

			Parser parser{};

			parser.HandleData(tar, {}, std::multiset<std::string_view>{txts...});
			
		}

		template<class... Ts>requires(RGL::AllTrue<std::convertible_to<Ts, std::string_view>...>::value && sizeof...(Ts) > 0)
		static void Handle(Record& tar, std::list<std::string>& l_mem, Ts... txts)
		{
			Parser parser{};

			parser.HandleData(tar, l_mem, std::multiset<std::string_view>{txts...});

			l_mem = parser.memory;
		}


	};


	


	//Now, I need to figure out if I want to visit all the syntax objects, how would I exactly go about doing such a thing?


	constexpr std::string_view routine_regex = "(?<Minus>- )|(?<Object><:.+(?=:>)..)|(?<Digits>[\\d\\.]+)|(?<Quotes>'[^']+.)|(?<Identifiers>(\\w|::)+)|(?<Symbols>[^\\w\\d\\s'\\.]([^\\w\\d\\s'\\.:]){0,})";
	constexpr std::string_view seq_routine_regex = "(?<Header>^[\\w;]{0,}\\s{0,}(?=;).)|(?<Minus>- )|(?<Object><:.+(?=:>)..)|(?<Digits>[\\d\\.]+)|(?<Quotes>'[^']+.)|(?<Identifiers>(\\w|::)+)|(?<Symbols>[^\\w\\d\\s'\\.]([^\\w\\d\\s'\\.:]){0,})";
	//May need revisions due to the ; possibly being caught first.
	constexpr std::string_view property_regex = "(?<Tooltip>;.+)|(?<DefValue>(?<=:)\\s{0,}\\K.[^;\n]+)|(?<Type>\\w+\\s{0,}:)|(?<RawValue>(\\d|'|<:)[^\\/\\/\\n]+)|(?<Identifier>\\w+)";

	namespace
	{
		using namespace RGL;

		void pure_test()
		{

			std::string func = "GetActorValue('SingleWielding', Damage | Permanent) - GetActorValue('SingleWielding', Damage | Permanent)";
			//std::string func = "label_1; GetActorValue(::ending = 'SingleWielding', ActorValue::Damage | ActorValue::Permanent); return result";

			RGL::Record function("Commons");
			function.CreateData("GetWieldingWeight");
			//I think I'll use hex for the code, just in case it grows that much in size. Keeps the space usage down.
			Record& code_record = function.CreateChild("@code0000");

			code_record.CreateData(func, routine_regex);
			//code_record.CreateData(func, seq_routine_regex);//This is what would be used if it was an array instead of a string.

			Record& params_required = function.CreateChild("params_required");

			////Would that this had parameters, this is what it would do.
			////This data represents the number of regular parameters
			//params_required.CreateData("1");
			//params_required.CreateChild("param1OrWhateverTheNameIs").CreateData("", property_regex);
			//Record& params_default = function.CreateChild("params_default");
			////This data represents the number of default parameters
			//params_default.CreateData("1");
			//params_default.CreateChild("param2OrWhateverTheNameIs").CreateData("", property_regex);

		}
	}


	//Should the goal be to produce a record, or produce a thing?
	// For now, nothing.
	
	void HandleRecord_Routine(std::string name, const FileNode& node)
	{
		//TODO: Todos for the toml handling function below. Status: Undetermined
		// I think I would prefer it if the file was made before we get to the file node personally.
		
		//First things this should do, make sure the object name can go somewhere, make sure it's not reserved, then make sure that that
		// the object is actually a table, and we are a go.
		auto& table = *node.as_table();
		

		Record& routine = FileManager::EmplaceFileRecord("TempSpot", name);
		//Right here in data, the first object will be the scope name of the record.


		std::string formula_code = table["formula"].value_or("0");

		std::list <std::string> memory{};

		auto param_view = table["params"];

		
		if (auto p_test = param_view.as_array(); p_test)
		{
			Record& req_params = routine.CreateChild("parameters_required");
				
			//If def params exist, that's where things must go, and if they don't that's a parsing error.
			Record* def_params = nullptr;

			auto& param_array = *p_test;
				

			for (auto& entry : param_array)
			{
				//Right here is the place for that handle multiple entries deally, BUT, this is not where it will be handling that properly.
				if (entry.is_string() == false) {
					throw ParsingError("parameters only accept string types.");
				}

				std::string str = entry.value_or("");

				//May create a static function to do this, might get messy.
				Record parameter{ str, property_regex.data() };

				

				//Visit right here.
				Parser::Handle(parameter, memory, "Function");

				if (parameter.FindData("\\w+:") != nullptr)
				{
					if (!def_params)
						def_params = &routine.CreateChild("parameters_default");

					def_params->PushBackChild(parameter);
				}
				else
				{
					if (!def_params)
						req_params.PushBackChild(parameter);
					else
						//ROMBUR: Needs to submit to error names.
						throw ParsingError("non-default declared after default params in... something");
				}
			}

		}

		if (table["default"])
		{
			//This does different things based on what type this is, so I'm gonna put this to the side. For now.
			//formula->defaultValue = table["default"].value_or(0);
		}
			
		//Code is stored in a record called code_body. This can range from having only one entry. The group record will hold the needed
		// data to tell whether it's a single line or sequence, with such data being held in it's tag as a boolean.

		//TODO: Parser needs to force the differences between native and non-native code.
		if (auto code = table["formula"]; code)
		{
			Record& body = routine.CreateChild("code_body");
			
			std::function list_handle = [&](toml::array list)
			{
				body.CreateData("+");
				
				std::vector<std::string> code_list{ list.size() };

				std::transform(list.begin(), list.end(), code_list.begin(),
					[](toml::node& entry) 
					{ 
						if (entry.is_string() == false)
							throw ParsingError("Code body within array must only use toml strings.");
						//Could do this another way?
						return entry.as_string()->get();
					}
				);

				body.CreateChildren("code_", code_list, seq_routine_regex.data());
					
				//body.GetChildFirst()->GetTag() = "code_entry";
				body.GetChildBack()->GetTag() = "code_return";//Only return needs to be named like this.

				//visit children right here.
				//Keeping in case.
				//for (auto& entry : list)
				//{
				//	//Right here is the place for that handle multiple entries deally, BUT, this is not where it will be handling that properly.
				//	if (entry.is_string() == false) {
				//		throw ParsingError("Code body within array must only use toml strings.");
				//	}
				//	//Need a function to do this specically.
				//		
				//	body.PushBackChild(Record{ entry.value_or(""), seq_routine_regex.data() });
				//}
			};

			std::function string_handle = [&](std::string str)
			{
				body.CreateData("-");
				//Name doesn't matter here.
				body.CreateChildWithData("", str, seq_routine_regex.data());
			};

			HandleNodeView("No name currently", code,
				list_handle,
				string_handle
			);
			//Why this is like this, I have no idea. I
			auto test = [&](Record* visitee, RecordIterator& iterator, RecordIterator end)
			{
				//It's notable that right here, this is where something would use memory
				Parser::Handle(*iterator, memory, "Function");
			};

			std::function func = test;
			GenericVisitor c_body_visitor{ func, nullptr };

			body.VisitChildren(&c_body_visitor);
		}
			
		//auto* flag_array = table["flags"].as_array();
		//if (flag_array)
		//{
		//	auto result = std::find_if(flag_array->cbegin(), flag_array->cend(), [](auto& it) {return it.value_or("") == "native"; });
		//	if (flag_array->end() == result)
		//		formula = CreateFormula(parameters);
		//}


		//End of the record.

		//At this point, the toml has been processed, now what needs to happen is the creation of an object that can handle a record.
		//This type needs to be able to be given a record, and construct itself from the data given. So for example.

		//auto routine = new Coroutine();
		//routine->HandleRecords(routine);

		//And then that would be the end of it.


		
	}
}
//*/

//Disabled for now.
//#undef TOKEN
//#undef SYNTAX