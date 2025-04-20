#pragma once

namespace LEX
{
	struct OverloadClause;
	struct ISpecializable;

	struct ISpecial//This should be INTERNAL inherited. External stuff doesn't need to be concerned with this.
	{
		//TODO: Rename ISpecial "ISpecialType". Other things claim the name interface.

		//This is a function that helps determine if the given Type/Function is merely an abstraction of an implementation. Which I should have named slightly better.
		virtual bool IsResolved() const = 0;

		//Rename this shit please.

		//This is part of the specializing interface or fucking whatever.
		virtual ISpecializable* GetSpecializable() = 0;
		ISpecializable* GetSpecializable() const
		{
			auto a_this = const_cast<ISpecial*>(this);
			return a_this->GetSpecializable();
		}
	};
}