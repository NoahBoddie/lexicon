#include "Lexicon/Interfaces/FormulaManager.h"

#include "Lexicon/IFormula.h"
#include "Lexicon/SignatureBase.h"
#include "Lexicon/FormulaHandler.h"


#include "Lexicon/Interfaces/ProjectManager.h"

#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"
#include "Lexicon/Engine/ParserTest.h"
#include "Lexicon/Engine/BasicFormula.h"
#include "Lexicon/Engine/RoutineCompiler.h"

namespace LEX
{


	[[nodiscard]] uint64_t FormulaManager::RequestFormulaFromRecord(const ISignature& base, std::span<std::string_view> params, std::string_view name, SyntaxRecord& ast,
		FormulaHandler& out, std::optional<IScript*> from, const std::source_location& loc)
	{
		//TODO FormulaManager needs to return the APIResult not a random ass integer.
		
		std::unique_ptr<BasicFormula> formula = std::make_unique<BasicFormula>();

		SyntaxRecord& records = formula->records = std::move(ast);

		formula->_returnType = base.result();

		auto parameters = base.parameters();

		if (auto target = base.target(); target) {
			formula->_thisInfo = std::make_unique<ParameterInfo>(target, parse_strings::this_word, 0);
		}

		if (params.empty() == false)
		{
			auto& f_params = formula->parameters;
			for (int i = 0; i < parameters.size(); i++)
			{
				f_params.push_back(ParameterInfo(parameters[i], std::string(params[i]), formula->GetParamCount()));
			}
		}


		Script* perspective;

		if (from.has_value() == false) {
			perspective = ProjectManager::instance->GetShared()->GetCommons();
		}
		else {
			//perspective = static_cast<Script*>(ProjectManager::instance->GetScriptFromPath(from));
			perspective = static_cast<Script*>(from.value());
		}


		if (!perspective) {
			report::apply::failure("Launch Script is null.");
			return 2;
		}


		//This needs to confirm it's proper
		if (RoutineCompiler::Compile(formula->_routine, records, formula.get(), perspective, nullptr, name) == false) {
			return 3;
		}
		formula->SetName(name);
		formula->SetFile(loc.file_name());
		out._formula = formula.release();

		//Zero means success
		return 0;
	}

	[[nodiscard]] uint64_t FormulaManager::RequestFormula(const ISignature& base, std::span<std::string_view> params, std::string_view routine, 
		FormulaHandler& out, std::optional<IScript*> from, const std::source_location& loc)
	{


		std::string name;

		constexpr auto limit = 25;
		if (routine.size() <= 25) {
			name = std::format("<: {} :>", routine);
		}
		else {
			name = std::format("<: {}... :>", routine.substr(0, 25 - 3));
		}

		SyntaxRecord ast;

		if (Parser::CreateSyntax<LineParser>(ast, routine, loc.line()) == false)
		{
			return 1;
		}

		return RequestFormulaFromRecord(base, params, name, ast, out, from, loc);
	}
}