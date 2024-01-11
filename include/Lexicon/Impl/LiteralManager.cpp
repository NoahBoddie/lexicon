#include "LiteralManager.h"


namespace LEX
{
	//*

	//TODO: Ideally obtain literal, this returns const, will want to experiment with that later.
	Literal  LiteralManager::ObtainLiteral(Record& ast)
	{
		//I have a concept within this of making hashes from the strings of records, and mutating the hash with it's children
		Variable entry = CreateVariable(ast);

		auto begin = _LiteralList().begin();
		auto end = _LiteralList().end();

		auto it = std::find_if(begin, end, [&](auto& arg) { return arg == entry; });

		size_t result = max_value<size_t>;

		if (it != end) {
			result = std::distance(begin, it);
		}
		else {
			result = _LiteralList().size();
			_LiteralList().emplace_back(entry);
			size = result;
		}
		RGL_LOG(trace, "<%> hit {} , addr {}", _LiteralList().size(), (uintptr_t)&_LiteralList());
		//GetLiteral(result);
		return Literal{ result };
	}

	Variable* LiteralManager::GetLiteral(size_t index)
	{
		RGL_LOG(trace, "<%> get at {}, size {}, addr {}", index, _LiteralList().size(), (uintptr_t)&_LiteralList());

		return &_LiteralList()[index];
	}


	void LiteralManager::FinalizeLiteral()
	{
		//Because this goes off, things will need to use a literal key
		_LiteralList().shrink_to_fit();
	}

	std::vector<Variable>& LiteralManager::_LiteralList()
	{
		static std::vector<Variable> _list{};
		return _list;
	}
	//*/
}