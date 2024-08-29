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



	[[nodiscard]] uint64_t FormulaManager::RequestFormula(SignatureBase& base, std::vector<std::string_view>& params, std::string_view routine, FormulaHandler& out)
	{

		std::unique_ptr<BasicFormula> formula = std::make_unique<BasicFormula>();

		formula->_returnType = base.result;

		for (int i = 0; i < base.parameters.size(); i++)
		{
			formula->parameters.push_back(ParameterInfo(base.parameters[i], std::string(params[i]), i));
		}


		Record ast = Impl::Parser__::CreateSyntax<Impl::LineParser>(std::string(routine));



		//This needs to confirm it's proper
		formula->_routine = RoutineCompiler::Compile(ast, formula.get(), ProjectManager::instance->GetShared()->GetCommons());


		formulaMap[formula.get()].refCount = 1;
		out._formula = formula.get();
		formula.release();

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
			logger::info("deleteddd");
		}
	}

}