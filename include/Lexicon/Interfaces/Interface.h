#pragma once

namespace LEX
{

	//Give own file.
	enum struct Update
	{
		Missing,//desired plugin doesn't even exist.
		Match,//Update versions match
		Invalid,//Query is invalid
		Library,//Library is out of date
		Engine,//Engine is out of date

	};

	struct Interface
	{
		virtual ~Interface() = default;

		//Used to help see if a request is valid. This should only be incrementing when the object has a new iteraction of
		// it's vtable basically. There's an incompatibility for requests newer than can be provided.
		virtual uintptr_t Version() const = 0;

		//Used for specific handling of versions. Say a function has to change functionality and the expectation is no longer valid.

		virtual bool CanHandle(uintptr_t) const { return true; }
	
	protected:
		Interface() = default;

		Interface(Interface&&) = delete;
		Interface(const Interface&) = delete;

		Interface& operator=(Interface&&) = delete;
		Interface& operator=(const Interface&) = delete;
	};

}


//TODO:I'm storing this idea here for now, but please, move it later.


namespace api
{
	namespace detail
	{
		template <typename T>
		concept container_type = requires(const T& t) 
		{ 
			{ t.size() } -> std::same_as<size_t>;
		} 
		&&
		requires(const T& t)
		{
			{ t.data() };
		};

	}
	

	//Should ONLY be used in parameters, or as the return type of a function where it points to relatively persistent data. 
	template <detail::container_type T> requires requires { typename T::const_pointer; }
	struct container final
	{
		using const_pointer = T::const_pointer;
		using elem = T;




		~container()
		{
			Clear();
		}

		constexpr container() = default;

		container(const elem& e) : _data{ e.data() }, _size{ e.size() } {}

		container(elem&& e) : container{ e } {}


		container(const container& other)
		{
			Obtain(other._data, other._size);
		}

		container(container&& other)
		{
			Move(other);
		}



		container& operator= (const elem& e)
		{
			return Obtain(e.data(), e.size());
		}

		container& operator= (elem&& e)
		{
			return Obtain(e.data(), e.size());
		}


		container& operator= (const container& c)
		{
			return Obtain(c._size, c._size);
		}

		container& operator= (container&& c)
		{
			return Move(c);
		}

		//make const version of this, ensure that it only exists in the event that the self actually has a [].
		auto& operator[] (size_t pos)
		{
			auto& self = get();

			return self[pos];
		}


		elem& get()
		{
			switch (GetMode())
			{
			case kEmptyMode:
			case kPointerMode:
				Obtain(_data, _size);
				break;

			case kElementMode:
				break;
			}

			return *_elem;
		}

		constexpr bool empty() const
		{
			return GetMode() == kEmptyMode;
		}

		operator elem& ()
		{
			return get();
		}


		elem& operator*()
		{
			return get();
		}



		elem* operator->()
		{
			return &get();
		}

		const_pointer data() const
		{
			switch (GetMode())
			{
			
			case kPointerMode:
				return _data;
				
			case kElementMode:
				return _elem->data();
			}
		
			return nullptr;
		}

		size_t size() const
		{
			switch (GetMode())
			{
			case kPointerMode:
				return _size;

			case kElementMode:
				return _elem->size();

			}
			
			return 0;
		}

	private:
		//Move some of this to container base so it doesn't make extra functions. Though, this shit is probably inline-able
		enum Enum
		{
			kEmptyMode,
			kPointerMode,
			kElementMode,
		};

		void Clear()
		{
			if (GetMode() == kElementMode) {
				delete _elem;
			}
		}



		elem& Obtain(const_pointer data, size_t size)
		{
			Clear();

			_elem = size ? new elem(data, data + size) : new elem;
			_size = k_elementMode;

			return *_elem;
		}

		container& Move(container& other)
		{
			_raw = other._raw;
			_size = other._size;

			other._raw = 0;
			other._size = 0;

			return *this;
		}


