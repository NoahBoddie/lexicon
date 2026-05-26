#pragma once

#include "Lexicon/ProxyGuide.h"
#include "Lexicon/ObjectInfo.h"
#include "Lexicon/ObjectData.h"
#include "Lexicon/ObjectPolicy.hpp"
#include "Lexicon/ObjectPolicyHandle.h"
#include "Lexicon/Interfaces/ObjectPolicyManager.h"
namespace LEX
{
	struct TypeInfo;

	struct Object;




	inline Void temp_objectExcept{};

	//NOTE: Create object and some others will possibly be moved to be a class, due to lack of partial specialization.
	template <typename T>
	decltype(auto) ToObject(T& obj)
	{


		//Should create object not be implemented properly, it will send this.
		// Having some way to immediately implement a certain type might be nice. Some sort of concept that's easy to 
		// have objects flagged for implemented.
		
		if constexpr (has_object_info<T>)
			//If the given object has been seen as a type that has an object policy
			return obj;
		else
			return LEX::detail::not_implemented{};
		
	}
	

	//Turn this 
	template <typename T, typename = void>
	struct ObjectTranslator
	{
		//TODO: Get rid of R value checks. Literally pointless. The top level function needs to deal with that.
		
		//TODO: Just merge ObjectTranslator with ToObject and name it to object. More on why below
		// its obtuse to use and I can't use template specialization as well. That, and ProxyGuides will be using the type.

		using _Type = std::invoke_result_t<decltype(ToObject<T>), T&>;

		//No const for now, but later yes.

		//The gist is, if the 

		//TODO: A concept here, if a function that matches the exact rules of ToObject exists within the ObjectInfo function
		// and is static, allow it to use that instead. This is just to reduce the space taken up by different types all over.


