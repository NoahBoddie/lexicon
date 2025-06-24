#include "Lexicon/Engine/LiteralManager.h"


namespace LEX
{
	//*
	std::vector<std::pair<Variable, TypeInfo*>> literalList{};


	//TODO: Ideally obtain literal, this returns const, will want to experiment with that later.
	Literal LiteralManager::ObtainLiteral(SyntaxRecord& ast)
	{
		//I have a concept within this of making hashes from the strings of records, and mutating the hash with it's children
		Variable entry = CreateVariable(ast);
		TypeInfo* info = entry.GetTypeInfo();

		assert(info);


		auto pair = std::make_pair(entry, info);

		auto begin = literalList.begin();
		auto end = literalList.end();

		auto it = std::find_if(begin, end, [&](auto& arg) { return arg == pair; });

		size_t result = max_value<size_t>;

		if (it != end) {
			result = std::distance(begin, it);
		}
		else {
			result = literalList.size();

			literalList.emplace_back(std::move(entry), info);
		}
	
		return Literal{ result };
	}

	Variable* LiteralManager::GetLiteral(size_t index)
	{
		auto var = std::addressof(literalList[index].first);

		return var;
	}

	//*/
}