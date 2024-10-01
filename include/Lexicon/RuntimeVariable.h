#pragma once

#include "Variable.h"


namespace LEX
{

	using RunValue = std::variant<Void, Variable, std::reference_wrapper<Variable>>;

	

	
	enum struct RuntimeVarType
	{
		Void,
		Value, 
		Ref,
	};
	struct RunVarData
	{

		using SizeType = std::_Variant_index_t<std::variant_size_v<RunValue>>;

		static constexpr auto req_size = 8 - sizeof(SizeType);

		//amount of references to a given variable. signed to allow it to go below for easy error reading.
		mutable int32_t refs = 0;

	};
	static_assert(sizeof(RunVarData) <= RunVarData::req_size, "RunVarData must equal the size of the padding in RunTypes.");




	struct RunDataHelper
	{
		enum Flag
		{
			None	= 0,
			Init	= 1 << 0,
			Refr	= 1 << 1,
			Free	= 1 << 2,	//A given runtime variable has freed its index but retains a pointer.
		};


		//This will help clear the Variable data spot without me having to put clear in every constructor. Hopefully.

		using _Ref = std::reference_wrapper<Variable>;


		static constexpr auto offset = sizeof(RunValue) - sizeof(RunVarData);

		RunVarData& GetData()
		{
			auto a_this = (uintptr_t)this;

			return *reinterpret_cast<RunVarData*>(a_this + offset);
		}

		const RunVarData& GetData() const
		{
			auto a_this = (uintptr_t)this;

			return *reinterpret_cast<RunVarData*>(a_this + offset);
		}

		RunValue& GetValue()
		{
			return *reinterpret_cast<RunValue*>(this);
		}

		const RunValue& GetValue() const
		{
			return *reinterpret_cast<const RunValue*>(this);
		}

		size_t index() const
		{
			return GetValue().index();
		}


		const RunDataHelper* GetRefHelper() const
		{

			constexpr auto ref_index = variant_index<RunValue, _Ref>();

			const RunValue& a_this = GetValue();

			const RunDataHelper* ret;

			switch (a_this.index())
			{
			case 1:
				ret = this; break;

			case 2: 
				ret = reinterpret_cast<RunDataHelper*>(&std::get<_Ref>(a_this).get()); break;


			default:
				ret = nullptr;
			}


			return ret;
		}


		protected:

		auto Inc() const
		{
			//logger::critical("inc {:X}", (uintptr_t)this);
			return ++GetData().refs;
		}

		auto Dec() const
		{
			//logger::critical("dec {:X}", (uintptr_t)this);
			return --GetData().refs;
		}

		void Unhandle()
		{
			if (index() == 2)
			{
				GetRefHelper()->Dec();
			}
			else if (index() == 1)
			{
				if (auto refs = GetData().refs; refs) {
					report::runtime::critical("{} refs remaining for run var ending {:X}", refs, (uintptr_t)this);
				}
			}
		}
		
		void Handle(const RunDataHelper* other, Flag flags = Flag::None)
		{
			if (flags & Flag::Init) {
				GetData().refs = 0;
			}
			if (other && (flags & Flag::Refr || other->index() == 2))
			{
				auto* help = other->GetRefHelper();

				other->GetRefHelper()->Inc();
			}
			



		}

		void Handle(const RunDataHelper& other, Flag flags = Flag::None)
		{
			return Handle(&other, flags);
		}

		public:


			int32_t Refs() const
			{
				return GetData().refs;
			}


		
		RunDataHelper()
		{
			Handle(nullptr, Flag::Init);
		}

		RunDataHelper(const RunDataHelper& other)
		{
			Handle(other, Flag::Init);
	
		}


		RunDataHelper(RunDataHelper&& other)
		{
			Handle(other, Flag::Init);
		}
		

		RunDataHelper& operator=(const RunDataHelper& other)
		{
			Unhandle();
			Handle(other);
			return *this;
		}



