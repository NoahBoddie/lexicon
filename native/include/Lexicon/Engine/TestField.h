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
#include "Lexicon/Engine/GlobalVariable.h"
#include "Lexicon/Engine/Literal.h"
#include "Lexicon/Engine/LiteralManager.h"
#include "Lexicon/Engine/Scope.h"
#include "Lexicon/Engine/Solution.h"
#include "Lexicon/Engine/RoutineBase.h"
#include "Lexicon/Engine/Operation.h"
#include "Lexicon/Engine/OperandType.h"
#include "Lexicon/Engine/Target.h"
#include "Lexicon/MemberPointer.h"
#include "Lexicon/Engine/RoutineCompiler.h"



#include "Lexicon/TypeID.h"


#include "Lexicon/Engine/ConcreteType.h"


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

#include "Lexicon/Specifier.h"

#include "Lexicon/Engine/OverloadInput.h"

#include "Lexicon/Engine/Declaration.h"

#include "Lexicon/Interfaces/ProcedureHandler.h"
#include "Lexicon/Dispatcher.h"

#include "Lexicon/String.h"

#include "Lexicon/Engine/Signature.h"

#include "Lexicon/Engine/GenericBase.h"

#include "Lexicon/GenericArray.h"

#include "Lexicon/Engine/GenericType.h"

#include "Lexicon/Interfaces/ReportManager.h"

#include "Lexicon/Engine/SyntaxRecord.h"

#include "Lexicon/RelateType.h"


#include "Lexicon/Engine/SettingManager.h"

#include "Lexicon/Formula.h"

#include "Lexicon/Engine/ConcreteGlobal.h"

#include "Lexicon/NativeReference.h"

#include "Lexicon/ITemplateBodyPart.h"
#include "Lexicon/ITemplateInserter.h"

#include "Lexicon/ProxyGuide.h"
#include "Lexicon/Revariable.h"
#include "Lexicon/Engine/SpecialFunction.h"
#include "Lexicon/Engine/GenericFunction.h"
#include "Lexicon/Array.h"


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




namespace LEX
{




	



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
		
		//*
		auto formula = Formula<double(std::string::*)()>::Create("size()");
		//std::string six = "six";
		//static_assert(!std::is_pointer_v<decltype(formula)::TarType>);
		double result_of_string;

		if (formula) {
			result_of_string = formula("six").Call();
		}
		else {
			result_of_string = NAN;
		}

		logger::critical("result of new form << {}", result_of_string);
		//*/
		std::system("pause");


		Voidable result = Formula<Voidable>::Run("(25).CouldBeLiterallyAnything()");

