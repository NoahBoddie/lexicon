#pragma once

#include "Lexicon/Qualifier.h"
#include "Lexicon/RuntimeVariable.h"

#include "Lexicon/Engine/Runtime.h"

namespace LEX
{
	struct RunUtil
	{
		static int CantPromote(const RuntimeVariable& var, Runtime* runtime, Refness type)
		{
			if (type >= Refness::Local) {

				if (var.IsReference() == false)
					return 1;

				if (type >= Refness::Scoped) {

					if (runtime->ContainsVariable(var) == true)
						return 2;

					if (type >= Refness::Global) {
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