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

		Variable* operator*();
		const Variable* operator*() const;

	private:
		size_t _id{};//Should this have some specific value?

	};
}