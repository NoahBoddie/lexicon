#pragma once 

//*src
#include "Object.h"
#include "Lexicon/Variable.h"
#include "Lexicon/Revariable.h"
#include "Lexicon/GenericArray.h"
namespace LEX
{
    class Variable; 

    struct TypeInfo;
	//*
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
			GenericArray array{ nullptr, {self->type() } };

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
			_container{ std::make_unique<Helper>(std::vector<ArrayVariable>(range.begin(), range.end())) }, _type{ type }
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
	//*/
}