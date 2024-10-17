#pragma once

namespace LEX
{
	struct IFormula;
	struct FormulaManager;

	struct FormulaHandler
	{
		friend FormulaManager;
		
		~FormulaHandler() { Unhandle(); }


		constexpr FormulaHandler() = default;

		FormulaHandler(IFormula* formula)
		{
			_formula = formula;
		}

		FormulaHandler(const FormulaHandler& other)
		{
			Transfer(other, true);
		}


		FormulaHandler(FormulaHandler&& other)
		{
			Transfer(other, false);
		}


		FormulaHandler& operator=(const FormulaHandler& other)
		{
			
			Transfer(other, true);
			return *this;
		}

		FormulaHandler& operator=(FormulaHandler&& other)
		{
			if (_formula != other._formula)
				Unhandle();

			Transfer(other, false);
			return *this;
		}

		constexpr operator bool() const noexcept
		{
			return _formula;
		}

		constexpr IFormula* formula()
		{
			return _formula;
		}

	private:
		void Transfer(const FormulaHandler& other, bool copy);

		void Unhandle();

	private:

		mutable IFormula* _formula = nullptr;
	};
	REQUIRED_SIZE(FormulaHandler, 0x8);
}