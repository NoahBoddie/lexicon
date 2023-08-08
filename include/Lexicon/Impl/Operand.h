#pragma once

#include "Lexicon/Impl/Register.h"
#include "Lexicon/Impl/OperandType.h"

namespace LEX::Impl
{
	struct Operand
	{
		//make some constructors for registers and such. PROBABLY best to make them a static function. More descriptive.
		Operand() = default;

		Operand(OperandType type, size_t value)
		{
			SetType(type);
			Set(value);
		}

		Operand(Register i) : Operand{ OperandType::Register, (uint32_t)i }
		{}

		//This is the only one that stays explicitly like this.
		template <class T>
		static Operand CreateAs(T i)
		{
			Operand t{};
			t.Force(i);
			return t;
		}



		OperandType GetType()
		{


			auto t = m_value >> m_lastByte;

			return static_cast<OperandType>(t);
		}

		void SetType(OperandType type)
		{
			m_value &= ~(m_shift << m_lastByte);

			auto t = static_cast<decltype(m_value)>(type) << m_lastByte;

			m_value |= t;
		}

		operator size_t()
		{
			return m_value;
		}

		//The difference between Get and As is that get removes the last byte, As is a reinterpret cast of it's value. 
		// Get is when it cares about the target type, As is when it ignores it.

		template <class T = size_t>
		T Get()
		{

			auto out = m_value & ~(m_shift << m_lastByte);

			return static_cast<T>(out);
		}


		template <class T = size_t>
		void Set(T v)
		{

			auto in = static_cast<size_t>(v) & ~(m_shift << m_lastByte);

			m_value = in | ((size_t)GetType() << m_lastByte);
		}

		template <class T = size_t>
		void Force(T v)
		{
			static_assert(std::is_pointer_v<T>);
			m_value = reinterpret_cast<size_t>(v);
		}


		template <class T>
		T As()
		{
			return static_cast<T>(m_value);
		}

		size_t m_value = max_value<size_t>;
	private:
		//The point of the last byte in bit form
		constexpr static size_t m_shift = 0xFF;
		constexpr static auto m_lastByte = (sizeof(decltype(m_value)) * 8) - 8;
	};
}