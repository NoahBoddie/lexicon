#pragma once

#include "IGenericParameter.h"

namespace LEX
{
	struct AbstractTypePolicy;

	struct IGenericArgument : public IGenericParameter
	{
		virtual AbstractTypePolicy* GetArgAtIndex(size_t i) const = 0;


		std::vector<AbstractTypePolicy*> GetArgs() const
		{
			auto _size = size();

			std::vector<AbstractTypePolicy*> result{ _size };

			for (int i = 0; i < _size; i++) {
				result[i] = GetArgAtIndex(i);
			}

			return result;
		}

		IGenericArgument* AsArgument() override
		{
			return this;
		}
	};


	struct GenericArgumentArray : public  IGenericArgument, public ClassAlias<std::vector<AbstractTypePolicy*>>
	{
		//Class alias should probably be protected
		ALIAS_HEADER;

		size_t size() const override
		{
			return std::vector<AbstractTypePolicy*>::size();
		}

		ITypePolicy* GetGenericParamAtIndex(size_t i) const override
		{
			//if I do source this isn't needed but Abstract will always be just off of ITypePolicy,
			// and this doesn't really need a source.
			return reinterpret_cast<ITypePolicy*>(GetArgAtIndex(i));
		}
		AbstractTypePolicy* GetArgAtIndex(size_t i) const override
		{
			return const_cast<AbstractTypePolicy*>(at(i));
		}
	};


}