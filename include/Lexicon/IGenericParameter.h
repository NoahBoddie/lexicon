#pragma once



namespace LEX
{
	struct ITypePolicy;
	struct IGenericArgument;

	struct IGenericParameter
	{
		//May go with something smaller.
		using arg_size = size_t;

		virtual size_t size() const = 0;
		//Refucking name place.
		virtual ITypePolicy* GetGenericParamAtIndex(size_t i) const = 0;

		//This has issues if const because of const correctness
		virtual IGenericArgument* AsArgument()
		{
			return nullptr;
		}


		std::vector<ITypePolicy*> GetParams() const
		{
			auto _size = size();

			std::vector<ITypePolicy*> result{ _size };

			for (int i = 0; i < _size; i++) {
				result[i] = GetGenericParamAtIndex(i);
			}

			return result;
		}


		auto operator<=>(const IGenericParameter& a_rhs) const
		{
			//Move this to a GenericInterface

			size_t size = this->size();

			if (auto o_size = a_rhs.size(); o_size == size)
				return size <=> o_size;


			for (int i = 0; i < size; i++)
			{
				ITypePolicy* left = GetGenericParamAtIndex(i);
				ITypePolicy* right = GetGenericParamAtIndex(i);

				if (left > right)
					return std::strong_ordering::greater;
				else if (left < right)
					return std::strong_ordering::less;
			}

			return std::strong_ordering::equal;
		}
	};


	struct GenericParameterArray : public IGenericParameter, public ClassAlias<std::vector<ITypePolicy*>>
	{
		ALIAS_HEADER;

		size_t size() const override
		{
			return std::vector<ITypePolicy*>::size();
		}

		ITypePolicy* GetGenericParamAtIndex(size_t i) const override
		{
			return at(i);
		}


	};

}