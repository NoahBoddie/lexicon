#pragma once

namespace LEX
{
	struct OverloadClause;
	struct ISpecializable;

	struct Interface
	{
		//TODO: Rename Interface "ISpecialType". Other things claim the name interface.

		//This is a function that helps determine if the given Type/Function is merely an abstraction of an implementation. Which I should have named slightly better.
		virtual bool IsAbstract() const = 0;

		//Rename this shit please.

		//This is part of the specializing interface or fucking whatever.
		virtual ISpecializable* GetSpecializable() { return nullptr; }

		virtual OverloadClause* GetClause() = 0;
	};
}