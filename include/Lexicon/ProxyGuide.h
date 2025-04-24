#pragma once

namespace LEX
{
	//The idea of a proxy guide is it allows for types like VariableType, ObjectTranslator, and Unvariable to be merged into one
	template <typename T>
	struct ProxyGuide : public detail::not_implemented {};

}