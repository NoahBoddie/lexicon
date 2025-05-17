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
			Ptr		= 1 << 2,	//Should establish a pointer ref, and needs no ref value. Best used when it's unknown if var is a RuntimeVariable
			Free	= 1 << 3,	//A given runtime variable has freed its index but retains a pointer.
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
		/*
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
		//*/
		RunValue& GetValue()
		{
			return *reinterpret_cast<RunValue*>(this);
		}

		const RunValue& GetValue() const
		{
			return *reinterpret_cast<const RunValue*>(this);
		}

		Type index() const
		{
			return static_cast<Type>(GetValue().index());
		}

		bool IsReference() const
		{
			return index() == kReference;
		}


		const Variable* GetRefVariable() const
		{

			const RunValue& a_this = GetValue();

			switch (a_this.index())
			{
			case kReference: 
				return std::addressof(std::get<_Ref>(a_this).get());


			default:
				return nullptr;
			}
		}


	protected:

		
	public:
		//bool IsRefNegated() const
		//{
		//	return index() == kReference && Refs();
		//}
	private:
		//void SetNegate(bool value) const 
		//{
			//auto var = GetRefVariable();
			//if (var)
			//var->GetData().refs = value ? -1 : 0;
		//}

	protected:

		//void TrySetNegated(bool value) const
		//{
		//	if (index() == kReference)
		//	{
		//		SetNegate(value);
		//	}
		//}

		void Unhandle()
		{
			if (index() == kReference)
			{
				//if (!Refs())
				GetRefVariable()->Dec();
			}
			else if (index() == kVariable)
			{
				//if (auto refs = GetData().refs; refs) {
				//	report::runtime::critical("{} refs remaining for run var ending {:X}", refs, (uintptr_t)this);
				//}
			}
		}
		
		void Handle(const Variable& var)noexcept
		{
			//if (index() == kReference)
			{
				//auto* help = other->GetRefHelper();
				//if (!other->IsRefNegated())
				var.Inc();
			}
			



		}

		void Handle(const RunDataHelper& other) noexcept
		{
			if (auto var = other.GetRefVariable())
			{
				Handle(*var);
			}
		}

		public:


		int32_t Refs() const
		{
			return GetRefVariable()->GetData().refs;//GetData().refs;
		}

		constexpr RunDataHelper() noexcept = default;
		
		~RunDataHelper()
		{
			Unhandle();
		}

		RunDataHelper(const RunDataHelper& other)
		{
			Handle(other);
	
		}


		RunDataHelper(RunDataHelper&& other)
		{
			Handle(other);
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

		RunDataHelper(const VariableRef& other)
		{
			Handle(other.get());
		}


		RunDataHelper(VariableRef&& other)
		{
			Handle(other.get());
		}
		
		/*
		//I don't think this will actually do anything
		RunDataHelper& operator=(const VariableRef& other)
		{
			Unhandle();
			Handle(other.get());
			return *this;
		}
		RunDataHelper& operator=(VariableRef&& other)
		{
			Unhandle();
			Handle(other.get());
			return *this;
		}
		//*/
	};



	class RuntimeVariable : 
		protected RunDataHelper, 
		public ClassAlias<RunValue>
	{
	public:
		
		//IMPORTANT
		//The problem is effectively the inability to deal with direct settings from literally anything through variant. Namely, unhandling.
		// This is what the helper was for, and now this set up is littered with this sort of issue.

		//Maybe go back to using constructors, instead of defining assign operators, rely on things being constructed as a runtime variable
		// and then beig converted over

		ALIAS_CTOR;
		//using RunDataHelper::IsRefNegated;

		

	private: 
	public:


		struct VariableHandler
		{
			RuntimeVariable* self = nullptr;

			Variable* operator->() { self->TryUpdateVal();  return self->Ptr(); }

			const Variable* operator->() const { self->TryUpdateVal(); return self->Ptr(); }

			VariableHandler(const RuntimeVariable* a) : self{ const_cast<RuntimeVariable*>(a) } {};


			~VariableHandler()
			{
				self->TryUpdateRef();
			}
		};


		void _CheckAssign(const RuntimeVariable& other)
		{
			if (IsEmpty() == true)
				return;

			if (other.IsEmpty() == true)
				report::runtime::error("Assigning undefined to a non-undefined?");
			
			//This should only be checking if this has a type.
			//Ref().CheckAssign(GetVariableType(*other));

			//Conversion must be a type conversion.
			//if (other->IsConvertibleTo(_type) == false)
			//	report::runtime::error("No type conversion between values.");
		}


	public:

		/*
		static RuntimeVariable CreateTempRef(Variable* var)
		{
			if (!var)
				report::fault::critical("Variable for temporary ref is null");

			RuntimeVariable result = std::ref(*var);

			result.TrySetNegated(true);

			return result;
		}

		static RuntimeVariable CreateTempRef(Variable& var)
		{
			return CreateTempRef(&var);
		}
		//*/


		~RuntimeVariable()
		{
			//TryUpdateRef();//NOTE: TEMP IDEA EXPERIMENTAL
			//Unhandle();
		}

		//*



		RuntimeVariable(const RuntimeVariable& other) = default;


		RuntimeVariable(RuntimeVariable&& other) = default;
		
		RuntimeVariable(const VariableRef& other) :
			Alias{ other },
			RunDataHelper{ other }
		{
		}

		RuntimeVariable(VariableRef&& other) :
			Alias{ other },
			RunDataHelper{ other }
		{
		}
		/*
		RuntimeVariable(const Variable& other) : Alias{ other }
		{
		}


		RuntimeVariable(Variable&& other) : Alias{ other }
		{
		}
		//*/


		RuntimeVariable& operator=(const RuntimeVariable& other) = default;//{ return *this; }
		RuntimeVariable& operator=(RuntimeVariable&& other) = default;// { return *this; }
		/*

		RuntimeVariable& operator=(const VariableRef& other)
		{
			RunDataHelper::operator=(other);
			Alias::operator=(other);
			return *this;
		}


		RuntimeVariable& operator=(VariableRef&& other)
		{
			RunDataHelper::operator=(other);
			Alias::operator=(other);
			return *this;
		}
		//*/

		/*
		RuntimeVariable& operator=(const Variable& other)
		{
			Alias::operator=(other);
			return *this;
		}


		RuntimeVariable& operator=(Variable&& other)
		{
			Alias::operator=(other);
			return *this;
		}
	
		//*/
		//*/



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


		RuntimeVariable& TransferRef(const RuntimeVariable& other, bool move)
		{
			_CheckAssign(other);

			if (move)
				*this = other;
			else
				*this = other;
			return *this;
		}

		RuntimeVariable& AssignRef(const RuntimeVariable& other)
		{
			return TransferRef(other, false);
		}

		RuntimeVariable& AssignRef(RuntimeVariable&& other)
		{
			//This version may not be required
			return TransferRef(other, true);
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

		const Variable* Ptr() const
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
			
			//result.Handle(this, Flag::Refr);

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
		VariableHandler operator->() { return { this }; }

		const VariableHandler operator->() const { return { this }; }

		Variable& operator*() { return Ref(); }

		const Variable& operator*() const { return Ref(); }

		void TryUpdate(bool value)
		{
			if (index() == (int)kExternal)
			{
				auto& extern_ref = std::get<ExternalRef>(*this);
				extern_ref.Update(value);
			}
		}


		void TryUpdateVal()
		{
			TryUpdate(true);
		}


		void TryUpdateRef()
		{
			TryUpdate(false);
		}


		//Used to use is void, but this checks if the ref is void too.
		bool IsVoid() { return !index() || Ref().IsVoid(); }

		bool IsEmpty() const { return !index(); }

		bool IsDetachedRef() const { return index() == (int)kDetached; }



		bool IsValue() const
		{
			return index() == (int)kVariable;
		}

		bool IsReference() const
		{
			return index() >= kReference;
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