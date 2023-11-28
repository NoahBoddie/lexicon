#pragma once

#include "Interface.h"
#include "IGenericArgument.h"

namespace LEX
{
	struct AbstractTypePolicy;


	struct ITypePolicy : public Interface
	{
		constexpr static uint32_t NonGenericIndex = full_value<uint32_t>;

		//ITypePolicy(ISpecializable* tar) : _target{ tar } {}

		//An interface of a type policy, which may or may not be a policy. Generally, one should use all the functions that this provides in order
		// to use the type policy.


		//Unsure of how this gets one however.


		//Might require an ISpecializable target. Actually, move this shit.
		//ISpecializable* _target = nullptr;

		//This function basically expects a number of things to be submitted, but here, the full specialization of a calling function could be used in order to
		// get a partially specialized function.
		//I may still go with interface because I don't think I want something where I derive a specializaton from TypePolicy, it'd be a waste of space, literal repeat.
		// I KNOW, instead, I could make an implementation policy
		//TODO:Make second interface for "AbstractTypePolicy" which GetTypePolicy returns, and what stores all the type policy info.
		virtual AbstractTypePolicy* GetTypePolicy(IGenericArgument* args) = 0;

		virtual bool IsAbstract() const { return false; }


		bool IsGenericArg()
		{
			return GetGenericIndex() != NonGenericIndex;
		}


		virtual uint32_t GetGenericIndex()
		{
			//full value represents that it isn't generic.
			return full_value<uint32_t>;
		}
	};

}