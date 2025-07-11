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

	struct FormulaData
	{
		//Helps prevent redoing a formula that's effectively already loaded. Not going to worry about that right now though.
		size_t refCount = 0;
	};


	std::unordered_map<IFormula*, FormulaData> formulaMap;



	FormulaData* FindFormData(IFormula* formula)
	{
		if (formula) {
			auto end = formulaMap.end();
			auto it = formulaMap.find(formula);

			if (formulaMap.end() != it)
				return &it->second;
		}
		return nullptr;
	}



	[[nodiscard]] uint64_t FormulaManager::RequestFormula(const ISignature& base, std::span<std::string_view> params, std::string_view routine, 
		FormulaHandler& out, std::optional<IScript*> from, const std::source_location& loc)
	{
		//TODO FormulaManager needs to return the APIResult not a random ass integer.
		std::optional<IScript*> test;

		std::unique_ptr<BasicFormula> formula = std::make_unique<BasicFormula>();

		formula->_returnType = base.result();

		auto parameters = base.parameters();

		if (auto target = base.target(); target){
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

		SyntaxRecord& ast = formula->records;
		
		if (Parser__::CreateSyntax<LineParser>(ast, routine, loc.line()) == false)
		{
			
			return 1;
		}


		Script* perspective;

		if (from.has_value() == false){
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

		std::string name;

		if (false) {
			//Some way to stringize a Signature locally would be really cool for visuals.
			name += base.result()->GetName();
			name += " (";

			name += ")";

		}
		else
		{
			constexpr auto limit = 25;
			if (routine.size() <= 25) {
				name = std::format("<: {} :>", routine);
			}
			else {
				name = std::format("<: {}... :>", routine.substr(0, 25 - 3));

			}
		}
		
		//This needs to confirm it's proper
		if (RoutineCompiler::Compile(formula->_routine, ast, formula.get(), perspective, nullptr, name) == false) {
			return 3;
		}
		formula->SetName(name);
		formula->SetFile(loc.file_name());

		formulaMap[formula.get()].refCount = 1;
		out._formula = formula.release();
		
		//Zero means success
		return 0;
	}

	void FormulaManager::IncrementForumula(IFormula* formula)
	{
		auto data = FindFormData(formula);
		//Needs some thread safety do you not agree?
		if (data) {
			data->refCount++;
		}
	}

	void FormulaManager::DecrementForumula(IFormula*& formula)
	{
		auto data = FindFormData(formula);
		//Needs some thread safety do you not agree?
		if (data && --data->refCount == 0) {
			delete formula;
			formulaMap.erase(formula);

			formula = nullptr;
		}
	}

}