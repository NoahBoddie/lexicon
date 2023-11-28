#pragma once

#include "ISpecializer.h"
#include "ITypePolicy.h"
#include "IFunction.h"
#include "AbstractTypePolicy.h"
#include "AbstractFunction.h"
#include "IGenericParameter.h"
#include "IGenericArgument.h"
#include "SpecialServer.h"
namespace LEX
{
	struct ISpecializable;

	//Partial is not considered an IGenericArgument, as it can contain undefined types within.
	struct SpecClient final : public ISpecializer, public ITypePolicy, public IFunction
	{
		SpecClient(ISpecializable* tar, IGenericParameter& args) : ISpecializer{ tar }, _types{ args.GetParams() } {}


		std::vector<ITypePolicy*> _types;

		SpecServer* ObtainSpecialized(IGenericArgument& args)
		{
			GenericArgumentArray gen_arr = MakeGenericArray(args);

			return GetTarget()->ObtainSpecialized(gen_arr);
		}

		virtual AbstractTypePolicy* GetTypePolicy(IGenericArgument* args) override
		{
			//if (_target->GetSpecialType() != SpecialType::Type) {
			//	return nullptr;
			//}
			if (!args)
				return nullptr;

			return dynamic_cast<AbstractTypePolicy*>(ObtainSpecialized(*args));
		}

		AbstractFunction* GetFunction(IGenericArgument* args) override
		{
			//if (_target->GetSpecialType() != SpecialType::Function) {
			//	return nullptr;
			//}
			if (!args)
				return nullptr;

			return dynamic_cast<AbstractFunction*>(ObtainSpecialized(*args));
		}

		GenericArgumentArray MakeGenericArray(IGenericArgument& args)
		{
			//If no args, issue probably.

			auto size = _types.size();

			GenericArgumentArray gen_array{ size };

			for (int i = 0; i < size; i++)
			{
				//I just realized this isn't needed, because if it's generic then it's just going to get what's at the index.
				//auto& type = _types[i];
				//gen_array[i] = type->IsAbstract() ? type->GetTypePolicy(args) : args->at(type->GetGenericIndex());
				gen_array[i] = _types[i]->GetTypePolicy(&args);
			}

			return gen_array;
		}

	};

}