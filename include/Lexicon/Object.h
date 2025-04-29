#pragma once

#include "Lexicon/ObjectInfo.h"
#include "Lexicon/ObjectData.h"
#include "Lexicon/ObjectPolicy.hpp"
#include "Lexicon/ObjectPolicyHandle.h"

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
	

	//TODO: Use a different constraint for this, it may not need to actually take a reference, for pointer
	// types that'd be a bit silly.
	//Also, ToObject and the object translator need to switch types.
	template<typename T>
	using obj_trans_type = std::invoke_result_t<ObjectTranslator<std::remove_cvref_t<T>>, const T&>;

	template<typename T>
	concept object_type = has_object_info<obj_trans_type<T>> && !std::is_same_v<obj_trans_type<T>, detail::not_implemented>;


	
	template <object_type Ty>
	Object MakeObject(Ty&& var);

	template <object_type T>
	Object MakeObject(const T& var)
	{
		return MakeObject<const T>(std::move(var));
	}



	struct Object
	{

		Object() = default;


		Object(ObjectData d, uint32_t i, ObjectDataType t)
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
			logger::debug("addy 2 {:X}", _data.fstVal);
			Unhandle(&other);
			Transfer(other, false);

		}

		Object(Object&& other)
		{
			//Do you actually want to unhandle here?
			Unhandle(&other);

			Transfer(other, true);
		}

		//TODO: For the same of this, MakeObject needs to be a little more const.
		template <object_type T>
		Object(const T& other)
		{
			*this = MakeObject(other);
		}


		Object& operator=(const Object& other)
		{
			Unhandle(&other);
			return Transfer(other, false);
		}


		Object& operator=(Object&& other)
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
				auto type = obj->policy->GetTypeResolved(obj->data());
				
				return type;	
			}
				//_data()
			//SpecializeType(ObjectData&, ITypeInfo * type) override
			
			//return core type instead.
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
				logger::trace("Object has already empty.");
				return;

			case ObjectDataType::kVal:
			case ObjectDataType::kPtr:
				policy->Destroy(_data);
				_data = policy->CreateData();
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
		Object& _BasicTransfer(Object& other, bool move)
		{
			//The assumption is that other isn't valid, and thus, no care needs to be taken in transfering information.


			policy = other.policy;
			type = other.type;

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
		



		Object& _SimpleTransfer(Object& other, bool move) 
		{
			//The assumption is that other isn't valid, and thus, no care needs to be taken in transfering information.
			_data = other._data;

			if (move)
				other._data.Clear();

			return _BasicTransfer(other, move);
		}

		
		Object& _AdvancedTransfer(Object& other, bool move)
		{//This assumes there's precious data to be transfered.
			
			//Transfers delete what data existed, so this needs to be reinitialized
			if (type == ObjectDataType::kNone) {
				logger::debug("addy 3a {:X}", _data.fstVal);
				_data = other.policy->CreateData();
				logger::debug("addy 3b {:X}", _data.fstVal);
			}
			if (move) {
				other.policy->Move(_data, other._data);
				other._data.Clear();
			}
			else {
				other.policy->Copy(_data, other._data);
			}

			
			return _BasicTransfer(other, move);
		}



		Object& Transfer(Object& other, bool move)
		{
			logger::debug("transfer, move {}", move);
			_ClearCheck();
			other._ClearCheck();

			switch (other.type)
			{
			case ObjectDataType::kRef:
				if (*this == other) {
					logger::debug("Skipping transfer, data is equal.");
					return *this;
				}
				if (!move)//If it's not a move, both retain their data, as such this is a new reference.
					other.policy->IncPoolRef(other._data.idxVal);

				[[fallthrough]];
			case ObjectDataType::kNone:
				return _SimpleTransfer(other, move);

			case ObjectDataType::kVal:
			case ObjectDataType::kPtr:
				return _AdvancedTransfer(other, move);

			default:
				//invalid.
				throw temp_objectExcept;
			}
			
		}



		

		Object& Transfer(const Object& other, bool move)
		{
			return Transfer(const_cast<Object&>(other), move);
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
		ObjectData& data()
		{
			switch (type)
			{
			case ObjectDataType::kNone://None might not report error here.
				report::fault::critical("object is empty. Cannot access data.");

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

		ObjectData& data() const
		{
			return const_cast<Object*>(this)->data();
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
		T* fetch()
		{
			if (Is<T>() == false)
				return nullptr;

			return std::addressof(get<T>());
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
				logger::info("error, cannot convert, {} vs {}", policy.index(), index);
				throw temp_objectExcept;
			}

			switch (type)
			{
			case ObjectDataType::kNone:
				logger::info("object is empty");
				throw temp_objectExcept;

			case ObjectDataType::kVal:
			case ObjectDataType::kPtr:
				return _data.ptr<T>();

			case ObjectDataType::kRef:
				return policy->RequestPool(_data.idxVal)->ptr<T>();

			default:
				logger::info("object data type not found");
				throw temp_objectExcept;
			}
		}


		std::string PrintString() const;




		ObjectData _data{};

		ObjectPolicyHandle policy{};

		ObjectDataType type = ObjectDataType::kNone;

	};
	REQUIRED_SIZE(Object, 0x10);






	
	//This likely can be relocated within object as create, with MakeObject being an external function.
	template <object_type Ty>
	Object MakeObject(Ty&& var)
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

		result.policy = GetObjectPolicyID<ObType>();



		if (!result.policy) {
			report::runtime::critical("no policies");
		}

		ObjectVTable* vtable = GetObjectInfo<ObType>();

		if (policy->IsCompatible(vtable) == false) {
			report::runtime::critical("incompatible policies");
		}

		//This should get handled in compatibility testing.
		if (result.policy)
		{
			//TODO: confirm this against what goes in the policy. This is when we can tell that something is far too old for the placement.
			auto* vtable = GetObjectInfo<ObType>();
			auto* test = result.policy.get();
			//assert(result.policy->base);

			if (result.policy->base != vtable) {
				//Non-inhouse checks (should only happen once.
				report::fault::critical("issue");
			}
		}



		//Around here, I'd actually like there to be some sort of type trait that will be able to parse if it's a reference or not, as to not copy
		// if it doesn't have to.
		//auto data = ToObject<T>(var);
		Res data = ObjectTranslator<T>{}(var);

		//This should be the raw type, no const, no pointer. See to it this is made pure.
		using _Pure = decltype(data);//NOTE, find out the return type before hand, that way if it's a reference we can handle that properly.


		//TODO: HANDLE POOLED DATA HERE.
		constexpr bool stor = object_storage_v<ObType>;



		//ObjectData to = FillObjectData<ObType>(data);
		ObjectData to{ data };




		if (policy->IsPooled(to) == true) {
			result._data = policy->InitializePool(to, stor);
			result.type = ObjectDataType::kRef;
		}
		else {
			result._data = to;
			if constexpr (stor) {
				result.type = ObjectDataType::kVal;
			}
			else {
				result.type = ObjectDataType::kPtr;

			}
		}

		policy->Initialize(result.data());

		//I would rather construct object on the spot here so we don't trigger any assignments
		return result;
	}


}