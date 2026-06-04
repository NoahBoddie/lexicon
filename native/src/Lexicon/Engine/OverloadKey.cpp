#pragma once

#include "Lexicon/Engine/QualifiedType.h"
#include "Lexicon/Engine/OverloadKey.h"
#include "Lexicon/Engine/OverloadInfo.h"

namespace LEX
{
	size_t OverloadArgument::CheckOverload(std::span<OverloadInfo*> clauses, Overload& ret)
	{
		Overload* last = nullptr;

		size_t result = -1;

		for (auto i = 0; i < clauses.size(); i++)
		{
			auto clause = clauses[i];

			Overload buffer;

			auto bias = Match(clauses[i], nullptr, buffer, last);

			switch (bias)
			{
			case OverloadBias::kAmbiguous:
				result = -1;
				break;

			case OverloadBias::kCurrent:
				ret = std::move(buffer);
				last = &ret;
				result = i;
				break;
			}
		}
		return result;
	}

	//QualifiedType OverloadArgument::GetTarget() const
	//{
	//	return {};
	//}
}