#pragma once

#include "Lexicon/Qualifier.h"
#include "Lexicon/RuntimeVariable.h"

#include "Lexicon/Engine/Runtime.h"

namespace LEX
{
	struct RunUtil
	{
		static int CantPromote(const RuntimeVariable& var, Runtime* runtime, Reference type)
		{
			if (type >= Reference::Local) {

				if (var.IsReference() == false)
					return 1;

				if (type >= Reference::Scoped) {

					if (runtime->ContainsVariable(var) == true)
						return 2;

					if (type >= Reference::Global) {
						if (runtime->GetCaller()->OwnsVariable(var) == true)
							return 3;
					}
				}
			}

			return 0;
		}
	};

	using RuntimeUtility = RunUtil;
}