#pragma once

#include "Literal.h"
#include "Variable.h"
#include "LiteralManager.h"

namespace LEX
{

	Variable* Literal::operator->()
	{
		return LiteralManager::GetLiteral(_id);
	}

	const Variable* Literal::operator->() const
	{
		return LiteralManager::GetLiteral(_id);
	}

	Variable* Literal::operator*()
	{
		return LiteralManager::GetLiteral(_id);
	}

	const Variable* Literal::operator*() const
	{
		return LiteralManager::GetLiteral(_id);
	}
}