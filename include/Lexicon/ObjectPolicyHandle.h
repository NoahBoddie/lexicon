#pragma once



namespace LEX
{
	struct ObjectPolicy;


	struct ObjectPolicyHandle
	{
		static constexpr uint32_t invalid_index = -1;

	public:
		ObjectPolicyHandle() = default;

		ObjectPolicyHandle(uint32_t i) : _index{ i }
		{

		}



		ObjectPolicyHandle& operator=(uint32_t i)
		{
			_index = i;
			return *this;
		}


		ObjectPolicy* get() const;


		operator ObjectPolicy* () const
		{
			return get();

		}


		ObjectPolicy* operator-> ()const
		{
			return get();
		}

		operator bool() const
		{
			return _index != invalid_index;
		}

		//Expand to 3 way operator.
		//bool operator ==(ObjectPolicyHandle other)
		//{
		//	return _index == other._index;
		//}

		//constexpr std::strong_ordering operator <=>(ObjectPolicyHandle other) const noexcept
		//{
		//	return _index <=> other._index;
		//}

		constexpr std::strong_ordering operator <=>(const ObjectPolicyHandle& other) const noexcept = default;
		//Sort out these ops real quick.

		uint32_t index() const
		{

			return _index;
		}

	private:

		uint32_t _index = -1;

	};
	REQUIRED_SIZE(ObjectPolicyHandle, 0x4);


	//Get ObjectPolicyHandle Here.
}