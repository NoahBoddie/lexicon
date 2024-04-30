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

		int foo(const TypeA1&)
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
			const TypeA a;
			TypeB b;
			TypeC c;
			//foo(a);
			//First: arg1 par 3, arg2 par 3 (or actual 5 if we consider multiple inherit)
			//Second call: arg1 par 7, arg2 par 4

			//foo(c);

			//bar(c);
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


	
	namespace Fields
	{
		using Specifier = DeclareSpecifier;

		enum struct FieldType
		{
			Invalid,
			Local,
			Parameter,
			Variable,
			Function,//Doesn't differentiate between method or function
		};

		//Both global data and a FunctionData need a bare bones version that sits without things that would be included in the field.
		// Kinda hard to do, given that one of these things comes with the qualified type.

		//Noting, that it seems almost as if the qualified data does not actually seem to get used when it comes down to membered fields.
		// Like, that sort of information would be stored no where for the function.

		struct GlobalData
		{
			std::string _name;

			ICallableUnit* _init = nullptr;//This is the default for a given global. Call to reset value. But, do note, this should be a formula, not a callable unit.
			Variable value{};//This is the current value of the global. Should be assigned a type and that type should not change.
		};

		namespace OLD
		{

			struct Field
			{
			public:
				~Field() = default;

				//These last 2 are sorta not really important. TypePolicy is however.
				virtual FieldType GetFieldType() const = 0;// { return FieldType::Invalid; }

				virtual size_t GetFieldIndex() const { return max_value<size_t>; }

				virtual ITypePolicy* GetTypePolicy() const { return nullptr; }

				//TODO: Make Field::AsSolution virtual
				Solution AsSolution();


				virtual Qualifier GetQualifiers() const = 0;

				QualifiedType GetQualifiedType() const;

				bool IsLocal() const { return GetFieldType() == FieldType::Local; }
			};


			struct FunctionInfo : public MemberInfo
			{

				//TODO: This needs to be able to handle overloads some how, via abstract functions maybe?


				struct Settings : public MemberInfo::Settings
				{
					bool isVirtual = false;
				};

				using FunctionType = FunctionBase;//


				union {
					FunctionType* function = nullptr;
					MemberPointer method;//prefered, works with the other.
				};

				bool IsVirtual() const
				{
					return GetData<Settings>().isVirtual;
				}

				FunctionType* Get()
				{
					if (IsVirtual() == false)
						return function;

					return nullptr;
				}

				FieldType GetFieldType() const override { return FieldType::Function; }


				FunctionInfo() = default;
			};


			struct InfoBase : public Field
			{
			public:
				struct Settings
				{
					Qualifier flags{};
				};



				InfoBase() = default;

				std::string name{};

				//Instead, you can just store basic qualfiers here. Percieve them any way you'd like.
				// it's upto the derived.
				size_t _data{};



				//Mainly used for members and function variables. But since I've yet to see anything that remotely 
				// outside these 2 uses, I think this suits just fine.
				size_t index = max_value<size_t>;

			protected:



				template <typename T> requires (sizeof(T) <= 0x8)
					T& DataAs()
				{
					return reinterpret_cast<T&>(_data);
				}

				template <typename T> requires (sizeof(T) <= 0x8)
					const T& DataAs() const
				{
					return reinterpret_cast<const T&>(_data);
				}

				template <typename T> requires (sizeof(T) <= 0x8)
					T GetData() const
				{
					return DataAs<T>();
				}

			public:


				size_t GetFieldIndex() const override
				{
					return max_value<size_t>;
				}


				virtual operator bool() const
				{
					//Should this be and?
					return name != "" || _data;
				}
			};




			class MemberInfo : public InfoBase
			{
			public:
				MemberInfo() = default;

				MemberInfo(size_t i) {}


				struct Settings : public InfoBase::Settings
				{
					bool isRuntimeMember = false;
				};

				virtual Qualifier GetQualifiers() const override
				{
					auto setting = GetData<Settings>();

					if (!setting.isRuntimeMember)
						setting.flags = setting.flags & ~Qualifier::Runtime;

					return GetData<Settings>().flags;
				}

				/*
				virtual BasicQualifier GetBasicFlags() const
				{
					return GetData<Settings>().basic;
				}

				virtual RuntimeQualifier GetRuntimeFlags() const
				{
					if (auto setting = GetData<Settings>(); setting.isRuntimeMember)
						return setting.runtime;

					return RuntimeQualifier::None;
				}
				//*/
			protected:
				AccessModifier _access = AccessModifier::Private;

			};



			class LocalInfo : public TemporaryInfo
			{
			public:
				LocalInfo(ITypePolicy* t, size_t i) : TemporaryInfo{ i }, _type{ t }
				{

				}

				FieldType GetFieldType() const override { return FieldType::Local; }

				ITypePolicy* GetTypePolicy() const override { return _type; }

			protected:
				ITypePolicy* _type = nullptr;



				operator bool() const override
				{
					//Should this be and?
					return _type || __super::operator bool();
				}
			};


			//Delete
			class TemporaryInfo : public InfoBase
			{
			public:
				//I don't think I need this, may merge into local info.
				TemporaryInfo(size_t i) : _index{ i } {}



				virtual Qualifier GetQualifiers() const override
				{
					return GetData<Settings>().flags;
				}

			protected:
				size_t _index = max_value<size_t>;

			};

			class ParameterInfo : public LocalInfo
			{
				struct Settings : public LocalInfo::Settings
				{
					bool isDefault = false;
				};


			public:
				ParameterInfo(ITypePolicy* t, size_t i, std::string n) : LocalInfo{ t, i }
				{
					name = n;
				}

				ParameterInfo(QualifiedType t, size_t i, std::string n) : LocalInfo{ t, i }
				{
					name = n;

					DataAs<Settings>().flags = t.flags;
				}
			};


			//Needs?
			struct VariableInfo : public MemberInfo
			{
				VariableInfo() = default;

				//no qualifiers for now.
				VariableInfo(ITypePolicy* t, size_t i, Qualifier q) : _index{ i }, _type{ t }
				{
					DataAs<Settings>().flags = q;
				}

				size_t GetFieldIndex() const override
				{
					return _index;
				}

				FieldType GetFieldType() const override { return FieldType::Variable; }

				ITypePolicy* GetTypePolicy() const override
				{
					return _type;
				}


				ITypePolicy* _type = nullptr;
				size_t _index = max_value<size_t>;
			};


		}


		struct Field
		{
			//The interface for fields
			virtual ~Field() = default;

			virtual String GetFieldName() const = 0;
			
			virtual FieldType GetFieldType() const = 0;

			virtual uint32_t GetFieldIndex() const = 0;


			virtual Qualifier GetQualifiers() const = 0;

			virtual Specifier GetSpecifiers() const = 0;

			virtual ITypePolicy* GetType() const = 0;


		};
		


		struct InfoBase : public Field
		{
			//The abstract class for infos. Holds all the data for it.

			Qualifier qualifiers;
			
			DeclareSpecifier specifiers;

			//Access modifiers and 


			//bool isVirtual = 0;
			//bool isIsRuntimeMember = 0;//This is actually used to tell if it should purge runtime stuff from members. I easily could do that.
			

			//All versions will need this at some point, so I'll do this here.
			uint32_t _index = -1;

			uint32_t GetFieldIndex() const override
			{
				return _index;
			}
			

			virtual operator bool() const 
			{
				//This was supposed to do something, no idea what it was.
				return true;
			}
		};






		class LocalInfo : public InfoBase
		{
		public:
			//LocalInfo(ITypePolicy* t, size_t i) : TemporaryInfo{ i }, _type{ t }
			//{
			//}

			FieldType GetFieldType() const override { return FieldType::Local; }

			ITypePolicy* GetType() const override { return _type; }

			Qualifier GetQualifiers() const override { return qualifiers; }
			Specifier GetSpecifiers() const override { return specifiers; }

			std::string GetFieldName() const override { return _name; }


		protected:
			std::string _name;
			
			ITypePolicy* _type = nullptr;



			operator bool() const override
			{
				//Should this be and?
				return _type || __super::operator bool();
			}
		};


		class ParameterInfo : public LocalInfo
		{
		public:
			//ParameterInfo(ITypePolicy* t, size_t i, std::string n) : LocalInfo{ t, i }
			//{
			//	name = n;
			//}
			//ParameterInfo(QualifiedType t, size_t i, std::string n) : LocalInfo{ t, i }
			//{
			//	name = n;
			//	DataAs<Settings>().flags = t.flags;
			//}


			FieldType GetFieldType() const override { return FieldType::Parameter; }

			//Should contain information like default info etc.
		};


		struct MemberInfo :public InfoBase
		{
			//This pivot is still going to need to exist for all the reasons that you'd imagine something like this would need. They sorta need to play proxy to the
			// specializations of both functions and for variables, so first this will need overloads, and each of those will need their respective specialization.


			bool IsStatic() const
			{
				return specifiers & Specifier::Static;
			}
		};


		struct FunctionInfo : public MemberInfo//Needs to be able to use overloads, so these can possibly be a bit bigger.
		{
			using FunctionType = FunctionBase;//


			union {
				std::array<size_t, 2> _raw{0 , 0};


				FunctionType* function;
				
				struct
				{
					MemberPointer method;//prefered, works with the other.
					FunctionData* signature;
				};
			};

			bool IsVirtual() const
			{
				return specifiers & Specifier::Virtual;
			}

			FunctionType* Get() const
			{
				if (IsVirtual() == false)
					return function;

				return nullptr;
			}

			FieldType GetFieldType() const override { return FieldType::Function; }

			//For now, this is true, there is no way to handle a function's type.
			ITypePolicy* GetType() const override { return nullptr; }

			Qualifier GetQualifiers() const override { return qualifiers; }
			Specifier GetSpecifiers() const override { return specifiers; }

			std::string GetFieldName() const override { return Get()->GetName(); }


			
		};

		struct VariableInfo : public MemberInfo
		{
			//no qualifiers for now.
			//VariableInfo(ITypePolicy* t, size_t i, Qualifier q) : _index{ i }, _type{ t }
			//{
			//	DataAs<Settings>().flags = q;
			//}


			FieldType GetFieldType() const override { return FieldType::Variable; }
			
			ITypePolicy* GetType() const override { return nullptr; }

			Qualifier GetQualifiers() const override { return qualifiers; }
			Specifier GetSpecifiers() const override { return specifiers; }

			std::string GetFieldName() const override { return Get()->GetName(); }


			union {
				std::array<size_t, 2> _raw{ 0 , 0 };

				//This would be a GlobalBase, rather than a global.
				Global* variable;

				struct
				{
					MemberPointer method;//prefered, works with the other.
					GlobalData* signature;
				};
			};

			Global* Get() const
			{
				if (IsStatic() == true)
					return variable;

				return nullptr;
			}
		};


		
		struct QualifiedField : public Field
		{
			enum struct Constness : uint8_t
			{
				kNone,
				kConst,
				kReadonly,
			};

			enum struct RefrType : uint8_t
			{
				kNone,
				kLeft,
				kRight,
			};

			//The qualified type could be anything qualifiable, but type seems to be the most likely to pull (well solution, but solutions are type qualified).
			QualifiedField(InfoBase* field, QualifiedType type)
			{
				//By this point, it's expected that the calling has already emplaced it's rules on what this is.


				auto qualifiers = field->GetQualifiers();

				switch (type.flags & Qualifier::Constness_)
				{
				case Qualifier::Const:
					//If the field is mutable or readonly, it cannot be made into const.
				if (FilterEquals<Qualifier::Constness_>(qualifiers, Qualifier::Modable) == false)
					_constLevel = Constness::kConst; break;

				case Qualifier::Readonly:
					_constLevel = Constness::kReadonly; break;
				}

				//If the field has a reference type, that is uses instead.
				if (!!(qualifiers & Qualifier::Reference_))
				{
					switch (type.flags & Qualifier::Reference_)
					{
					case Qualifier::RefL:
						_refLevel = RefrType::kLeft; break;

					case Qualifier::RefR:
						_refLevel = RefrType::kRight; break;
					}
				}
				
			}

			InfoBase* _target = nullptr;

			Constness _constLevel = Constness::kNone;
			
			//Thi
			RefrType _refLevel = RefrType::kNone;
			


			uint32_t GetFieldIndex() const override
			{
				return _target->GetFieldIndex();
			}

			FieldType GetFieldType() const override
			{
				return _target->GetFieldType();
			}

			ITypePolicy* GetType() const override
			{
				return _target->GetType();
			}

			Qualifier GetRawQualifiers() const
			{
				return _target->GetQualifiers();
			}

			Qualifier GetQualifiers() const override
			{
				auto result = GetRawQualifiers();
				

				if (_constLevel != Constness::kNone)
				{
					result &= ~Qualifier::Constness_;

					switch (_constLevel)
					{
					case Constness::kConst:
						result |= Qualifier::Const; break;

					case Constness::kReadonly:
						result |= Qualifier::Readonly; break;
					}

				}


				if (_refLevel != RefrType::kNone)
				{
					result &= ~Qualifier::Reference_;

					switch (_refLevel)
					{
					case RefrType::kLeft:
						result |= Qualifier::RefL; break;

					case RefrType::kRight:
						result |= Qualifier::RefR; break;
					}

				}


				return result;
			}
			Specifier GetSpecifiers() const override
			{
				return _target->GetSpecifiers();
			}

			std::string GetFieldName() const override
			{
				return _target->GetFieldName();
			}

			operator Field* () const
			{
				return _target;
			}

			operator bool() const
			{
				return _target;
			}
		};



		enum struct ScopeType : uint8_t
		{
			Required,		//It's a given that the parent scope will have to run the following when encountered
			Conditional,	//The parent scope may not run the contents of this scope when encountered
			Depedent,		//Whether the parent scope runs the contents of this scope are dependent on another scope.
			Header,			//The header scope, the only things that go here would be parameters and such. Cannot have a parent.

			
			//Header is the default scope,
			//Required is any scope that will be encountered (say {..})
			//Condition is stuff like ifs and while loops.
			//Dependent means it's reliant on a conditional to trigger. 
			// I'm considering something like trivial, say if something's set within an if statement.
		};


		struct Scope
		{
		private:
			enum ReturnType : uint8_t
			{
				None,		//There is no guarenteed return
				Nested,		//There is a return, through different scopes.
				Immutable	//There is a non conditional return interpreted. Cannot be overwritten.
			};
		public:

			//The generic object that handles the concept of a scope.

			~Scope()
			{
				//When the scope of the function using scope decays, it will set its parent scope back to being the main scope, as well as
				// adding a decrement value to the variable count.


				//There's just one problem. You see, it's that pesky fucking loose operation stack you see.  No way to add to it from destruction.

				//Okay, what if I do a best of both worlds. The return for operations are within the compiler, and are submitted to a buffer? Doesn't work for similar reasons.

				//Here's what I can do. There's a wrapper to the compile function calls, similar to try module and such



				_CheckExit();


				//if (!parent)//if it's top level, that instead decides if it even removes anything.
				if (IsTopLevel() == false)
					process->ModVarCount(-vars.size());

				process->currentScope = parent;



				//If stuff like out is to be used, I'll need


				//TODO: If I can, I'd like some optimization that collapses all variable increments
				// ^ perhaps such optimizations can be made here in post, with the ability to insert instructions at a certain position.
				// all I would need is the index of where this scope started, then once this ends I'd submit all the instructions
				// for creating them en masse, and each instruction to load the type policy onto the variable
			}


			[[nodiscard]] Scope(RoutineCompiler* compiler, ScopeType s) :process{ static_cast<RoutineCompiler*>(compiler) }, _type{ s }
			{
				//When created, it will set itself as the main scope, and record the old scope.

				//This needs to handle the return type so it can get a proper compiling error for not returning when required.

				parent = process->currentScope;
				process->currentScope = this;

				if (parent && s == ScopeType::Header) {
					report::compile::fatal("invalid header scope with parent detected.");
				}
			}



			LocalInfo* CreateVariable(std::string name, QualifiedType type)
			{
				//Should consider not using a pointer due to invalidation. Instead, maybe give a copy.

				assert(type.policy);

				bool header = IsHeader();

				//auto result = variables.size();

				//variables.push_back(name);

				if (auto& var = vars[name]; var) {
					report::compile::critical("Variable name already taken");
				}

				auto index = !header ? process->ModVarCount(type.policy) : process->ModParamCount(type.policy);


				RGL_LOG(debug, "Attempting to create {} at index {}", name, index);

				LocalInfo& result = vars[name] = LocalInfo{ type.policy, index, type.flags };


				//This is only temporarily valid.
				return &result;
			}

			//LocalInfo* GetLocalVariable(std::string& name)
			//VariableInfo* GetGlobalVariable(std::string& name);

			QualifiedField SearchField(std::string name, OverloadKey& key, FieldPredicate pred = nullptr);

			//I will abet this for now, but it dies some day
			QualifiedField SearchField(std::string name, FieldPredicate pred = nullptr);


			ITypePolicy* SearchType(std::string name);

			//If this is the scope that the routine uses to hold its top level data returns true.
			bool IsRoutine() const { return !parent; }

			bool IsHeader() const { return _type == ScopeType::Header && !parent; }

			bool IsTopLevel() const { return IsHeader() || (parent ? parent->_type == ScopeType::Header : false); }


			//As variables get declared, this list will grow in size.
			// Variable might need to be an object. If it is, I think mixing it with compiled operand maybe useful. Whatever this is, it would need qualifiers.
			// qualifiers that don't really exist at a later point, and just direct the compiler on how to handle things.
			//std::vector<std::string> variables{};

			std::map<std::string, LocalInfo> vars;

			//For use with stuff like in, out, and ref
			std::vector<Field*> assignments;


			//If scope has no parent, it will address the function data stored on the compiler, making it the chief scope.
			Scope* parent = nullptr;

			RoutineCompiler* process = nullptr;//Will turn statement compiler into a routinecompiler.


			const ScopeType _type;



			ReturnType _return{};

			std::string name();

			void _CheckExit()
			{

				if (IsHeader() == true) {

					if (!_return && process->GetReturnType()->FetchTypeID() != -1)
						report::compile::fatal("Explicit return expected. {}", name());

					return;
				}

				switch (_type)
				{
				case ScopeType::Header:
					break;

				case ScopeType::Required:
					if (_return)
						parent->_ConfirmExit();

					break;

				case ScopeType::Conditional:
					if (_return)
						parent->_FlagExit();
					else
						parent->_UnflagExit();

					break;

				case ScopeType::Depedent:
					if (!_return)
						parent->_UnflagExit();

					break;
				}
			}

			void _FlagExit()
			{

				if (_return != Immutable)
					_return = ReturnType::Nested;
			}

			void _UnflagExit()
			{
				if (_return != Immutable)
					_return = ReturnType::None;
			}

			void _ConfirmExit()
			{
				_return = ReturnType::Immutable;
			}


			void FlagReturn()
			{
				_return = ReturnType::Immutable;
			}

			//Flag for a return call within this scope. But it means different things for the routine's scope than it does for something like an
			// if scope. So I gotta work that out.



			//Make sure to remove copy assignment stuff. Doesn't need it. Shouldn't leave its initial function

		};



		QualifiedField Scope::SearchField(std::string name, OverloadKey& key, FieldPredicate pred)
		{
			//Move to the compiler maybe?
			auto end = vars.end();

			if (auto it = vars.find(name); it != end) {
				return &it->second;
			}
			else if (parent) {
				return parent->SearchField(name, key, pred);
			}
			else if (auto field = process->GetEnvironment()->SearchField(name, key, pred); field) {
				return field;


			}

			return nullptr;
		}

		QualifiedField Scope::SearchField(std::string name, FieldPredicate pred)
		{
			OverloadInput input;

			return SearchField(name, input, pred);
		}

		ITypePolicy* Scope::SearchType(std::string name)
		{
			//I'm too fucking lazy to make it work normally, so this is what we're gonna deal with til I do.

			Record dummy{ "dummy", 0, Record{name} };

			auto result = GetPolicyFromSpecifiers(dummy, process->GetEnvironment());

			return result;
		}


		std::string Scope::name()
		{
			return process->_targetFunc->_name;
		}


		void TestInstantiate()
		{
			VariableInfo{};
			FunctionInfo{};
			LocalInfo{};
			ParameterInfo{};
			FunctionInfo{};
		}
	}




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
