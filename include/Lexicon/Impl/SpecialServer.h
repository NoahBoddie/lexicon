#pragma once

#include "ISpecializer.h"
#include "IGenericArgument.h"

namespace LEX
{
	struct AbstractTypePolicy;

	struct SpecServer : public ISpecializer
	{
		std::vector<AbstractTypePolicy*> _types;

		SpecServer(ISpecializable* tar, IGenericArgument& args) : ISpecializer{ tar }, _types{ args.GetArgs() } {}
	};
}