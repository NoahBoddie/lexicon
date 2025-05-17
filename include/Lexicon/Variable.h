#pragma once

#include "String.h"
#include "Number.h"
#include "Lexicon/TypeInfo.h"
//#include "Function.h"
//#include "ExternalHandle.h"
#include "Object.h"
#include "Array.h"
//#include "Delegate.h"
//#include "FunctionHandle.h"

#include "Reflection.h"

#include "TypeID.h"

#include "VariableType.h"

#include "Lexicon/Impl/common_type.h"



namespace LEX
{

	using VariableValue = std::variant
		<
		//Types that will not need to exist coming soon:
		// prompt, Index,
		Void,						    //Void given form. Invalidates all other types.
		Number,                         //Represents all numeric values. Integers, Floats, and Boolean values.
		String,
		Object,
		Reflection*
		>;




	using SizeType = std::_Variant_index_t<std::variant_size_v<VariableValue>>;

	constexpr auto req_size = 8 - sizeof(SizeType);

	ENUM(VariableFlag, uint16_t)
	{
		None,
			Defined = 1 << 0,       //Has the variable been successfully defined.
			//CreatedExtern = 1 << 1,     //Variable was created externally
			//ChangedOnce = 1 << 2,   //Variable was changed once. Rarely unset.
			//ChangedTwice = 1 << 3,  //If the variable was changed. Unset if being used in a reference function
			//I'd like to have some 



			//Actually, to reiterate, I think that temporary should be reversed. Persistent is given to any variable that starts
	};

#pragma pack(push, 1)
	struct VariableData
	{
		//Program the clearing of this padding into the component.

		mutable uint32_t refs = 0;

		mutable VariableFlag flags = VariableFlag::None;
		uint8_t changeID = 0;//Counts how many times it's been changed

		void SetChange(bool add)
		{
			if (add) {
				if (!++changeID) {
					changeID++;
				}
			}
			else {
				changeID = 0;
			}

		}

		void ClearChange()
		{
			changeID = 0;
		}

		bool HasFlag(VariableFlag flag) const
		{
			return flags & flag;
		}

		void SetFlag(VariableFlag flag, bool value)
		{
			if (value)
				flags |= flag;
			else
				flags &= ~flag;
		}




		bool IsDefined() const { return HasFlag(VariableFlag::Defined); }



		void Clear()
		{
			*this = VariableData();
		}
	};
	static_assert(sizeof(VariableData) <= req_size, "VarData must equal the size of the padding in VariableValue");

#pragma pack(pop)

	namespace
	{
		//Method to find an object or variable.

		template <class T, class U> struct is_one_of;

		template <class T, class... Ts>
		struct is_one_of<T, std::variant<Ts...>>
			: std::bool_constant<(std::derived_from<T, Ts> || ...)>
		{
		};


		template<typename T>
		concept ObjectClass = is_one_of<T, VariableValue>::value;

	}


