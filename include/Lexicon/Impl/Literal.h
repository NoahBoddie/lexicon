#pragma once


namespace LEX
{
	class Variable;

	class Literal final
	{
	public:
		constexpr Literal() = default;
		constexpr Literal(size_t i) : _id{ i } {}


		Variable* operator->();

		const Variable* operator->() const;

		Variable& operator*();
		const Variable& operator*() const;


		operator bool() const
		{
			//rename this value to something specific plz
			return _id == max_value<size_t>;
		}
	private:
		size_t _id{ max_value<size_t> };

	};
}