		logger::critical("{}", result.PrintString());

	}




	namespace NEW_PARSING
	{

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
			}
			else {
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







	inline void TestCreateAllSettings()
	{
		return;
		//This is the name of the unique policy for numbers, that can handle the conversions between things itself.
		using NumericPolicy = ConcreteType;

		std::vector<ConcreteType*> results;

		NumeralType type{};
		Size        size{};
		Signage     sign{};
		Limit       limit{ Limit::Overflow };

		IdentityManager::instance->GenerateID("NUMBER", Number::Settings::length);

		ConcreteType* primary_policy = new ConcreteType{ "NUMBER", 0 };

		for (int a = 0; a < NumeralType::Total; a++)
		{
			for (int b = 0; b < Size::Total; b++)
			{
				for (int c = 0; c < Signage::Total; c++)
				{
					for (int d = 1; a < Limit::Total; d++)
					{
						Number::Settings settings{ (NumeralType)a, (Size)b, (Signage)c, (Limit)d };
						ConcreteType* number_policy = new ConcreteType{ "NUMBER", settings.GetOffset() };
						Variable defaultValue{ settings };
						number_policy->EmplaceDefault(defaultValue);
						results.emplace_back(number_policy);


					}
				}
			}
		}

		//static TypePolicy* float64 = new TypePolicy{ "NUMBER", Number::Settings::GetOffset(NumeralType::Floating) };

		//do something with values.
	}


	class IntrinsicPolicy : public ConcreteType
	{
		//The concept of an intrinsic policy is first simply that intrinsic policies are the only 
		// policies that can Claim a specific space. Think like how void would or something like that.

		//Second, further branches of it will be able to present default versions of things such as strings, numbers
		// arrays, etc. This prevents us from having to carry the variable around in every single type despite not needing it.

		//There will likely have to be one for generic, but every generic/concrete one won't need new satelites, as they
		// will just assume that it is a concrete policy. Which it is.





	};

#ifdef ENUM_TEST
	namespace EnumTesting
	{

		struct IEnumInfo : public TypeInfo
		{
			//All that enum info will need, is the ability to turn strings into numbers, and numbers into strings.

			//In terms of reading and loading flags, I think what I'll likely do is have it filled one by one, submitting string_view
			// to be filled by the info.


			virtual bool GetEnumName(uint64_t value, std::string_view& out) const = 0;
			virtual bool GetFlagName(uint64_t flags, uint8_t& pos, std::string_view& out) const = 0;//Keep calling til it returns false


			virtual bool GetValue(const std::string_view& name, uint64_t& out) const = 0;

			virtual bool GetNameAtIndex(size_t index, bool isFlag, std::string_view& out) const = 0;


			virtual bool SetEnumValue(const std::string_view& name, uint64_t& value) const = 0;

			virtual bool SetFlagValue(const std::span<std::string_view>& names, bool is_add, uint64_t& value) const = 0;
			

			//Pulls all flags. Must use if only trying to operate on flags.
			virtual uint64_t GetAllFlags() const = 0;
		};


		using EnumTypeBase = PivotTypeBase<IEnumInfo>;


		struct EnumInfo : public EnumTypeBase
		{
			//The idea of this being a type info is that I can instead use this when trying to assign enum members to each other.
			struct EnumMember// : public TypeInfo
			{
				std::string_view name;
				uint64_t useMask = -1;
				size_t parindex = -1;
			};

			//static constexpr EnumMember defaultScope{ "default", -1 };

			struct EnumEntry
			{
				std::string_view name;
				uint64_t value{};
				size_t parindex = -1;

			};

			//When it comes to operators, I'd like to manually have those here be only accessible if the enum has some flag values.


			//I'd like it that enums can inherit


			void AddEntry(const std::string_view& name, bool is_flag, std::optional<uint64_t> value)
			{
				if (!value)
				{
					//value = is_flag ? 
				}
			}

			//This is the only one of these I need. The index is kinda not really needed other than to make this quicker
			size_t nextValue = 0;

			size_t incValue(bool is_flag)
			{
				//This doesn't quite work out I think, largely, incValue should prevent overlap
				// between used values and proposed new values. I'll need to do this manually I think.

				size_t result;
				
				if (is_flag) {
					auto width = std::bit_width(nextValue);

					if (!nextValue || nextValue & (nextValue - 1)){
						result = nextValue = 1 << (++width);
					}

					nextValue = 1 << (++width);
				}
				else
				{
					return nextValue++;
				}
			}

			uint32_t enumCount = 0;
			uint32_t enumIndex = 0;


			uint8_t flagCount = 0;//Only one of these really needs to be respected
			uint8_t flagIndex = 0;

			uint64_t enumBits = 0;
			uint64_t flagBits = 0;


			//This can actually be shortened to less than a byte if need be.
			Size   size{ Size::Byte };
			Signage   sign{ Signage::Unsigned };

			std::vector<EnumMember> members;

			std::vector<EnumEntry> entries;


			//I can actually use the counts for the starting points and iterate through them. These should be ordered.
			const EnumEntry* FindEntry(uint64_t value, std::optional<bool> isFlag) const
			{

				if (value) {
			
					for (auto i = isFlag.value_or(false) ? 0 : enumCount; i < isFlag.value_or(true) ? entries.size() : enumCount; i++)
					{
						auto& entry = entries[i];
						if (entry.value == value)
							return &entry;
					}

					//for (auto& entry : entries)
					//{
					//	if (entry.value == value && (!isFlag.has_value() || isFlag.value() == entry.isFlag))
					//		return &entry;
					//}
				}
				return nullptr;
			}

			const EnumEntry* FindEntry(const std::string_view& name, std::optional<bool> isFlag) const
			{

				if (name.empty() == false) {
					for (auto i = isFlag.value_or(false) ? 0 : enumCount; i < isFlag.value_or(true) ? entries.size() : enumCount; i++)
					{
						auto& entry = entries[i];
						if (entry.name == name)
							return &entry;
					}
					
					
					//for (auto& entry : entries)
					//{
					//	if (entry.name == name && (!isFlag.has_value() || isFlag.value() == entry.isFlag))
					//		return &entry;
					//}
				}
				return nullptr;
			}

			const EnumEntry* GetEntry(size_t i, bool isFlag) const
			{
				if (isFlag)
					i += enumCount;

				if (entries.size() <= i)
					return nullptr;

				return &entries[i];
			}




			bool GetEnumName(uint64_t value, std::string_view& out) const override
			{
				value &= enumBits;
				const EnumEntry* entry = FindEntry(value, false);

				if (entry)
					out = entry->name;

				return entry;

			}
			bool GetFlagName(uint64_t flags, uint8_t& pos, std::string_view& out) const
			{
				while (pos <= sizeof(uint64_t))
				{
					if (auto value = (static_cast<uint64_t>(1) << pos++) & flags; value & flagBits)
					{
						const EnumEntry* entry = FindEntry(value, true);

						out = entry->name;
						return true;
					}
					//else if (value >= flagBits) return false;
					
				} 

				return false;
			}



			bool GetValue(const std::string_view& name, uint64_t& out) const override
			{
				
				const EnumEntry* entry = FindEntry(name, std::nullopt);

				if (entry)
					out = entry->value;

				return entry;
			}

			bool GetNameAtIndex(size_t index, bool isFlag, std::string_view& out) const override
			{
				const EnumEntry* entry = GetEntry(index, isFlag);


				if (entry)
					out = entry->name;

				return entry;
			}

			bool SetEnumValue(const std::string_view& name, uint64_t& out) const override
			{

				const EnumEntry* entry = FindEntry(name, true);


				if (entry)
					out = entry->value;

				return entry;
			}

			bool SetFlagValue(const std::span<std::string_view>& names, bool is_add, uint64_t& out) const override
			{
				//returns false if only one of them fails

				//Unsure if I want it to not work full stop  if one fails
				uint64_t prox = out;
				

				for (auto& name : names)
				{

					const EnumEntry* entry = FindEntry(name, true);


					if (!entry)
						return false;

					if (is_add)
						prox |= entry->value;
					else
						prox &= entry->value;
				}

				out = prox;

				return true;
			}

			//Pulls all flags. Must use if only trying to operate on flags.
			uint64_t GetAllFlags() const override
			{
				return flagBits;
			}

		};

		//Terms
		//Enum-primary value
		//Flags- secondary value
		//Name- name of the enumeration
		//Value- the number value.


		struct BasicEnum
		{
			BasicEnum(IEnumInfo* info, const std::string_view& value)
			{

			}

			uint64_t value = 0;
			IEnumInfo* info = nullptr;


			std::string_view GetEnumName()
			{
				std::string_view result;

				if (info->GetEnumName(value, result) == false)
					return "<Unknown>";

				return result;
			}

			std::vector<std::string_view> GetFlagNames()
			{
				//Doing this in case it changes mid function
				auto buff = value;

				std::vector<std::string_view> result;

				uint8_t index = 0;

				while (true)
				{
					std::string_view entry;

					if (info->GetFlagName(buff, index, entry) == false) {
						break;
					}

					result.push_back(entry);

				}


				return result;
			}

		};

		template <StringLiteral EnumName>
		struct Enum : public BasicEnum
		{
			//Enum(const std::string_view& value)
		};



	}
#endif






	







	//Maybe use spans instead of vectors.



	//Both special part and special body will likely become ITemplate somethings. Thinking about it, if  I have a function or type that needs 
	// to be specialized, it fits that this would be specializing it right?
	//If that happens get argument will not be required any longer.




		
		

	//These 2 can largely be boiler place tbh.
		








	struct TemplateSpan : public ITemplateBodyPart
	{
		//The idea of this is that it basically serves as a span for an ITemplatePart. Mainly used for template matching to prevent it from having issues
		ITemplatePart* target = nullptr;
		size_t limit = -1;


		//The client is whoever we'd be accessing from. So this is a little less cut and dry, and more prevailent to the circumstances.
		GenericBase* GetClient() const override { return target->GetClient(); }


		size_t GetSize() const override { return std::min(target->GetSize(), limit); }


		ITypeInfo* GetPartArgument(size_t i) const override
		{
			if (i >= limit)
				return nullptr;
			return target->GetPartArgument(i);
		}

		TypeInfo* GetBodyArgument(size_t i) const override
		{
			if (i >= limit)
				return nullptr;

			if (auto tar = target->TryResolve()) {
				return tar->GetBodyArgument(i);
			}

			return nullptr;
		}

		bool IsResolved() const override
		{
			return target->TryResolve();
		}

	};


	namespace TemporaryTypes
	{
		//I think this system should actually be for all types, but finalized by the field a the very end

		struct GenericTempOwner;

		struct TempSpecial : public SpecialBase
		{
			//Main function of this class is to act as a temporary special so we don't accidentally specialize something that shouldn't be yet. 
			~TempSpecial() override;



			GenericTempOwner* GetTempOwner();

			SpecialBase* ObtainSpecial(ITemplatePart* args) override
			{
				auto array = SpecializeTemplate(args);

				auto result = GetGeneric()->ObtainSpecial(array);

				//I'm unsure how safe of an idea this is.
				delete this;

				return result;
			}

			TempSpecial(GenericTempOwner* base, ITemplatePart* args);

			TempSpecial(GenericTempOwner* base, ITemplateBody* args);


		};


		struct GenericTempOwner : public GenericBase
		{
			void RemoveTemporary(TempSpecial* temp)
			{
				std::erase_if(tempList, [temp](auto& it) {return it.get() == temp; });
			}

			virtual std::unique_ptr<TempSpecial> CreateTemporary(ITemplatePart* args) = 0;

			virtual SpecialBase* TryObtainTemporary(ITemplatePart* args) override
			{
				if (TemplateMatches(args) == false) {
					report::fault::error("cant handle args");
				}

				//TODO: Merge ObtainBody/ObtainPart into this, there's no specialization for it, so no need.

				if (auto temp = args->TryResolve(); temp)
					return ObtainBody(temp);
				else
					return ObtainPart(args);
			}



			virtual void ClearTemporary()
			{
				tempList.clear();
			}



			std::vector<std::unique_ptr<TempSpecial>> tempList;
		};


		TempSpecial::TempSpecial(GenericTempOwner* base, ITemplatePart* args) : SpecialBase{ base, args } {}

		TempSpecial::TempSpecial(GenericTempOwner* base, ITemplateBody* args) : SpecialBase{ base, args } {}

		GenericTempOwner* TempSpecial::GetTempOwner()
		{
			return static_cast<GenericTempOwner*>(GetGeneric());
		}
		TempSpecial::~TempSpecial()
		{
			GetTempOwner()->RemoveTemporary(this);
		}

	}



#ifdef TTTTTTTTTT
	//Nothing highlights after this
	template <typename T>
	struct ContainerHelper
	{
		using iterator = std::vector<T>::iterator;
		using const_iterator = std::vector<T>::const_iterator;

		using span = std::span<T>;
		using const_span = std::span<const T>;

	private:
		inline static std::mutex _accessLock;

	public:
		virtual ~ContainerHelper() = default;


		virtual std::mutex& GetLock() const final
		{
			return _accessLock;
		}

		virtual std::span<T> data() final { return _data; }
		virtual std::span<const T> data() const final { return _data; }

		virtual void insert(iterator where, std::span<const T> range) final
		{
			_data.insert_range(where, range);
		}

		virtual void erase(iterator where, iterator end) final
		{
			if (where == end)
				_data.erase(where);
			else
				_data.erase(where, end);
		}

		virtual void resize(size_t new_size) final { return _data.resize(new_size); }
		virtual void reserve(size_t new_size) final { return _data.reserve(new_size); }

		virtual size_t size() const { return _data.size(); }
		virtual size_t capacity() const { return _data.capacity(); }

		virtual iterator begin() final { return _data.begin(); }
		virtual iterator end() final { return _data.end(); }

		virtual const_iterator begin() const final { return _data.begin(); }
		virtual const_iterator end() const final { return _data.end(); }

		const_iterator cbegin() const { return begin(); }
		const_iterator cend() const { return end(); }


		ContainerHelper() = default;

		ContainerHelper(const std::vector<T>& other) : _data{ other } {}
		ContainerHelper(std::vector<T>&& other) : _data{ std::move(other) } {}

		ContainerHelper(std::span<const T> range) : _data{ range.begin(), range.end() } {}

	private:
		std::vector<T> _data{};
	};

	struct ArrayVariable : public std::shared_ptr<Variable>
	{
		using Base = std::shared_ptr<Variable>;

		//using Base::Base;
		//using Base::operator bool;
		//using Base::operator*;
		//using Base::operator->;

		//Largely, this should always have a value. Voiding is for the base of this thing.
		ArrayVariable() : Base{ std::make_unique<Variable>() } {};

		ArrayVariable(const Variable& other) : Base{ std::make_shared<Variable>(other) } {}
		ArrayVariable(Variable&& other) : Base{ std::make_shared<Variable>(std::move(other)) } {}

		ArrayVariable(const ArrayVariable& other) : ArrayVariable{ other.ref() } {}
		ArrayVariable(ArrayVariable&& other) = default;



		ArrayVariable& operator=(const Variable& other)
		{
			Base::operator=(std::make_shared<Variable>(other));
			return *this;
		}
		ArrayVariable& operator=(Variable&& other)
		{
			
			Base::operator=(std::make_shared<Variable>(std::move(other)));
			return *this;
		}
		ArrayVariable& operator=(const ArrayVariable& other)
		{
			return operator=(other.ref());
		}

		ArrayVariable& operator=(ArrayVariable&& other) = default;


		
		//ArrayVariable(const Base& other) : Base{ other }{}
		//ArrayVariable(Base&& other) : Base{ std::forward<Base>(other) } {}
		

		Variable& ref()
		{
			return *get();
		}
		const Variable& ref() const
		{
			return *get();
		}


		Variable* ptr()
		{
			return get();
		}
		const Variable* ptr() const
		{
			return get();
		}

		operator Variable& ()
		{
			return ref();
		}
		operator const Variable& () const
		{
			return ref();
		}
	};


	struct Array
	{
		using Helper = ContainerHelper<ArrayVariable>;

		//I would like to make array a little more seperated in terms of the type. For example, I'd like array to be seperated into 2 sections
		// depending on where it's been accessed from. One cannot change it's size, and the other can.
		// eh, I think it would probably be better to just make tuple a different type and make a conversion.





				
		//To handle list invalidation, I think what I'll do is turn the array into something that holds onto RuntimeVariables only.
		//All plain variables are detachedm while other reference types will remain. Probably.

		//Problem with this, the shared pointer does not really regard the idea of this very well. Here's what I think I'll do. 
		// the shared pointer becomes a new type. This new type is basically a register system for detached references.
		// basically, when the shared pointer is created, it will add that pointer to a list. This list can be used by dispatch to compare to see if
		// it needs to remove a value.
		//Problem though, for the system that unvariables types, this may not exactly actually be effective. I could have a revariable set or something,
		// some kind of guide.

		//I need to consider how this works really really hard.

		//Also, reuse the container helper.

		//This may go outside of here.





		static TypeInfo* GetVariableType(const Array* self)
		{
			if (!self || !self->_type) {
				auto result = IdentityManager::instance->GetTypeByOffset("ARRAY", 0);

				if (!result) {
					report::fault::error("'ARRAY' at offset 0 is empty.");
				}

				return result->GetTypePolicy(nullptr);
			}

			auto type = IdentityManager::instance->GetTypeByOffset("ARRAY", 0);

			//TODO: Remove the boilerplate from using a GenericArray, it's common to want to do something like this
			GenericArray array{ nullptr, {self->type() }};

			auto result = type->GetTypePolicy(array.TryResolve());

			if (!result) {
				report::error("Failed to specialized 'ARRAY' offset of 1.");
			}

			return result;

		}

		//Needs a static cast to vector, plu

		bool HasContainer() const
		{
			return _container != nullptr;
		}

		TypeInfo* type() const
		{
			return _type;
		}

		Helper::span data()
		{
			if (HasContainer() == false)
				return {};

			return _container->data();
		}

		
		Helper::span data() const
		{
			//This should be const, I don't care right now.
			if (HasContainer() == false)
				return {};

			return _container->data();
		}

		size_t size() const
		{
			if (!_container)
				return 0;

			return _container->size();
		}

		constexpr Array() noexcept = default;
		
	
		Array(std::span<const Variable> range, TypeInfo* type = nullptr) : 
			_container{ std::make_unique<Helper>(std::vector<ArrayVariable>(range.begin(), range.end())) }, _type {type}
		{

		}
		Array(const Array& other)
		{
			//TODO: Unboiler plate
			_type = other._type;
				
			if (other._container) {
				_container = std::make_unique<Helper>(other._container->data());
				logger::info("This is a copy");
			}
		}

		Array(Array&& other) = default;

		Array& operator=(const Array& other)
		{
			_type = other._type;

			if (other._container) {
				_container = std::make_unique<Helper>(other._container->data());
			}

			return *this;
		}

		Array& operator=(Array&& other) = default;

		//Required type must have a valid implementation of Unvariable
		template <typename T> requires (!std::is_base_of_v<detail::not_implemented, Unvariable<T>>)//stl::castable_from<Variable>
		explicit operator std::vector<T>() const
		{
			if (!_container)
				return {};


			if constexpr (std::is_same_v<T, Variable>)
			{
				return std::vector<T>{_container->begin(), _container->end() };
			}
			else {
				auto data = _container->data();

				std::vector<T> result{};

				result.resize(data.size());

				std::transform(data.begin(), data.end(), result.begin(), [](ArrayVariable& it)
					{
						return Unvariable<T>{}(it.ptr());
					});

				return result;
			}
				
		}

		std::string PrintString(std::string_view context) const
		{
			auto stuff = _container->data();

			auto size = stuff.size();

			//std::vector<std::string> entries{ size };
			std::string result = "[";

			for (int i = 0; i < size; i++)
			{
				if (i)
					result += ", ";

				result += stuff[i]->PrintString();


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

	//private:

		std::unique_ptr<Helper> _container{};

		mutable TypeInfo* _type = nullptr;


	};

	//TODO: Make built in object infos, where if a type is denoted as such, it will make it's own object info, trying to 
	// call a static version of the classes functions
	template <>
	struct LEX::ObjectInfo<Array> : public QualifiedObjectInfo<Array>
	{
		template <specialization_of<std::vector> Vec>
		static Array ToObject(const Vec& obj)
		{
			logger::info("hitting");
			std::vector<Variable> buff;
			buff.reserve(obj.size());
			//const std::vector<void*> test;

			//void* other = test[1];


			std::transform(obj.begin(), obj.end(), std::back_inserter(buff), [&](auto it) {return it; });


			return Array{ buff };

		}


		TypeOffset GetTypeOffset(ObjectData& data) override
		{
			return data.get<Array>().type() != nullptr;
		}


		TypeInfo* SpecializeType(ObjectData& data, ITypeInfo* type) override
		{
			TypeInfo* result;

			if (type->IsResolved() == false) {

				GenericArray array{ nullptr, {data.get<Array>().type()} };

				auto result = type->GetTypePolicy(array.TryResolve());

				if (!result) {
					report::error("Failed to specialized 'ARRAY' offset of 1.");
				}

			}
			else {
				result = type->GetTypePolicy(nullptr);
			}

			return result;
		}

		//the form object info needs to edit the transfer functions,


		String PrintString(ObjectData& a_self, std::string_view context) override
		{
			return a_self.get<Array>().PrintString(context);
		}

	};

#endif

	
	//TODO: Move the set up for 

	

	//This is a test, it has more qualifications to fill than this.
	
	/*
	template <typename T> requires (!detail::subject_has_var_type<T>&&
		requires(ProxyGuide<T> guide, const std::remove_pointer_t<T>* ptr) { { guide.VariableType(ptr) } -> pointer_derived_from<TypeInfo*>; })
	struct VariableType<T>
	{
		TypeInfo* operator()(const std::remove_pointer_t<T>* arg)
		{

			return ProxyGuide<T>{}.VariableType(arg);
			//if constexpr (detail::subject_has_var_type_Value<T>) {
			//	return T::GetVariableType(arg);
			//}
			//else {
			//	return T::GetVariableType();
			//}
		}

		//Can likely remove this
		TypeInfo* operator()()
		{
			return this->operator()(nullptr);
		}
	};
	
	template <typename T> requires (requires(ProxyGuide<T> guide, const T& arg) { { guide.ObjectTranslator(arg) } -> has_object_info; })
	struct ObjectTranslator<T>
	{
		decltype(auto) operator()(const T& obj)
		{
			return ProxyGuide<T>{}.ObjectTranslator(obj);
		}
	};
	template <typename T> requires (requires(ProxyGuide<T> guide, Variable* arg) { { guide.Unvariable(arg) } -> std::convertible_to<T>; })
	struct Unvariable<T>
	{
		decltype(auto) operator()(Variable* var)
		{
			ProxyGuide<T> guide{};

			decltype(auto) result = guide.Unvariable(var);

			return result;
		}
	};


			

	//*/


#ifdef TTTTTTTTTT
	template<typename T>
	struct ProxyGuide <std::vector<T>> : public RefCollection
	{

		TypeInfo* VariableType(const std::vector<T>* vec)
		{
			return IdentityManager::instance->GetTypeByOffset("ARRAY", 0)->GetTypePolicy(nullptr);
			//TODO: This literally does not work, please implement this properly.
			//return Array::GetVariableType(vec);
		}

		Array ObjectTranslator(const std::vector<T>& obj)
		{
			std::vector<Variable> buff;
			buff.reserve(obj.size());
			//const std::vector<void*> test;

			//void* other = test[1];


			std::transform(obj.begin(), obj.end(), std::back_inserter(buff), [&](auto it) {return it; });


			return Array{ buff };

		}






		//TODO:Unboiler plate revariable pls, k thx
		void Revariable(const std::vector<T>& arg, Variable* var)
		{
			Array& array = var->AsObject().get<Array>();

			//We are making some assumptions here, and doing no checks

			if (auto size = array._container->size(); arg.size() != size) {
				report::runtime::error("const array's size was adjusted erroneously.");
			}

			auto data = array.data();

			for (size_t i = 0; i < arg.size(); i++)
			{
				auto& entry = arg[i];
				auto& to = data[i];

				LEX::Revariable<const T> revar;

				revar(entry, to.ptr());

				Collect(std::addressof(entry), to.ptr());

				TryToCollect(revar);
			}
		}



		void Revariable(std::vector<T>& arg, Variable* var)
		{
			Array& array = var->AsObject().get<Array>();
			
			//We are making some assumptions here, and doing no checks

			if (auto size = array._container->size(); arg.size() != size) {
				array._container->resize(size);
			}
			
			auto data = array.data();

			for (size_t i = 0; i < arg.size(); i++)
			{
				auto& entry = arg[i];
				auto& to = data[i];

				LEX::Revariable<T> revar;

				revar(entry, to.ptr());

				Collect(std::addressof(entry), to.ptr());

				TryToCollect(revar);
			}
		}


	};

	template <>
	struct Revariable<Array> : public RefCollection
	{
		void Fill(const Array& a_this, Array& other, bool assign)
		{

			auto this_data = a_this.data();
			auto other_data = other.data();

			for (size_t i = 0; i < a_this.size(); i++)
			{
				auto& entry = this_data[i].ref();
				auto& to = other_data[i].ref();

				Collect(std::addressof(entry), std::addressof(to));

				if (Object* object = entry.FetchObject()) {
					if (auto entry_array = object->fetch<Array>()) {

						Fill(*entry_array, std::addressof(to), assign);
						continue;
					}
				}

				if (assign)
					to.Assign(entry);

			}
		}

		void Fill(const Array& arg, Variable* var, bool assign)
		{
			Array& arg_array = var->AsObject().get<Array>();

			//We are making some assumptions here, and doing no checks

			if (auto size = arg.size(); arg_array._container->size() != size) {
				if (assign)
					arg_array._container->resize(size);
				else
					report::runtime::error("const array's size was adjusted erroneously.");
			}

			return Fill(arg, arg_array, assign);
		}


		void operator()(const Array& arg, Variable* var)
		{
			return Fill(arg, var, false);
		}

		void operator()(Array& arg, Variable* var)
		{
			return Fill(arg, var, true);
		}
	};

	template <typename T> requires (std::is_same_v<std::remove_cvref_t<T>, Array> && (std::is_const_v<T> || std::is_reference_v<T>))
		struct Revariable<T> : public Revariable<Array> {};
#endif



	int size(Array&& a_this)
	{
		return a_this.data().size();
	}




	int& RefTest3(StaticTargetTag, int& test)
	{


		return test;
	}

	Array CreateOne(StaticTargetTag)
	{
		std::vector<int> result{ 1, 2, 3, 4, 5, 69, 420 };

		return ObjectTranslator<decltype(result)>{}(result);
	}

	void resize(Array& a_this, int new_size)
	{
		if (a_this._container)
			a_this._container->resize(new_size);
	}

	Variable& get_backend(Array&& a_this, int index)
	{
		auto data = a_this.data();

		if (data.size() <= index) {
			report::runtime::error("error, fix now.");
		}

		return data[index];
	}


	INITIALIZE("function_register")
	{
		//Array t;
		//std::tuple<Array, Array> tuple;
		//std::tuple<Array, Array>& tuple2 = tuple;

		//auto tuple = std::forward_as_tuple(tuple);

		//std::apply(ArraySize_, std::forward<decltype(tuple2)>(tuple2));


		if (ProcedureHandler::instance->RegisterFunction(RefTest3, "Shared::Commons::RefTest3") == false) {
			logger::break_debug("failure rft3");
		}
		int num = 1;
		logger::info("success{} => {}", num++, ProcedureHandler::instance->RegisterCoreFunction(get_backend, "get"));
		logger::info("success{} => {}", num++, ProcedureHandler::instance->RegisterCoreFunction(CreateOne, "CreateOne"));
		logger::info("success{} => {}", num++, ProcedureHandler::instance->RegisterCoreFunction(resize, "resize"));
		logger::info("success{} => {}", num++, ProcedureHandler::instance->RegisterCoreFunction(size, "size"));

		std::system("pause");
	}





	INITIALIZE()
	{
		{
			
			const RuntimeVariable test1{};
			RuntimeVariable test2{};
			RuntimeVariable test3 = test1;
			RuntimeVariable test4 = std::move(test3);

			test2 = test1;
			test2 = std::move(test1);
			test2.operator=(test1);
			test2.operator=(test2);
		}
		{
			Variable test1;
			RuntimeVariable test2 = std::ref(test1);
			//return;
		}

		//This surprisingly, actually fucking works.
		//constexpr auto TEST = convertible_variant_index_v<VariableValue, std::vector<int>>;
		//constexpr auto TEST = std::convertible_to<std::vector<int>, Object>;
		RegisterObjectType<Array>("ARRAY", 1);
		static ConcreteType* basicArray = new ConcreteType{ "ARRAY", 0 };
		static ConcreteType* complexArray = new ConcreteType{ "ARRAY", 1 };

		std::vector<int> vars{ 1, 2, 3, 4, 5, 6, 7 };


		Object _array = MakeObject(vars);
		bool policy = _array.policy;
		
		logger::critical("Object check: {} , {} (+info {})", _array.get<Array>()._container->size(), policy, policy ? !!_array.policy->base : false);


		logger::info("~Testing PrintString for Arrays: \"{}\"", _array.PrintString());
		
		const int& thgb = vars[0];

		decltype(auto) d = std::move(thgb);

		//*
		Array new_array = _array.get<Array>();
		Variable fail = MakeObject(new_array);
		
		const std::vector<int> test{ new_array };

		

		std::vector<std::vector<int>> out{ {96, 69}, {402, 420}, {27, 72, 76}, {40, 42} };
		
		//*f
		Variable fail3 = MakeObject(std::move(out));

		Revariable<const std::vector<std::vector<int>>> revar1{};
		revar1(out, &fail3);
		
		//TODO: Array needs to be able to construct from arrays, this is not a good way to make stuff.
		// Also make ToObject easier to use. make it a real function seperate from ObjectTranslator.
		Array fail2 = ObjectTranslator<std::vector<std::vector<int>>>{}(out);

		Variable var = fail2;
		Revariable<Array&&> revar{};
		revar(fail2, &var);
		//*/

		//I NEED TO TEST THESE ADDRESSES ARE IN THE RIGHT SPOT

		for (auto& x : out)
		{
			logger::info("Addy: {:X}", (uintptr_t)&x);
			for (auto& y : x)
			{
				logger::info("addy: {:X}", (uintptr_t)&y);
			}
		}

		for (Variable& variable : new_array._container->data())
		{
			logger::critical(" var: {}", variable.AsNumber());
		}
		_array.get<Array>()._container->data()[4].ref() = 69;

		auto new_vars = Unvariable<std::vector<int>>{}(&fail);

		for (auto& variable : new_vars)
		{
			logger::critical(" new var: {}", variable);
		}
		//*/

		constexpr auto testers = std::derived_from<Revariable<std::vector<int>>, RefCollection>;

		logger::info("~Testing PrintString for Arrays(after): \"{}\"", _array.PrintString());

		auto gen_built = GenericDataBuilder<Array>().get<Array>();
	
		logger::critical("GenericBuilt check: {} ", gen_built._container ? gen_built._container->size() : 0);

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







	
	template <typename... T>
	struct NumberRepresent
	{};


	void Test2()
	{
		
		//This is an example of how we would represent non-existant number types within a native environment. Only the last one will count, 
		// but the others help establish it as a number. After that, it then proxies the base type it's trying to be and handles conversions before
		// and after.
		NumberRepresent<unsigned, bool> test;
	}


	namespace TestingRecordViewer
	{
		struct RecordHandler
		{

			using RecordType = SyntaxRecord;

			virtual void Destroy(const RecordType* record) const
			{
				delete record;
			}
			
			virtual std::string_view tag(const RecordType* record) const
			{
				return record->GetView();
			}

			virtual const RecordType::Inherit* GetInheritLayer(const RecordType* record) const//requires (non-zero inherited)
			{
				return record;
			}

			virtual const RecordType::Data& GetEnum(const RecordType* record) const//requires (non-void type) actually, this should mirror the handling of data 
			{
				return record->GetEnum();
			}


			virtual const RecordType* GetChild(const RecordType* record, size_t i) const
			{
				//returns null when it has no more to give. This is used privately to create a vector
				return std::addressof(record->GetChild(i));
			}


			static RecordHandler* GetSingleton()
			{
				static RecordHandler singleton;
				return std::addressof(singleton);
			}
		};


		struct BasicRecordView
		{
			using RecordType = SyntaxRecord;

			//TODO: Instead of a virtual record reader through and  through, what I'd like is to have the virtual nature outsourced to an object.
			// This would make it safe to tranfer, at the behest of the object it's records were created from.


			~BasicRecordView()
			{
				if (_managed)
					delete _target;
			}

			BasicRecordView(const RecordType& other) : _target{ std::addressof(other) }, _managed{ false }
			{

			}

			BasicRecordView(RecordType&& other) : _target{ new RecordType(std::move(other)) }, _managed{ true }
			{

			}





			const RecordType::Inherit* GetInheritLayer() const//requires (non-zero inherited)
			{
				return  _handler->GetInheritLayer(_target);
			}

			const RecordType::Data& GetEnum() const//requires (non-void type) actually, this should mirror the handling of data 
			{
				return  _handler->GetEnum(_target);
			}



			const RecordType::Inherit* operator->() const//requires (non-zero inherited)
			{
				return GetInheritLayer();
			}
			
			
			const RecordType* GetChild(size_t i)
			{
				//returns null when it has no more to give. This is used privately to create a vector
				return _handler->GetChild(_target, i);
			}

			//I GOT IT, in order to display the inherited stuff, it will have a function that displays the inherited layer, rather it will give a const pointer
			// to the inherited layer. This will give us the ability to control what we show, but also conditionally show stuff like syntax record without
			// giving away too much.

			std::string_view tag() const
			{
				return _handler->tag(_target);
			}
			//Collected here should be all the stuff we want to possibly have be standard. So children, parentage, etc etc. 
			// The problem is having it target something like a syntax record, it cannot inherit those special types.

		private:
			RecordHandler* const _handler = RecordHandler::GetSingleton();
			const RecordType* _target;
			bool _managed;

		};
		//template<typename T>
		//using BasicRecordViewer =  std::shared_ptr<Record

		void TestRecordViewer()
		{
			SyntaxRecord test;

			BasicRecordView viewer{ test };
			//viewer->;
			//RecordViewer test;
		}

	}

	namespace ClassStructSystem
	{
		//This type would serve for something to link to a function with generic structures.
		// Like the ref types, this would basically alias the object. I don't think I'll require this however.
		template <typename Type, StringLiteral... TempArgs>
		using generic_type = int;

		template <StringLiteral... TempParams>
		struct GenericFunctionTag {};


		//Will be used to represent custom class objects, preventing it from being instantiated
		template <StringLiteral TypeName>
		using class_type = int;

		//Will be used to represent custom struct objects, preventing it from being instantiated
		template <StringLiteral TypeName>
		using struct_type = int;


		//This represents an enum value, with the type
		template <StringLiteral TypeName>
		using enum_type = int;


		struct test_struct_class
		{

			
			template <typename T>
			T member(std::string_view name)
			{
				//This is how one would access a class struct, however, instead of doing it like this, I'd like something similar to a native_reference,
				// a scripted reference. Which basically would conversely serve to take a native object and impose changes on a scripted variable.
				// notably, by doing this, I can have referenciable return types
				return {};
			}
		};



		namespace complete_reference_coverage
		{
			//This is an idea by which external references can cover native to native, scripted to native, and native to scripted references.
			// Think the issue of returning a reference parameter, but that parameter leads to a scripted object. That's what this attempts to resolve.
			// An object that serves as the handler for that, something similar to the local ref types, aliasing themselves as that object while serving
			// as a reference to either a scripted object or a native one.

			//
		}


		class ClassStruct
		{

			TypeInfo* type = nullptr;

		private:
			union
			{
				uintptr_t			_raw = 0;
				Variable* memberList;
				RuntimeVariable* runtimeList;
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

	

}




namespace LEX
{
	inline int tv = 1;
	int& function(int t)
	{
		std::vector<int> m{ 1, 23, 4, 5, 6 };
		return m[1];

		//static int v = 5;
		//return v;
		//So when using a function like this as a procedure, it will check the address of the returned against the addresses of all the 
		// entries in the tuple, if it matches no entries in the tuple, and/or falls within a range
		
		
		//Some notice should be given, I think I'll largely make it ill advised to use such a thing.
		
		return tv;

		
	}


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
