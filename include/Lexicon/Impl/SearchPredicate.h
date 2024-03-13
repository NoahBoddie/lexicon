#pragma once

namespace LEX
{
	struct Field;
	struct ITypePolicy;
	struct OverloadKey;
	enum struct SearchResult;

	//Will expand later.
	template <typename R>
	using SearchPredicate = std::function<SearchResult(std::string&, OverloadKey&, R)>;
	//old = std::function<SearchResult(SearchParams&, Field*)>;

	using FieldPredicate = SearchPredicate<Field*>;

	using TypePredicate = SearchPredicate<ITypePolicy*>;

}