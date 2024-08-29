#pragma once

#include "Interface.h"

namespace LEX
{
	struct IFormula;
	struct SignatureBase;
	struct FormulaHandler;


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(FormulaManager)
			{
				[[nodiscard]] virtual uint64_t RequestFormula(SignatureBase& base, std::vector<std::string_view>& params, std::string_view routine, FormulaHandler& out) = 0;
				virtual void IncrementForumula(LEX::IFormula* formula) = 0;
				virtual void DecrementForumula(LEX::IFormula*& formula) = 0;
			};
		}

		CURRENT_VERSION(FormulaManager, 1);
	}


	struct IMPL_SINGLETON(FormulaManager)
	{
		[[nodiscard]] uint64_t RequestFormula(SignatureBase& base, std::vector<std::string_view>&params, std::string_view routine, FormulaHandler& out) override;
		void IncrementForumula(LEX::IFormula* formula) override;
		void DecrementForumula(LEX::IFormula*& formula) override;
	};
}