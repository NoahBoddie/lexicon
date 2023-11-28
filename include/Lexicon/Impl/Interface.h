#pragma once

namespace LEX
{
	struct ISpecializable;

	struct Interface
	{
		//This is a function that helps determine if the given Type/Function is merely an abstraction of an implementation. Which I should have named slightly better.
		virtual bool IsAbstract() const = 0;

		//Rename this shit please.

		//This is part of the specializing interface or fucking whatever.
		virtual ISpecializable* GetSpecializable() = 0;
	};
}