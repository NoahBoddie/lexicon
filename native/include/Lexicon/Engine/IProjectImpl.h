#pragma once

#include "Lexicon/Interfaces/IProject.h"

namespace LEX
{
	struct IProjectImpl : public IProject
	{
	private:
		IScript* GetCommons() override final;

	public:
		virtual Script* GetCommons(bool = {}) = 0;
	};
}