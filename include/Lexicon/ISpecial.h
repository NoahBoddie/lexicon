#pragma once

namespace LEX
{
	struct OverloadClause;
	struct ISpecializable;

	struct ISpecial
	{
		//TODO: Rename ISpecial "ISpecialType". Other things claim the name interface.

		//This is a function that helps determine if the given Type/Function is merely an abstraction of an implementation. Which I should have named slightly better.
		virtual bool IsResolved() const = 0;

		//Rename this shit please.

		//This is part of the specializing interface or fucking whatever.
		virtual ISpecializable* GetSpecializable() { return nullptr; }
	};
}