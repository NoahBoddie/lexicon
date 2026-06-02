#pragma once

#include "OverloadFlag.h"

#include "Lexicon/Engine/Overload.h"

namespace LEX
{
	struct Overload;
	struct OverloadParameter;
	struct ITypeInfo;
	struct QualifiedType;

	struct OverloadArgument
	{//The argument side of the overload.

		virtual OverloadBias Match(OverloadParameter*, ITypeInfo* scope, Overload& out, Overload* prev) = 0;



		/*
		static size_t CheckOverload2(OverloadArgument& input, std::vector<OverloadInfo*> clauses, Overload& ret)
		{
			Overload* last = nullptr;

			size_t result = -1;

			for (auto i = 0; i < clauses.size(); i++)
			{
				auto clause = clauses[i];

				Overload buffer;

				auto bias = input.Match(clauses[i], nullptr, buffer, last);

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
		//*/
	};

}