	//TODO:Move get_index to a type_trait in RGL
	template<typename VariantType, typename T, std::size_t index = 0>
	constexpr std::size_t variant_index() {
		static_assert(std::variant_size_v<VariantType> > index, "Type not found in variant");
		if constexpr (index == std::variant_size_v<VariantType>) {
			return index;
		}
		else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
			return index;
		}
		else {
			return variant_index<VariantType, T, index + 1>();
		}
	}


	ENUM(VariableEnum, uint8_t)
	{
		Void,
		Number,
		String,
		Object,
		Reflect,
		Total,
	};




	struct DataHelper
	{
		//This will help clear the Variable data spot without me having to put clear in every constructor. Hopefully.

		~DataHelper() { Unhandle(); }
		
		
		DataHelper() { Clear(); }
		DataHelper(const DataHelper& other) { Clear(); }
		DataHelper(DataHelper&& other) { Clear(); }


		DataHelper& operator=(const DataHelper& other) = default;
		DataHelper& operator=(DataHelper&& other) = default;


		VariableData& GetData()
		{
			auto a_this = (uintptr_t)this;

			return *reinterpret_cast<VariableData*>(a_this + sizeof(VariableValue) - sizeof(VariableData));
		}

		const VariableData& GetData() const
		{
			auto a_this = (uintptr_t)this;

			return *reinterpret_cast<VariableData*>(a_this + sizeof(VariableValue) - sizeof(VariableData));
		}


		auto Inc() const
		{
			//logger::critical("inc {:X}", (uintptr_t)this);
			return ++GetData().refs;
		}

		auto Dec() const
		{
			auto& refs = GetData().refs;
			assert(refs);
			//logger::critical("dec {:X}", (uintptr_t)this);
			return --refs;
		}


		void Clear()
		{
			GetData().Clear();
		}



		void Unhandle() const
		{
			if (auto refs = GetData().refs; refs) {
				report::runtime::critical("{} refs remaining for run var ending {:X}", refs, (uintptr_t)this);
			}
		}





	


	};








	template <typename T1, typename T2>
	concept Assignable = requires(T2 base, T1 insert) {
		{ base.operator=(insert) };
	};

	template <typename T1, typename T2>
	concept Constructible = requires(const T1 & insert) {
		{ T2{ insert } };
	};

	//template <typename T>
	//TypeInfo* TypeOf()
	//{
		//The idea of this function is that it takes the core type and splits how it finds it's type.
		//For objects it should be their object interface that does it. However, part of this might be that the actual object will need to find that itself (if it can).
		// For example, delegates and function pointers will be able to use their real types to get that information
		//For Numbers the setting should do it
		//For Booleans it's obvious
		//For
		//return nullptr;
	//}

	struct Variable : public DataHelper, public RGL::ClassAlias<VariableValue>
	{
		ALIAS_HEADER;

		//Variable() = default;
	

	public:

		VariableValue& value() noexcept { return *this; }
		const VariableValue& value() const noexcept { return *this; }
		


	private:

		void SetDefined(bool val)
		{
			//Defined is now based upon the value that is used. To cover stuff like classes not being set up properly, I'm keeping the defined bit.
			GetData().SetFlag(VariableFlag::Defined, val);
		}




	public:


		TypeInfo* GetTypeInfo() const;


		bool IsDefined() const
		{
			//TODO: Use IsDefined to determine definition. It will be vital at some point.
			return !IsVoid();// && value().GetData().IsDefined();
		}

		bool IsChanged() const
		{
			return GetData().changeID;
		}

		//So typeinfo is something I'll be putting right here. If the value is 0xFFFFFFFF or something rather other,
		// it's any. 0, it's like bare bones default.
		//So, if it exists, we include the value.

		constexpr std::strong_ordering operator<=>(const Variable& other) const
		{
			//The type doesn't matter in this.
			return value() <=> other.value();
		}

		constexpr bool operator==(const Variable& other) const
		{
			return operator<=>(other) == std::strong_ordering::equal;
		}

		VariableEnum GetVariableEnum() const
		{
			return GetVariableEnum();
		}

		bool IsVariableEnum(VariableEnum type) const
		{
			return GetVariableEnum() == type;
		}

		//These should also maybe check the policies?
		bool IsVoid() const { return std::holds_alternative<Void>(value()); }
		bool IsObject() const { return std::holds_alternative<Object>(value()); }
		bool IsNumber() const { return std::holds_alternative<Number>(value()); }
		bool IsString() const { return std::holds_alternative<String>(value()); }




		Number& AsNumber() { return std::get<Number>(value()); }

		String& AsString() { return std::get<String>(value()); }

		Object& AsObject() { return std::get<Object>(value()); }
		//Array AsArray() { throw nullptr; }

		Object* FetchObject()
		{
			if (IsObject() == false) {
				return nullptr;
			}

			return std::addressof(AsObject());
		}


		void Clear()
		{
			value() = Void{};
			//_SetDefined(false);
			//_SetChanged(false);
		}

		//destroy
		void SetVariableEnum__(VariableEnum a_rhs)
		{
			//When doing something like this, clear qualifiers

			switch (a_rhs)
			{
			case VariableEnum::String:     value() = ""; break;
				//case VariableEnum::Number:     value() = (Number)0; break;

			case VariableEnum::Object:     value() = Object{}; break;

			default:    throw nullptr;// This is in error, just have no idea how.
			}
		}




		Variable& Transfer(const Variable& other, bool move)
		{

			if (move)
				value() = other.value();
			else
				value() = other.value();



			//_SetDefined(true);
			//_SetChanged(true);



			return *this;
		}


		Variable& Assign(const Variable& other)
		{
			return Transfer(other, false);
		}

		Variable& Assign(Variable&& other)
		{
			return Transfer(other, true);
		}


		//TODO: Conversion the non-variable assign with an AssignImpl function
		template <Constructible<VariableValue> T>
		Variable& Assign(const T& other)
		{
			//I will make a transfer pivot for this later
			//if (move)
			//    value() = std::forward<T>(other);
			//else
			value() = other;

			//_SetDefined(true);


			return *this;
		}


		static TypeInfo* GetVariableType(const Variable* it)
		{
			if (it) {
				return it->GetTypeInfo();
			}

			return common_type::object();
		}



		//template <class _Ty, enable_if_t<!is_same_v<_Remove_cvref_t<_Ty>, variant>
		//&& is_constructible_v<_Variant_init_type<_Ty, _Types...>, _Ty>
		//    && is_assignable_v<_Variant_init_type<_Ty, _Types...>&, _Ty>,
		//    int> = 0>
		//_CONSTEXPR20 variant& operator=(_Ty&& _Obj) noexcept(


		//If it's the exact thing it should try to give a reference I think
		template<variant_convertible_from<VariableValue> To>
		explicit operator To()
		{

			To result = {};

			bool triggered = false;

			std::visit([&](auto&& self) {
				using From = std::remove_reference_t<std::remove_cv_t<decltype(self)>>;

				if constexpr (stl::castable_to<From, To>) {
					triggered = true;
					result = static_cast<To>(self);

				}
				}, value());

			if (!triggered)
				report::critical("Variable of value {} cannot convert to {}.", PrintString(), typeid(To).name());

			return result;
		}


		template<variant_convertible_from<VariableValue> To, size_t I = 0>
		inline bool CanCastTo() const
		{
			//make one without the I, make that the impl version
			constexpr size_t k_index = convertible_variant_index_v<VariableValue, To, I>;


			if (value().index() == k_index)
				return true;

			if constexpr (k_index != -1)
			{
				if constexpr (constexpr size_t _I = k_index + 1; _I < std::variant_size_v<VariableValue>)
					return CanCastTo<To, _I>();

			}

			return false;
		}

		RuntimeVariable Convert(TypeInfo* to);


	private:

		template<typename T>
		constexpr static bool Workaround()
		{
			return requires(const T & t)
			{
				{ t.PrintString() } -> std::convertible_to<String>;
			};
		}


	public:

		std::string PrintString() const
		{
			String string = std::visit([this](auto&& self) -> String {
				using T = std::decay_t<decltype(self)>;

				constexpr bool has_func = Workaround<T>();

				if constexpr (has_func) {
					return self.PrintString();
				}
				else if constexpr (std::is_same_v<T, String>) {

					return std::format("\"{}\"", self.view());
				}
				else if constexpr (std::is_same_v<T, Void>) {
					return "void::()";
				}
				else {
					return std::format("{}::(inv)", magic_enum::enum_name(GetVariableEnum()));
				}
				}, value());

			return string;
		}
	};
	static_assert(sizeof(Variable) == (sizeof(VariableValue)), "Size of Variable Component and Value must be equal for data helper to work.");
	



	struct Voidable : public Variable
	{
		//Does this need a proxy guide?

		using Variable::Variable;
		using Variable::operator=;


		static TypeInfo* GetVariableType(const Voidable* it)
		{
			if (it) {
				Variable::GetVariableType(it);
			}

			//This type could be anything, even void. As such, the representation of a Variable is the basic interface voidable.
			return common_type::voidable();
		}

	};



}




/*

    //Represents the any type, may use just so Any isn't mistaken for void.
    //That, or I could have any rep the type it actually is.
    struct Any {};
    using Property = int64_t;


    //This represents the std::file_location type deal. This is more interface stuff.
    CStringW thisDllDirPath()
    {
        //This is sorta system I'd like to use to associate plugin instances with who's calling for what.
        // I could actually use this plus an external functions to call upon th interface system.

        CStringW thisPath = L"";
        WCHAR path[MAX_PATH];
        HMODULE hm;
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPWSTR)&thisDllDirPath, &hm))
        {
            GetModuleFileNameW(hm, path, MAX_PATH);
            PathRemoveFileSpecW(path);
            thisPath = CStringW(path);
            if (!thisPath.IsEmpty() &&
                thisPath.GetAt(thisPath.GetLength() - 1) != '\\')
                thisPath += L"\\";
        }
        else if (_DEBUG) std::wcout << L"GetModuleHandle Error: " << GetLastError() << std::endl;

        if (_DEBUG) std::wcout << L"thisDllDirPath: [" << CStringW::PCXSTR(thisPath) << L"]" << std::endl;
        return thisPath;
    }


//*/