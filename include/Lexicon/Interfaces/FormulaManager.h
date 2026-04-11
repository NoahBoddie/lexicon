#pragma once

#include "Interface.h"

#ifdef LEX_SOURCE
#include "Lexicon/Engine/SyntaxRecord.h"
#endif

namespace LEX
{
	struct IScript;
	struct IFormula;
	struct ISignature;
	struct SignatureBase;
	struct FormulaHandler;
	struct FormulaData;


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(FormulaManager)
			{
				[[nodiscard]] virtual uint64_t RequestFormula(const LEX::ISignature& base, std::span<std::string_view> params, 
					std::string_view routine, FormulaHandler& out, std::optional<LEX::IScript*> from = std::nullopt,
					const std::source_location& loc = std::source_location::current()) = 0;
			};
		}

		CURRENT_VERSION(FormulaManager, 1);
	}


	struct IMPL_SINGLETON(FormulaManager)
	{
#ifdef LEX_SOURCE
		[[nodiscard]] uint64_t RequestFormulaFromRecord(const ISignature& base, std::span<std::string_view> params,
			std::string_view name, SyntaxRecord& routine, FormulaHandler& out, std::optional<IScript*> from = std::nullopt,
			const std::source_location& loc = std::source_location::current()) INTERFACE_FUNCTION;
#endif

		[[nodiscard]] uint64_t RequestFormula(const ISignature& base, std::span<std::string_view> params, 
			std::string_view routine, FormulaHandler& out, std::optional<IScript*> from = std::nullopt, 
			const std::source_location& loc = std::source_location::current()) override;
	};
}