		RunDataHelper& operator=(RunDataHelper&& other)
		{
			Unhandle();
			Handle(other);
			return *this;
		}
	};





	class RuntimeVariable : protected RunDataHelper, public ClassAlias<std::variant<Void, Variable, std::reference_wrapper<Variable>>>
	{
	public:
		
		ALIAS_HEADER;

	private: 

		//TODO: Ref count idea for RuntimeVariable below
		// The concept is for every reference to a Variable that's housed within the runtime variable, that's an additional reference.
		//  When a runtime variable that holds a simple variable ends, it will check if the ref count is 0. 
		//  If not, it will detect there's undefined behaviour, as there's a reference pointing to nothing somewhere.
		// On the other hand, if it's a reference, it carries an offset of bytes to the helper data (this helps with changing sizes).
		// For this element, I can implement a similar helper to variable, it would look like this:
		//struct Helper
		//{
		//	TestB& operator=(TestB&)
		//	{
		//		//Helping here
		//	}
		//};
		//struct RuntimeVariable : public Helper, ClassAlias<>...


		void _CheckAssign(RuntimeVariable& other)
		{
			if (IsEmpty() == true)
				return;

			if (other.IsEmpty() == true)
				report::runtime::error("Assigning undefined to a non-undefined?");

			//This should only be checking if this has a type.
			Ref().CheckAssign(GetVariableType(*other));

			//Conversion must be a type conversion.
			//if (other->IsConvertibleTo(_type) == false)
			//	report::runtime::error("No type conversion between values.");
		}


	public:


		~RuntimeVariable()
		{
			Unhandle();
		}

		using VariableRef = std::reference_wrapper<Variable>;

		Variable& Ref()
		{
			get_switch (index())
			{
			default:
				report::runtime::critical("RuntimeVariable is undefined. (val = {})", switch_value);
				throw nullptr;

			case 1:
				return std::get<Variable>(*this);

			case 2:
				return std::get<VariableRef>(*this);
			}
		}


		RuntimeVariable& AssignRef(RuntimeVariable& other)
		{
			_CheckAssign(other);
	
			*this = other;

			return *this;
		}

		RuntimeVariable& AssignRef(RuntimeVariable&& other)
		{
			return AssignRef(static_cast<RuntimeVariable&>(other));
		}

		RuntimeVariable& AssignRef(Variable& other)
		{
			RuntimeVariable wrap = std::ref(other);


			return AssignRef(wrap);
		}

		Variable* Ptr()
		{
			return &Ref();
		}

		RuntimeVariable AsRef()
		{
			//This function turns this into a reference even if it's not one. IE, it will make a runtime that refers
			// to this. Long story short, ensures it's passed by ref not val. Use with caution.
			RuntimeVariable result = std::ref(Ref());
			
			result.Handle(this, Flag::Refr);

			return result;
		}

		size_t index() const
		{
			return Alias::index();
		}

		void Clear()
		{
			if (index() == 0)
				return;

			*this = Void{};
		}



		Variable* operator->()
		{
			if (IsEmpty() == true)
				report::runtime::critical("RuntimeVariable is undefined, cannot be accessed.");

			return &Ref();
		}


		Variable& operator*()
		{
			if (IsEmpty() == true)
				report::runtime::critical("RuntimeVariable is undefined, cannot be accessed.");

			return Ref();
		}

		//Used to use is void, but this checks if the ref is void too.
		bool IsVoid() { return !index() || Ref().IsVoid(); }

		bool IsEmpty() const { return !index(); }


		bool IsValue() const
		{
			return index() == 1;
		}


		bool IsReference() const
		{
			return index() == 2;
		}

		operator Variable()
		{
			return Ref();
		}
		//Here's how it should work. This needs a wrapper for Variable&. When this object is deleted it's given a callback to hold onto when deleted.
		// When deleted and is used in a place that doesn't allow deletion
	};
	static_assert(sizeof(RuntimeVariable) == sizeof(RunValue), "blah blah blah");
}