		Enum GetMode() const
		{
			if (!_raw)
				return Enum::kEmptyMode;

			if (!_size)
				return Enum::kEmptyMode;

			if (_size == k_elementMode)
				return Enum::kElementMode;

			return kPointerMode;

		}


		static constexpr size_t k_elementMode = -1;

		union
		{
			uint64_t _raw = 0;
			const_pointer _data;
			elem* _elem;
		};
		size_t _size = 0;


	};

	
	//api::transmitter is a one way object that recieves a container via it's data and size, and uses this to construct it within it's source location.
	template <detail::container_type T> requires requires { typename T::const_pointer; }
	struct transmitter final
	{
		using const_pointer = T::const_pointer;
		using elem = T;
		
		using data_type = std::add_pointer_t<std::ranges::range_value_t<const std::string>>;
		//using data_type = decltype(((elem*)(0))->data());


	public:

		//destruction is trival stil, so this will due.
		//~transmitter(){}

		transmitter(elem& e) : _ref{ &e } {}

		
		transmitter& operator= (const elem& e)
		{
			return move(e.data(), e.size());
		}

		transmitter& operator= (elem&& e)
		{
			return move(e.data(), e.size());
		}


		transmitter& operator= (const container<elem>& e)
		{
			return move(e.data(), e.size());
		}

		transmitter& operator= (container<elem>&& e)
		{
			return move(e.data(), e.size());
		}

	private:
		//This will always copy.
		virtual transmitter& move(data_type data, size_t size) final
		{
			if (_ref)
			{
				*_ref = elem{ data, size };
			}

			return *this;
		}


	private:

		elem* _ref = nullptr;
	};

	namespace trans
	{
		template <class Elem, class Traits = std::char_traits<Elem>, class Alloc = std::allocator<Elem>>
		using basic_string = transmitter<std::basic_string<Elem, Traits, Alloc>>;



		using string = transmitter<std::string>;

	}


	template <class Elem, class Traits = std::char_traits<Elem>, class Alloc = std::allocator<Elem>>
	using basic_string = container<std::basic_string<Elem, Traits, Alloc>>;



	using string = container<std::string>;

	//todo: I think I should probably use std::span more here.
	template <class T, class Alloc = std::allocator<T>>
	using vector = container<std::vector<T, Alloc>>;
	//using 

	/*
	template<class T, class Alloc = std::allocator<T>>
	class vector
	{

		using elem = T;
		using pointer = T*;


		pointer _data = nullptr;
		std::uint64_t _size{};

	public:

		T& operator[] (std::uint64_t index) const
		{
			assert(index < _size);

			return _data[index];
		}

		uint64_t size() const
		{
			return _size;
		}
		
		operator std::vector<T>() { return _data && _size ? std::vector<T>(_data, _data + _size) : std::vector<T>{}; }

		vector(std::vector<T>& v) : _data{ v.data() }, _size{ v.size() } {}
		{

		}
		vector(std::vector<T>&& v) : vector{v} {}
		{

		}
	};

	template<class T, class Alloc = std::allocator<T>>
	class vector
	{
		using Vector = std::vector<T, Alloc>;
		
		mirror& _ref;

	public:

		T& operator[] (std::uint64_t index) const
		{
			assert(index < _size);

			return _data[index];
		}

		uint64_t size() const
		{
			return _size;
		}

		operator std::vector<T>() { return _data && _size ? std::vector<T>(_data, _data + _size) : std::vector<T>{}; }

		vector(Vector& v) : _ref{v} {}
		{

		}
		vector(Vector&& v) : vector{ v } {}
		{

		}
	};

	//I won't be using this for right now.
	template <class Elem, class Traits>
	class basic_string_view
	{

	};
	
	template <class Elem, class Traits = std::char_traits<Elem>>
	class basic_string_view;

	using string_view = basic_string_view<char>;
	//*/
}