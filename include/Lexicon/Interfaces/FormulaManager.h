#pragma once

#include "Interface.h"

namespace LEX
{
	struct IScript;
	struct IFormula;
	struct ISignature;
	struct SignatureBase;
	struct FormulaHandler;


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(FormulaManager)
			{
				[[nodiscard]] virtual uint64_t RequestFormula(const LEX::ISignature& base, std::span<std::string_view> params, 
					std::string_view routine, FormulaHandler& out, std::optional<LEX::IScript*> from = std::nullopt) = 0;
				virtual void IncrementForumula(LEX::IFormula* formula) = 0;
				virtual void DecrementForumula(LEX::IFormula*& formula) = 0;
			};
		}

		CURRENT_VERSION(FormulaManager, 1);
	}


	struct IMPL_SINGLETON(FormulaManager)
	{
		[[nodiscard]] uint64_t RequestFormula(const ISignature& base, std::span<std::string_view> params, 
			std::string_view routine, FormulaHandler& out, std::optional<IScript*> from = std::nullopt) override;
		void IncrementForumula(LEX::IFormula* formula) override;
		void DecrementForumula(LEX::IFormula*& formula) override;
	};
}