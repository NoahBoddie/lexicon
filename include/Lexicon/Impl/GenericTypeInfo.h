#pragma once

#include "TypeInfo.h"
#include "ISpecializable.h"
#include "SpecialTypePolicy.h"

namespace LEX
{
	struct GenericTypeInfo : public TypeInfo, public ISpecializable
	{

		SpecServer* MakeServer(IGenericArgument& args) override
		{
			//Not actually proper, this is just what it sends.
			return new SpecialTypePolicy();
		}

	};