		decltype(auto) operator()(const T& val)
		{
			//This bit is actually not needed at all.
			if constexpr (std::is_same_v<_Type, LEX::detail::not_implemented>) {
				return LEX::detail::not_implemented{};
			}
			else {
				return ToObject(val);
			}

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
	

	//TODO: Use a different constraint for this, it may not need to actually take a reference, for pointer
	// types that'd be a bit silly.
	//Also, ToObject and the object translator need to switch types.
	
	//TODO: use a required for this, it can fail disasterously
	template<typename T>
	using obj_trans_type = std::invoke_result_t<ObjectTranslator<std::remove_cvref_t<T>>, add_ref_to_non_ptr_t<qualify_extracted_type_t<T, std::add_const_t>>>;

	template<typename T>
	concept object_type = has_object_info<obj_trans_type<T>> && !std::is_same_v<obj_trans_type<T>, detail::not_implemented>;




	//Object context targets the objects being given to it and attempt to gleam context from it.
	// Useful on nullable types where it'd be useful to know what type it spawned from.
	template <typename T>
	struct ObjectContext
	{
		std::optional<uint16_t> operator()(const T& val)
		{
			return std::nullopt;

		}
	};


	template <typename T> requires (requires(ProxyGuide<T> guide, const T& arg) { { guide.UseObjectContext(arg) } -> std::convertible_to<std::optional<uint16_t>>; })
		struct ObjectContext<T>
	{
		decltype(auto) operator()(const T& obj)
		{
			return ProxyGuide<T>{}.UseObjectContext(obj);
		}
	};




	ENUM(ObjectFlag, uint8_t)
	{
		None = 0,
		HasContext = 1 << 0,
	};

	struct Object
	{
		template <object_type Ty>
		static Object MakeObject(Ty&& var)
		{
			//Name this ToObject, and the other MakeObject. This one creates the object, the other makes an argument into a thing convertible to an object.

			using T = std::remove_cvref_t<Ty>;

			using Res = obj_trans_type<T>;

			//Unsure if I wanna use cvref just yet
			//using _Type = std::remove_cvref_t<obj_trans_type<T>>;
			using ObType = std::remove_cvref_t<Res>;


			Object result{};

			//This is no extra trouble, given the values for ID are cached.
			ObjectPolicy* policy = GetObjectPolicy<ObType>();

			result.policy = policy->GetPolicyID();



			if (policy->IsCompatible(GetObjectVersion<ObType>()) == false) {
				report::runtime::critical("Type incompatibility detected");
			}


			//Around here, I'd actually like there to be some sort of type trait that will be able to parse if it's a reference or not, as to not copy
			// if it doesn't have to.
			//auto data = ToObject<T>(var);
			Res data = ObjectTranslator<T>{}(var);

			std::optional<uint16_t> context = ObjectContext<T>{}(var);

			if (context.has_value()) {
				result.SetContext(context.value());
			}


			//This should be the raw type, no const, no pointer. See to it this is made pure.
			using _Pure = decltype(data);//NOTE, find out the return type before hand, that way if it's a reference we can handle that properly.


			constexpr bool is_value_store = GetObjectStorage<ObType>() == ObjectStorage::Value;

			constexpr ObjectDataType data_type = is_value_store ? ObjectDataType::kVal : ObjectDataType::kPtr;

			//ObjectData to = FillObjectData<ObType>(data);
			ObjectData to{ data };


			
			TypeInfo* type = policy->GetTypeResolved(ObjectParams{ to, data_type, context });

			if (policy->IsPooled(type) == true) {
				result._data = policy->InitializePool(to, is_value_store);
				result.type = ObjectDataType::kRef;
			}
			else {
				result._data = to;
				result.type = data_type;
			}



			policy->Initialize(result.data(), type);

			//I would rather construct object on the spot here so we don't trigger any assignments
			return result;
		}


		//template <object_type T>
		//Object MakeObject(const T& var)
		//{
		//	return MakeObject<qualify_extracted_type_t<T, std::add_const_t>>(std::move(var));
		//}





		Object() = default;


		Object(ObjectData d, uint32_t i, ObjectDataType t)//, uint16_t offset)
		{
			//Make this intrinsic or whatever.
			_data = d;
			policy = i;
			type = t;
		}

		//A big ol note for these, it should not allow invalid reference data to be copied, so instead, it should give it an empty object data.


		//Similar to variable, assigning should be taking in the rules of the other before the rules of itself. For responsible assigning, I'll make an assign function.
		// For now, we assume everythings the same type.

		//I have no fucking clue how to do the originals (though I'm sure it's move, copy, swap) so I'm doing it manually for right now.
		Object(const Object& other)
		{
			Unhandle(&other);
			Transfer(other, false);

		}

		Object(Object&& other) noexcept
		{
			//Do you actually want to unhandle here?
			Unhandle(&other);
			Transfer(other, true);
		}

		//TODO: For the same of this, MakeObject needs to be a little more const.
		template <object_type T>
		Object(const T& other) //: Object{ MakeObject(other) } 
		{
			*this = MakeObject(other);
		}


		Object& operator=(const Object& other)
		{
			Unhandle(&other);
			return Transfer(other, false);
		}


		Object& operator=(Object&& other) noexcept
		{
			Unhandle(&other);
			return Transfer(other, true);
		}

		~Object()
		{
			Unhandle();
		}
		

		static TypeInfo* GetVariableType(const Object* obj)
		{
			if (obj)
			{
				auto type = obj->policy->GetTypeResolved(*obj);
				
				return type;	
			}
				//_data()
			//SpecializeType(ObjectData&, ITypeInfo * type) override
			
			//return a core type instead.
			return nullptr;
		}




		void Destroy()
		{
			//Destroying an object will do 2 things.
			// First, on references will delete the object being pointed to, as well as clearing the reference from here.
			// Second, it it's a value or pointer storage, it will delete what was previously there and place a new one.

			//An additional note, once references are cleared, I'll be using kNone type, BUT the handle stays.


			switch (type)
			{
			case ObjectDataType::kNone:
				report::message::trace("Object has already empty.");
				return;

			case ObjectDataType::kVal:
			case ObjectDataType::kPtr:
				policy->Destroy(_data);
				_data = policy->CreateData();//Why does this leave it with valid data?
				return;

			case ObjectDataType::kRef:
				type = ObjectDataType::kNone;
				policy->DestroyPool(_data.idxVal);
				policy->DecPoolRef(_data.idxVal);
				_data.Clear();
				return;
			}
		}

		//Hide me.
		void Unhandle(const Object* other = nullptr)
		{
			//This will not fully unhandle if the other side is matching and this type isn't a reference

			//That being said, if I can unhandle it selectively, and leave some data where ever it's being set, that might be good.
			// basically, that if it's not a reference, unhandle won't unload that. Then, for "this", if it's both an object/data type and
			// has the same object handle policy, it will not unhandle the data. Instead, letting an assignment take place.

			//Consideration needs to be spared for lhs data to be validated.

			switch (type)
			{
			
			case ObjectDataType::kVal:
			case ObjectDataType::kPtr:
				if (other && policy == other->policy && type == other->type)
					return;

				return policy->Destroy(_data);

			case ObjectDataType::kRef:
				//If they have the same policy, have the same type, and are both pooled to the same object, unhandle
				// shouldn't need to do anything in anticipation of the change.
				//-Problem is I need to condition the other side too.
				//Create an equals operator for this.
				if (!other || *this != *other){
					policy->DecPoolRef(_data.idxVal);
				}
				return;

			default:
				break;
			}
		}
		
		//If other is pooled, we have no need of creating data or any of that mess.

		//This part never changes
		Object& BasicTransfer(Object& other, bool move)
		{
			//The assumption is that other isn't valid, and thus, no care needs to be taken in transfering information.


			policy = other.policy;
			type = other.type;
			flags = other.flags;
			_context = other._context;
			if (move) {
				other.policy = ObjectPolicyHandle::invalid_index;
				other.type = ObjectDataType::kNone;
			}

			return *this;
		}


		//When move is used, it should be assumed that we are moving, and therefore, the stealing of resources is justified.
		// if we are copying, an effort should be made to preserve.
		//ADDITIONALLY HOWEVER, if a transfer is moving we also want to invalidate the other. This is because say we initialize something like a pool, and then
		// set it to something else. Transfer then will not need to adjust anything. BUT, if we don't steal that data, when the other dies, it takes the only reference with it.
		// So to be simple, steal it.
		



		Object& SimpleTransfer(Object& other, bool move) 
		{
			//The assumption is that other isn't valid, and thus, no care needs to be taken in transfering information.
			_data = other._data;

			if (move)
				other._data.Clear();

			return BasicTransfer(other, move);
		}

		
		Object& AdvancedTransfer(Object& other, bool move)
		{//This assumes there's precious data to be transfered.
			
			//Transfers delete what data existed, so this needs to be reinitialized
			if (type == ObjectDataType::kNone) {
				_data = other.policy->CreateData();
			}
			if (move) {
				other.policy->Move(_data, other._data);
				other._data.Clear();
			}
			else {
				other.policy->Copy(_data, other._data);
			}

			
			return BasicTransfer(other, move);
		}



		Object& Transfer(Object& other, bool move)
		{
			_ClearCheck();
			other._ClearCheck();

			switch (other.type)
			{
			case ObjectDataType::kRef:
				if (*this == other) {
					return *this;
				}
				if (!move)//If it's not a move, both retain their data, as such this is a new reference.
					other.policy->IncPoolRef(other._data.idxVal);

				[[fallthrough]];
			case ObjectDataType::kNone:
				return SimpleTransfer(other, move);

			case ObjectDataType::kVal:
			case ObjectDataType::kPtr:
				return AdvancedTransfer(other, move);

			default:
			{
				auto& new_other = other;
				report::critical("ObjectDataType is '{}'({}) and cannot be read. {}", magic_enum::enum_name(new_other.type), (int)new_other.type, new_other.GetContext().value_or(0));

			}
			}
			
		}



		

		Object& Transfer(const Object& other, bool move)
		{
			return Transfer(unconst(other), move);
		}

		void _ClearCheck() const
		{
			//Requires everything except handle to be mutable.
			//Should primarily check for object pooling invalidity. This should help to prevent dead references from extending their lifetimes.
		}


		//The idea is basically that one delegates it's tasks to an object that the policy caries, more on that later.

		//Roughly to just about anything you can think of, there should be no implicit conversions (unless handled specifically by something like "MakeObject"),
		// and instead functions that can be relied upon for failure recognition. So, instead of the operator for adding 2 objects, have a function for it that creates a 
		// new object. This way the use is an explicit choice, never an accident.

		TypeInfo* GetType()
		{
			//TODO: This needs filling out, please define GetType for Object

			//The concept of get type should be getting the object info, and submitting this to it. The base version should 
			return nullptr;
		}


		constexpr std::strong_ordering operator<=>(const Object& other) const noexcept = default;
		constexpr bool operator==(const Object& other) const noexcept { return operator<=>(other) == std::strong_ordering::equal; }

		/*
		constexpr std::strong_ordering operator<=>(const Object& other) const
		{
			if (auto result = policy <=> other.policy; result != std::strong_ordering::equivalent)
				return result;

			if (auto result = type <=> other.type; result != std::strong_ordering::equivalent)
				return result;

			if (auto result = type <=> other.type; result != std::strong_ordering::equivalent)
				return result;

			return std::strong_ordering::equivalent;

		}

		constexpr std::strong_ordering operator<=>(Object&& other) const
		{
			return operator<=>(other);
		}
		//*/


		//constexpr bool operator==(const Object& a_rhs) const
		//{
		//	return operator<=>(a_rhs) == std::strong_ordering::equal;
		//}


		std::partial_ordering Compare(const Object& other) const
		{
			return std::partial_ordering::unordered;
		}


		//Switch _data and data.
		ObjectData& data(bool allow_empty)
		{
			switch (type)
			{
			case ObjectDataType::kNone:
				//TODO: empty allowed will be ignored if the object stored is an object type.
				if (!allow_empty)
					report::fault::critical("object is empty. Cannot access data.");

				[[fallthrough]];
			case ObjectDataType::kVal:
			case ObjectDataType::kPtr:
				return _data;

			case ObjectDataType::kRef:
				return *policy->RequestPool(_data.idxVal);

			default:
				report::fault::critical("object data type {} not valid", magic_enum::enum_name(type));
				
			}

			//Use this function more plz.
		}

		ObjectData& data(bool allow_empty) const
		{
			return const_cast<Object*>(this)->data();
		}

		ObjectData& data()
		{
			return data(false);
		}

		ObjectData& data() const
		{
			return data(false);
		}


		template <has_object_info T>
		bool Is() const
		{
			auto index = GetObjectPolicyID<T>();
			return policy.index() == index;
		}

		//TODO: I may give these the chance to go strate

		//This needs the ability to get a pointer of the given type as well, something that should be used often with pooling types.
		template <has_object_info T>
		T& get()
		{
			//TODO: Object::get() has no guard rails at all. Please implement some.

			auto index = GetObjectPolicyID<T>();

			if (policy.index() != index) {
				report::error("error, cannot convert, {} vs {}", policy.index(), index);
			}

			switch (type)
			{
				case ObjectDataType::kNone:
					report::error("object is empty");

				case ObjectDataType::kVal:
				case ObjectDataType::kPtr:
					return _data.get<T>();

				case ObjectDataType::kRef:
					return policy->RequestPool(_data.idxVal)->get<T>();

				default:
					report::error("object data type not found");
			}
		}

		template <has_object_info T>
		const T& get() const
		{
			return unconst(this)->get<T>();
		}



		template <has_object_info T>
		T* fetch()
		{
			if (Is<T>() == false)
				return nullptr;

			return ptr<T>();
		}


		template <object_type T>
		explicit operator T ()
		{
			using TrueType = std::remove_cvref_t<obj_trans_type<T>>;

			decltype(auto) result = get<TrueType>();
			
			if constexpr (std::is_same_v<std::remove_cvref_t<TrueType>, std::remove_cvref_t<T>>)
				return result;
			else
				return static_cast<T>(result);
		}
		

		template <has_object_info T>
		T* ptr()
		{
			auto index = GetObjectPolicyID<T>();

			if (policy.index() != index) {
				report::apply::error("error, cannot convert, {} vs {}", policy.index(), index);
				throw temp_objectExcept;
			}

			switch (type)
			{
			case ObjectDataType::kNone:
				return nullptr;

			case ObjectDataType::kVal:
			case ObjectDataType::kPtr:
				return _data.ptr<T>();

			case ObjectDataType::kRef:
				return policy->RequestPool(_data.idxVal)->ptr<T>();

			default:
				report::fault::critical("object data type '{}' is unexpected", magic_enum::enum_name(type));
				throw temp_objectExcept;
			}
		}

		constexpr bool IsEmpty() const noexcept
		{
			return type == ObjectDataType::kNone;
		}

		bool IsValueZero() const
		{
			if (!IsEmpty() && policy) {
					return !policy->Exists(data());
			}
				
			return true;
		}


		std::string PrintString() const;


		bool HasFlag(ObjectFlag flag) const
		{
			return flags & flag;
		}

		void SetFlag(ObjectFlag flag, bool value) const
		{
			if (value)
				flags |= flag;
			else
				flags &= ~flag;
		}



		std::optional<uint16_t> GetContext() const noexcept
		{
			if (HasFlag(ObjectFlag::HasContext) == true) {
				return _context;
			}

			return std::nullopt;
		}

		void SetContext(uint16_t value) noexcept
		{
			SetFlag(ObjectFlag::HasContext, true);
			_context = value;
		}



		ObjectData _data{};

		ObjectPolicyHandle policy{};
		uint16_t _context{};
		ObjectDataType type = ObjectDataType::kNone;
		mutable ObjectFlag flags = ObjectFlag::None;
	};
	REQUIRED_SIZE(Object, 0x10);






	
	//This likely can be relocated within object as create, with MakeObject being an external function.

	template <object_type Ty>
	Object MakeObject(Ty&& var)
	{
		return Object::MakeObject(std::forward<Ty>(var));
	}

	//template <object_type T>
	//Object MakeObject(const T& var)
	//{
	//	return MakeObject<qualify_extracted_type_t<T, std::add_const_t>>(std::move(var));
	//}

}