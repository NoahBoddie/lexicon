#include "Lexicon/FormulaHandler.h"


#include "Lexicon/Interfaces/FormulaManager.h"
#include "Lexicon/IFormula.h"
namespace LEX
{

	void FormulaHandler::Transfer(const FormulaHandler& other, bool copy)
	{
		if (auto formula = other._formula)
		{
			if (copy)
				//FormulaManager::instance->IncrementForumula(formula);
				formula->ModRefCount(true, {});

			else
				other._formula = nullptr;

			_formula = formula;

		}
	}

	void FormulaHandler::Unhandle()
	{
		if (_formula) {
			//FormulaManager::instance->DecrementForumula(_formula);
			_formula->ModRefCount(false, {});
		}
	}
}