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
#include "Lexicon/Engine/Runtime.h"
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

	//*
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
	AbstractTypePolicy* VariableType<Void>::operator()(const Void*)
	{
		return nullptr;
	}
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
	AbstractTypePolicy* Variable::_CheckVariableType()
	{
		AbstractTypePolicy* result = std::visit([&](auto&& lhs) {
			return FetchVariableType(lhs);
			}, _value);

		return result;
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



		RuntimeVariable Execute(std::vector<RuntimeVariable>& args, RuntimeVariable* def)
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


			report::debug("inf test {}", test_inf.ToString());

			unsigned_int8.Assign(testB);
			report::debug("uint8 {}", unsigned_int8.ToString());

			report::debug("{} + {} = {} ?", testA.ToString(), testB.ToString(), testResult.ToString());
			constexpr auto fill = std::numeric_limits<int8_t>::max();

			unsigned_int8.Visit([](auto& it) { it += 1; });

			report::debug("uint8 later {}", unsigned_int8.ToString());
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


	namespace NewGenericV2
	{
		constexpr TypeID Trival = 0xDEADBEEF;
		constexpr TypeID Tuple = 0xDEADBAAD;

		struct ITemplateBody;
		struct TemplateTuple;

		struct TemplateType : public ITypePolicy, public HierarchyData
		{
			//GenericType is an ITypePolicy that largely should not exist with any HierarchyData. It's from this fact
			// plus the fact HierarchyData is a lot that I think I should split the function between 2 parts.
			//This type will use a seperate HierarchyData having type in order to answer questions it's questions about it.
			
			//But until I have a proper design for that I think having hierarchy data will be fine.
			void Test()
			{
				IdentityManager::instance->GetIDFromName("TRIVAL");


				ITypePolicy* test = nullptr;

				HierarchyData* other = dynamic_cast<HierarchyData*>(test);
			}

			std::string name;
			size_t index = -1;
			TemplateTuple* _tupleData = nullptr;
			TemplateType(std::string n, size_t i) : name{ n }, index{ i } {};



			AbstractTypePolicy* GetTypePolicy(LEX::IGenericArgument* args) override
			{
				return nullptr;
			}

			virtual ITypePolicy* CheckTypePolicy(GenericBase* ask, ITemplatePart* args) override;

			AbstractTypePolicy* GetTypePolicy(ITemplateBody* args) override;



			virtual bool IsResolved() const { return false; }

			virtual TypeID GetTypeID() const { return _tupleData ? Tuple : Trival; }

			virtual DataType GetDataType() const { return DataType::Interface; }



			virtual std::string_view GetName() const { return name; }


			HierarchyData* GetHierarchyData() const override 
			{ 
				const HierarchyData* out = this;
				return const_cast<HierarchyData*>(out);
			}

			ITypePolicy* GetHierarchyType() override
			{
				return this;
			}

		};



		struct ITemplateInsertBody;
		struct ITemplateInsertPart;

		struct ITemplateBody;

		struct ITemplatePart
		{
			virtual size_t GetSize() const = 0;

			virtual ITypePolicy* GetPartArgument(size_t i) const = 0;

			//TODO: Instead of all this, why not make a hash of each entry?

			//Always try to promote before using. 
			virtual ITemplateBody* TryPromoteTemplate() { return nullptr; }

			bool IsSpecialized() { return TryPromoteTemplate(); }





			virtual std::unique_ptr<ITemplatePart> MakeGenericArray(GenericBase* ask, ITemplatePart* args);

			virtual std::unique_ptr<ITemplateBody> MakeGenericArray(ITemplateBody* args);



		};

		struct ITemplateBody : public ITemplatePart
		{
			ITypePolicy* GetPartArgument(size_t i) const override final
			{
				return GetBodyArgument(i);
			}


			virtual AbstractTypePolicy* GetBodyArgument(size_t i) const = 0;


			ITemplateBody* TryPromoteTemplate() override
			{
				return this;
			}
		};

		
		struct ITemplateInsertBody;
		struct ITemplateInsertPart;
		struct ITemplateInsertPart
		{

			virtual void InsertType(ITypePolicy* part) = 0;
			
			virtual ITemplateInsertBody* TryPromoteInserter() { return nullptr; }

		};

		struct ITemplateInsertBody : public ITemplateInsertPart
		{

			void InsertType(ITypePolicy* part) override
			{
				auto back = part->GetTypePolicy((ITemplateBody*)nullptr);

				assert(back);

				return InsertType(back);
			}

			virtual void InsertType(AbstractTypePolicy* body) = 0;

			virtual ITemplateInsertBody* TryPromoteInserter() { return this; }
		};

		
		struct GenericPartArray : public ITemplatePart, public ITemplateInsertPart
		{
			GenericPartArray() { logger::info("fff"); }
			size_t GetSize() const override { return _types.size(); }

			ITypePolicy* GetPartArgument(size_t i) const override
			{
				auto type = _types[i];

				return type;
			}

			void InsertType(ITypePolicy* part) override
			{
				_types.emplace_back(part);
			}


		

			mutable std::vector<ITypePolicy*> _types;
		};

		struct GenericBodyArray : public ITemplateBody, public ITemplateInsertBody
		{

			size_t GetSize() const override { return _types.size(); }

			AbstractTypePolicy* GetBodyArgument(size_t i) const override
			{
				auto& type = _types[i];

				return type;
			}


			void InsertType(AbstractTypePolicy* body) override
			{
				_types.emplace_back(body);
			}

			mutable std::vector<AbstractTypePolicy*> _types;
		};


		std::unique_ptr<ITemplatePart> ITemplatePart::MakeGenericArray(GenericBase* ask, ITemplatePart* args)
		{
			//If no args, issue probably.

			auto size = GetSize();

			size_t proper = 0;

			GenericPartArray* gen_array = new GenericPartArray;

			std::unique_ptr<ITemplatePart> result{ gen_array };


			gen_array->_types.resize(size);

			for (int x = 0; x < size; x++)
			{
				auto& l_type = gen_array->_types[x];
				auto r_type = args->GetPartArgument(x);

				l_type = r_type->CheckTypePolicy(ask, args);

				proper += l_type->IsResolved();
			}

			if (proper == size)
				return gen_array->MakeGenericArray(nullptr);

			return result;
		}

		std::unique_ptr<ITemplateBody> ITemplatePart::MakeGenericArray(ITemplateBody* args)
		{
			//If no args, issue probably.

			auto size = GetSize();

			GenericBodyArray* gen_array = new GenericBodyArray;

			std::unique_ptr<ITemplateBody> result{ gen_array };

			gen_array->_types.resize(size);

			for (int x = 0; x < size; x++)
			{
				auto type = args->GetPartArgument(x);

				gen_array->_types[x] = type->GetTypePolicy(args);
			}

			return result;
		}





		using IGenericParameter = ITemplatePart;
		using IGenericArgument = ITemplateBody;

		
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



		struct SpecialBase;
		struct SpecialPart;
		struct SpecialBody;
		struct GenericBase;

		struct ISpecializable
		{
			//I don't wanna do this but there's no dynamic cast without polymorphism so +8
			virtual ~ISpecializable() = default;
			
			virtual GenericBase* GetGeneric() const = 0;

			//virtual SpecialBase* GetPart(ITemplatePart* args) = 0;

			virtual SpecialBody* ObtainBody(IGenericArgument* args) = 0;


			virtual bool TemplateMatches(ITemplatePart* args) = 0;


		};

		struct GenericBase : public ISpecializable
		{
			//This is the class that handles Generic stuff, old name ISpecializable



			GenericBase* GetGeneric() const override
			{
				return const_cast<GenericBase*>(this);
			}

			SpecialPart* FindPart(GenericBase* tar, ITemplatePart* args);

			SpecialBody* FindBody(ITemplateBody* args);



			virtual std::unique_ptr<SpecialPart> CreatePart(ITemplatePart* args) = 0;
			virtual std::unique_ptr<SpecialBody> CreateBody(ITemplateBody* args) = 0;

			//Note that validation needs to happen around here for multiple parts.


			//Might need to be override for this
			virtual SpecialPart* ObtainPart(GenericBase* client, ITemplatePart* args)
			{
				auto result = client->FindPart(this, args);

				if (!result) {
					result = client->incomplete.emplace_back(CreatePart(args)).get();
				}

				return result;
			}

			SpecialBody* ObtainBody(ITemplateBody* args) override
			{
				if (TemplateMatches(args) == false)
				{
					report::fault::error("cant handle args");
				}

				auto result = FindBody(args);

				if (!result) {
					result = Specialize(args);
				}

				return result;
			}

			SpecialBody* Specialize(ITemplateBody* args)
			{
				SpecializeParts(args);

				return complete.emplace_back(CreateBody(args)).get();

			}

			void SpecializeParts(ITemplateBody* args);

			SpecialBase* ObtainSpecial(GenericBase* client, ITemplatePart* args);

			bool TemplateMatches(ITemplatePart* args) override
			{
				//This expects a completed template btw.

				auto size = _template.size();



				if (args->GetSize() != size)
					return false;

				for (int i = 0; i < size; i++)
				{
					auto& param = _template[i];

					//This would be the thing it should be trying to turn into
					ITypePolicy* _param = &param;

					ITypePolicy* arg = args->GetPartArgument(i);

					Element* element = dynamic_cast<Element*>(this);

					ITypePolicy* scope = element->FetchEnvironment()->ToType();

					if constexpr (0)
					{//For now it accepts all, so no real reason to do this.
						auto conv = arg->IsConvertibleTo(_param, scope, nullptr);

						if (conv <= ConvertResult::Failure) {
							return false;
						}
					}
				}

				return true;
			}

			//So try specialize is what you'll get when you try to specialize I guess?
			//SpecialBase* TrySpecialize()

			//SpecialBody* ObtainBody()


			//I'm unsure if this would even bee needed honestly.
			//void ResolveIncomplete(IGenericArgument*) {}




			std::vector<TemplateType> _template;

			std::vector<std::unique_ptr<SpecialPart>> incomplete;
			std::vector<std::unique_ptr<SpecialBody>> complete;
		};
	

		


		struct SpecialBase : public ISpecializable
		{
			SpecialBase(GenericBase* b) : _base{ b } {}

			//I don't wanna do this but there's no dynamic cast without polymorphism so +8


			GenericBase* GetGeneric() const override
			{
				return _base;
			}

			virtual SpecialPart* AsPart() { return nullptr; }
			virtual SpecialBody* AsBody() { return nullptr; }

			virtual ITemplatePart* AsTemplate() = 0;
			
			virtual bool TemplateMatches(ITemplatePart* args) = 0;

			
			virtual size_t GetSize() const = 0;

			//Get rid of this, AsTemplate handles this.
			virtual ITypePolicy* GetArgument(size_t i) const = 0;


		private:
			mutable GenericBase* _base = nullptr;
		};






		//Maybe use spans instead of vectors.



		//Both special part and special body will likely become ITemplate somethings. Thinking about it, if  I have a function or type that needs 
		// to be specialized, it fits that this would be specializing it right?
		//If that happens get argument will not be required any longer.

		struct SpecialPart : public SpecialBase, public ITemplatePart
		{
		public:
			SpecialPart(GenericBase* type, ITemplatePart* spec) : SpecialBase{ type } 
			{
				auto size = spec->GetSize();

				for (auto i = 0; i < size; i++)
				{
					auto arg = spec->GetPartArgument(i);
					_types.emplace_back(arg);
				}
			}




			SpecialBase* ObtainSpec(GenericBase* ask, ITemplatePart* args)
			{
				//Purify arguments here, then send those args. Since this is par

				auto _array = MakeGenericArray(ask, args);

				return GetGeneric()->ObtainSpecial(ask, _array.get());
			}


			//This is still unhandled perhaps?
			SpecialBody* ObtainBody(ITemplateBody* args) override
			{
				//Purify arguments here, then send those args. Since this is par

				auto _array = MakeGenericArray(args);

				return GetGeneric()->ObtainBody(_array.get());
			}

			bool TemplateMatches(ITemplatePart* args) override
			{
				//This cannot have fully defined types, so if it can be promoted there's no match
				if (args->TryPromoteTemplate() != nullptr)
					return false;

				auto size = _types.size();



				if (args->GetSize() != size)
					return false;

				for (int i = 0; i < size; i++)
				{
					auto& lhs = _types[i];

					ITypePolicy* rhs = args->GetPartArgument(i);

					if (lhs != rhs) {
						return false;
					}

				}

				return true;
			}

			ITypePolicy* GetArgument(size_t i) const override final
			{
				return _types[i];
			}

			virtual ITypePolicy* GetPartArgument(size_t i) const override final
			{
				return _types[i];
			}

			ITemplatePart* AsTemplate() override
			{
				return this;
			}

			size_t GetSize() const override final
			{
				return _types.size();
			}

		private:
			std::vector<ITypePolicy*> _types;
		};


		
		struct SpecialBody : public SpecialBase, public ITemplateBody
		{
		public:
			SpecialBody(GenericBase* type, ITemplateBody* spec) : SpecialBase{ type }
			{
				auto size = spec->GetSize();

				for (auto i = 0; i < size; i++)
				{
					auto type = spec->GetBodyArgument(i);
					_types.emplace_back(type);
				}
			}

			virtual SpecialBody* ObtainBody(IGenericArgument* args) override 
			{ 
				return this; 
			}

			bool TemplateMatches(ITemplatePart* part) override
			{
				//This cannot have fully defined types, so if it can be promoted there's no match
				
				ITemplateBody* args = part->TryPromoteTemplate();

				if (!args)
					return false;

				auto size = _types.size();



				if (args->GetSize() != size)
					return false;

				for (int i = 0; i < size; i++)
				{
					auto& lhs = _types[i];
					AbstractTypePolicy* rhs = args->GetBodyArgument(i);

					if (lhs != rhs) {
						return false;
					}

				}

				return true;
			}


			ITypePolicy* GetArgument(size_t i) const override final
			{
				return _types[i];
			}


			virtual AbstractTypePolicy* GetBodyArgument(size_t i) const override final
			{
				return _types[i];
			}

			ITemplatePart* AsTemplate() override
			{
				return this;
			}

			virtual size_t GetSize() const override final
			{
				return _types.size();
			}

		private:
			//This isn't properly needed, what actually might be needed is just a hash of some kind.
			std::vector<AbstractTypePolicy*> _types;

		};



		
		struct GenericType : public PolicyBase, public GenericBase
		{
			//ITypePolicy* GetTypePolicy()



			AbstractTypePolicy* GetTypePolicy(LEX::IGenericArgument* args)// override
			{
				return nullptr;
			}

			ITypePolicy* CheckTypePolicy(GenericBase* base, ITemplatePart* args) override
			{
				return dynamic_cast<ITypePolicy*>(ObtainSpecial(base, args));
			}

			AbstractTypePolicy* GetTypePolicy(ITemplateBody* args)// override
			{
				return dynamic_cast<AbstractTypePolicy*>(ObtainBody(args));
			}

			std::unique_ptr<SpecialPart> CreatePart(ITemplatePart* args) override;
			
			std::unique_ptr<SpecialBody> CreateBody(ITemplateBody* args) override;

		};

		
		

		//These 2 can largely be boiler place tbh.
		struct SpecialTypePart : public ITypePolicy, public SpecialPart
		{
			ITypePolicy* _self = nullptr;

			SpecialTypePart(GenericBase* base, ITypePolicy* type, ITemplatePart* spec) : SpecialPart{ base, spec }, _self{ type }
			{
				report::info("NewBody {}", (uintptr_t)this);
			}

			SpecialTypePart(GenericType* type, ITemplatePart* spec) : SpecialPart{ type, spec }, _self{ type }
			{
				report::info("NewPart {}", (uintptr_t)this);
			}

			

			ITypePolicy* GetSelf()
			{
				return _self;
			}
			
			const ITypePolicy* GetSelf() const
			{
				return _self;
			}



			AbstractTypePolicy* GetTypePolicy(LEX::IGenericArgument* args) override
			{
				return nullptr;
			}


			ITypePolicy* CheckTypePolicy(GenericBase* ask, ITemplatePart* args) override
			{
				return dynamic_cast<ITypePolicy*>(ObtainSpec(ask, args));
			}

			AbstractTypePolicy* GetTypePolicy(ITemplateBody* args)// override
			{
				return dynamic_cast<AbstractTypePolicy*>(ObtainBody(args));
			}



			virtual TypeID GetTypeID() const { return GetSelf()->GetTypeID(); }

			virtual DataType GetDataType() const { return GetSelf()->GetDataType(); }



			virtual std::string_view GetName() const { return GetSelf()->GetName(); }

	
			HierarchyData* GetHierarchyData() const override
			{
				return GetSelf()->GetHierarchyData();
			}

		};



		struct SpecialTypeBody : public AbstractTypePolicy, public SpecialBody
		{
			ITypePolicy* _self = nullptr;
			
			SpecialTypeBody(GenericBase* base, ITypePolicy* type, ITemplateBody* spec) : SpecialBody{ base, spec }, _self{ type }
			{
				report::info("NewBody {}", (uintptr_t)this);
			}

			SpecialTypeBody(GenericType* type, ITemplateBody* spec) : SpecialBody{ type, spec }, _self{ type }
			{
				report::info("NewBody {}", (uintptr_t)this);
			}

			ITypePolicy* GetSelf()
			{
				return _self;
			}

			const ITypePolicy* GetSelf() const
			{
				return _self;
			}

			AbstractTypePolicy* GetTypePolicy(ITemplateBody* args) override
			{
				logger::info("HIT:");
				return this;
			}



			virtual TypeID GetTypeID() const { return GetSelf()->GetTypeID(); }

			virtual DataType GetDataType() const { return GetSelf()->GetDataType(); }



			virtual std::string_view GetName() const { return GetSelf()->GetName(); }



			HierarchyData* GetHierarchyData() const override
			{
				return GetSelf()->GetHierarchyData();
			}


			virtual Variable GetDefault() { throw nullptr; }

	
			virtual void SetDefault(Variable&) { throw nullptr; }

			virtual AbstractTypePolicy* GetExtends() { throw nullptr; }

		};


		struct DynamicTupleElement : public ITypePolicy
		{
			size_t index = -1;
			Operand operand;

			AbstractTypePolicy* GetTypePolicy(ITemplateBody* args) override
			{
				//This is basically how I would do it.

				auto runtime = dynamic_cast<Runtime*>(args);

				assert(runtime);

				auto tuple = dynamic_cast<SpecialTypeBody*>(runtime->GetArgAtIndex(index));

				Number access = operand.GetVariable(runtime)->AsNumber();

				assert(access._setting.IsInteger() && access < 0);

				

				return tuple->GetArgument(access)->GetTypePolicy((ITemplateBody*)nullptr);
			}



			//This does nothing else but get specialized. All other uses should be reported.

			virtual TypeID GetTypeID() const { throw nullptr; }

			virtual DataType GetDataType() const { throw nullptr; }



			virtual std::string_view GetName() const { throw nullptr; }



			HierarchyData* GetHierarchyData() const override { throw nullptr; }


			virtual Variable GetDefault() { throw nullptr; }


			virtual void SetDefault(Variable&) { throw nullptr; }

			virtual AbstractTypePolicy* GetExtends() { throw nullptr; }

		};

		struct TemplateTuple : public ITypePolicy, public GenericBase
		{
			TemplateType* parent = nullptr;
			GenericBase* owner = nullptr;


			//When searching for a part or a body, the find function will need to expand it first. I think actually, I may make those obtains virtual.
			// Correction, they're intended to be virtual. So, what I'll do is expand the packs (but I guess only if I need to) and then use them to find
			// what I'm looking for.



			void UnpackImpl2(ITemplateInsertPart* out, ITypePolicy* tar)
			{
				if (tar->GetTypeID() == Tuple)//This means it's a tuple, we can use it for our aims.
				{
					//At this point it should be a special base somewhere, a lack of one is an error 100%.
					SpecialBase* spec = dynamic_cast<SpecialBase*>(tar);
					
					assert(spec);

					auto size = spec->GetSize();

					for (int i = 0; i < size; i++)
					{
						auto type = spec->GetArgument(i);

						out->InsertType(type);
					}
				}
				else
				{
					out->InsertType(tar);
				}
			}
			
			void UnpackImpl1(ITemplateInsertPart* out, ITemplatePart* args)
			{
				auto size = args->GetSize();

				for (int i = 0; i < size; i++)
				{
					ITypePolicy* type = args->GetPartArgument(i);

					UnpackImpl2(out, type);
				}
			}


			GenericPartArray Unpack(ITemplatePart* args)
			{
				GenericPartArray result;

				UnpackImpl1(&result, args);

				return result;
			}

			GenericBodyArray Unpack(ITemplateBody* args)
			{
				GenericBodyArray result;

				UnpackImpl1(&result, args);

				return result;
			}


			SpecialPart* ObtainPart(GenericBase* client, ITemplatePart* args) override
			{
				//Unpack here

				auto pack = Unpack(args);

				return __super::ObtainPart(client, &pack);
			}

			SpecialBody* ObtainBody(ITemplateBody* args) override
			{
				//Unpack here

				auto pack = Unpack(args);

				return __super::ObtainBody(&pack);
			}




			ITypePolicy* CheckTypePolicy(GenericBase* base, ITemplatePart* args) override
			{
				return dynamic_cast<ITypePolicy*>(ObtainSpecial(base, args));
			}

			AbstractTypePolicy* GetTypePolicy(ITemplateBody* args)// override
			{
				return dynamic_cast<AbstractTypePolicy*>(ObtainBody(args));
			}


			std::unique_ptr<SpecialPart> CreatePart(ITemplatePart* args) override
			{

				return std::make_unique<SpecialTypePart>(this, this, args);
			}

			std::unique_ptr<SpecialBody> CreateBody(ITemplateBody* args) override
			{
				return std::make_unique<SpecialTypeBody>(this, this, args);
			}


		};










		SpecialBody* GenericBase::FindBody(ITemplateBody* args)
		{
			for (auto& spec : complete)
			{
				if (spec->TemplateMatches(args) == true)
					return spec.get();
			}

			return nullptr;
		}
		void GenericBase::SpecializeParts(ITemplateBody* args)
		{
			for (auto& part : incomplete) {
				report::info("thing {}", (uintptr_t)part.get());
				part->ObtainBody(args);
			}
		}
		SpecialPart* GenericBase::FindPart(GenericBase* tar, ITemplatePart* args)
		{
			for (auto& spec : incomplete)
			{
				//First compare target, then compare args.

				if (spec->GetGeneric() == tar && spec->TemplateMatches(args) == true)
					return spec.get();
			}

			return nullptr;
		}
		SpecialBase* GenericBase::ObtainSpecial(GenericBase* client, ITemplatePart* args)
		{
			if (TemplateMatches(args) == false)
			{
				report::fault::error("cant handle args");
			}

			if (auto temp = args->TryPromoteTemplate(); temp)
				return ObtainBody(temp);
			else
				return ObtainPart(client, args);
		}


		std::unique_ptr<SpecialPart> GenericType::CreatePart(ITemplatePart* args) 
		{
			
			return std::make_unique<SpecialTypePart>(this, args);
		}

		std::unique_ptr<SpecialBody> GenericType::CreateBody(ITemplateBody* args)
		{
			return std::make_unique<SpecialTypeBody>(this, args);
		}

		ITypePolicy* TemplateType::CheckTypePolicy(GenericBase* ask, ITemplatePart* args)
		{
			auto part = args->GetPartArgument(index);

			return part;
		}

		AbstractTypePolicy* TemplateType::GetTypePolicy(ITemplateBody* args)// override
		{
			auto body = args->GetBodyArgument(index);

			return body;
		}


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

	
	template<typename T>
	struct failure;

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


	

	INITIALIZE()
	{
		FetchVariableType<Variable>();
		FetchVariableType<Number>();
		FetchVariableType<String>();
		FetchVariableType<Variable>();
		FetchVariableType<Variable>();
		FetchVariableType<Variable>();
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
