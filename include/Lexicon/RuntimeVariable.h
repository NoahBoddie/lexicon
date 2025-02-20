#pragma once

#include "Variable.h"
#include "Lexicon/NativeReference.h"

namespace LEX
{
	using VariableRef = std::reference_wrapper<Variable>;
	using DetachedRef = std::shared_ptr<Variable>;
	using ExternalRef = NativeReference;//Rename pls
	using RunValue = std::variant<
		Void, 
		Variable, 
		VariableRef,
		DetachedRef,
		ExternalRef
	>;

	

	

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

		enum Type
		{
			kInvalid,
			kVariable,
			kReference,
			kDetached,
			kExternal,
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
			case kVariable:
			case kDetached:
			case kExternal:
				ret = this; break;

			case kReference: 
				ret = reinterpret_cast<RunDataHelper*>(&std::get<_Ref>(a_this).get()); break;


			default:
				ret = nullptr;
				break;
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
			if (index() == kReference)
			{
				GetRefHelper()->Dec();
			}
			else if (index() == kVariable)
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
			if (other && (flags & Flag::Refr || other->index() == kReference))
			{
				//auto* help = other->GetRefHelper();

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





	class RuntimeVariable : 
		protected RunDataHelper, 
		public ClassAlias<RunValue>
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


		void _CheckAssign(const RuntimeVariable& other)
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

		/*
		//Maybe use later?
		template <typename Self>
		decltype(auto) Ref(this Self&& self) {
			get_switch(self.index()) {
			default:
				report::runtime::critical("RuntimeVariable is undefined and cannot be accessed. (val = {})", switch_value);
				throw nullptr;
			case 1:
				return std::get<Variable>(self);
			case 2:
				return std::get<VariableRef>(self);
			}
		}
		/*/

		const Variable& Ref() const
		{
			get_switch (index())
			{
			default:
				report::runtime::critical("RuntimeVariable is undefined and cannot be accessed. (val = {})", switch_value);

			case kVariable:
				return std::get<Variable>(*this);

			case kReference:
				return std::get<VariableRef>(*this);

			case kDetached:
				return *std::get<DetachedRef>(*this);

			case kExternal:
				return std::get<ExternalRef>(*this).Ref();
			}
		}

		Variable& Ref()
		{
			return const_cast<Variable&>(std::as_const(*this).Ref());
		}
		//*/

		RuntimeVariable& AssignRef(const RuntimeVariable& other)
		{
			_CheckAssign(other);
	
			*this = other;

			return *this;
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
			//Detached RuntimeVariable refs aren't actually RuntimeVariables, so that should be 
			if (IsUniqueRef() == true)
				return *this;

			//This function turns this into a reference even if it's not one. IE, it will make a runtime that refers
			// to this. Long story short, ensures it's passed by ref not val. Use with caution.
			RuntimeVariable result = std::ref(Ref());
			
			result.Handle(this, Flag::Refr);

			return result;
		}

		RuntimeVariable&& Detach()
		{
			//Takes the Variable inside and moves it to a DetachedReference


			if (IsUniqueRef() == false) {
				Unhandle();//Unsure if this part is needed

				//bool is_ref = IsRuntimeRef();

				//auto& ref = Ref();
				*this = std::make_shared<Variable>(std::move(Ref()));
				
				//I don't think this is necessary, but I'm keeping it alive for now
				//if (is_ref) {
					//Here we also need to turn the place it came from into this very thing. This way we're still referencing it.
					// Note, if this isn't the only reference, it's gonna explode.
					//reinterpret_cast<RuntimeVariable&>(ref) = *this;
				//}
			}

			return std::move(*this);
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


		//TODO: Use ptr instead
		Variable* operator->() { return &Ref(); }

		const Variable* operator->() const { return &Ref(); }

		Variable& operator*() { return Ref(); }

		const Variable& operator*() const { return Ref(); }



		//Used to use is void, but this checks if the ref is void too.
		bool IsVoid() { return !index() || Ref().IsVoid(); }

		bool IsEmpty() const { return !index(); }

		bool IsDetachedRef() const { return index() == (int)kDetached; }


		bool IsValue() const
		{
			return index() == (int)kVariable;
		}


		bool IsRuntimeRef() const
		{
			return index() == (int)kReference;
		}

		bool IsExternRef() const
		{
			return index() == (int)kExternal;
		}

		bool IsUniqueRef() const
		{
			return IsExternRef() || IsDetachedRef();
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