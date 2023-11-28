#pragma once


#include "IGenericParameter.h"
#include "IGenericArgument.h"
#include "GenericParamPolicy.h"


//Temp? need source??
#include "SpecialClient.h"
#include "SpecialServer.h"
namespace LEX
{
	//The notes
		//The abstract classes are the bridges between the specializers and how they're used (the interface classes). To get them to this point, I could have a fuction do it, but
		// a static cast will be fine.

		//But this abstraction as we have it mainly only works for the complete specialization of something, and not for the incomplete.


		//struct AbstractTest

	struct SpecClient;
	struct Spec;


	struct ISpecializable : public IGenericParameter//Should be set up by it's parameters. obvs.
	{
		virtual ~ISpecializable()
		{
			for (auto& spec : _servers)
			{
				delete spec;
			}

			for (auto& cli : _clients)
			{
				delete cli;
			}
		}

	protected:


		SpecClient* FindClient(ISpecializable* target, IGenericParameter& args)
		{
			for (auto& client : _clients)
			{
				//First compare target, then compare args.
			}

			return nullptr;
		}

		SpecServer* FindServer(IGenericArgument& args)
		{
			for (auto& server : _servers)
			{
				//First compare target, then compare args.
			}

			return nullptr;
		}

		SpecClient* CreateClient(ISpecializable* target, IGenericParameter& args)
		{
			SpecClient* result = new SpecClient(target, args);

			_clients.push_back(result);

			return result;
		}

		//The actual server object depends. So this needs to be used. This is the only thing I need virtual.
		virtual SpecServer* MakeServer(IGenericArgument& args) = 0;

		SpecServer* CreateServer(IGenericArgument& args)
		{
			SpecServer* result = MakeServer(args);


			_servers.push_back(result);

			//Every time a specializable is specialized, the things that it targets with partials needs to specialize whatever it targets.
			//Make sure to add first, otherwise it's going to try to keep adding the same new server.
			SpecializeClients(args);

			return result;
		}

		void SpecializeClients(IGenericArgument& args)
		{
			for (auto& client : _clients) {
				//It will have already specialized itself if this is the case, but it needs something else off the client. But I this might need to happen.
				// so I need to figure out how I can prevent the mere reference of the self 1 to 2
				//if (client->GetTarget() != this)
				client->ObtainSpecialized(args);
			}
		}
		virtual ISpecializable* GetParentSpecializable() = 0;
	public:

		//I'll have to dynamic cast anyways, so ditching this probably.
		//virtual SpecialType GetSpecialType() = 0;

		//Switch to these names.
		SpecServer* ObtainSpecialized(IGenericArgument& args)
		{
			auto result = FindServer(args);

			if (!result) {
				result = CreateServer(args);
			}

			return result;
		}

		SpecServer* ObtainSpecialized(IGenericArgument* args)
		{
			return ObtainSpecialized(*args);
		}


		SpecClient* ObtainPartialized(ISpecializable* tar, IGenericParameter& args)
		{
			SpecClient* result = FindClient(tar, args);

			if (!result) {
				result = CreateClient(tar, args);
			}

			return result;
		}

		SpecClient* ObtainPartialized(ISpecializable* tar, IGenericParameter* args)
		{
			return ObtainPartialized(tar, *args);
		}




		ISpecializer* ObtainSpecializer(ISpecializable* tar, IGenericParameter& params)
		{
			IGenericArgument* args = params.AsArgument();

			return !args ? (ISpecializer*)ObtainPartialized(tar, params) : (ISpecializer*)tar->ObtainSpecialized(*args);

		}


		ISpecializer* ObtainSpecializer(ISpecializable* tar, GenericParameterArray params)
		{
			GenericArgumentArray args{ params.size() };

			bool is_gen = false;

			std::transform(params.begin(), params.end(), params.begin(),
				[&](auto it) -> AbstractTypePolicy*
				{
					if (it->IsGenericArg() == true) {
						is_gen = true;
					}

					if (is_gen)
						return (nullptr);



					return it->GetTypePolicy(nullptr);
				});

			IGenericParameter* send;

			if (is_gen) {
				send = &params;
			}
			else {
				send = &args;
			}

			auto result = ObtainSpecializer(tar, *send);
			return result;
		}

		//The above is actually confusing. Is this supposed to be used on the specialization that houses the query? OR is it supposed to be the target of the query.



		size_t size() const override
		{
			return _params.size();
		}

		ITypePolicy* GetGenericParamAtIndex(size_t i) const override
		{
			auto ret = &_params.at(i);
			return const_cast<GenericParamPolicy*>(ret);
		}


		//Change the above to only use reference.

		GenericArgumentArray GetGenericParamsAsArgument()
		{
			//This has to be made so it will take the generic parameters and convert them into arguments.
			// to do this, we get what they extends (which they should ALWAYS extend something) and we submit that. What it extends is always an abstract type, one 
			// willing and able to answer questions.
			
			auto count = _params.size();
			
			
			GenericArgumentArray args{ count };

			for (int i = 0; i < count; i++) {
				args[i] = _params[i].extends();
			}

		}

	private:
		//This should have a get parent function probably.
		size_t _expectedArgs = 0;//Normally, Params would expect these. However, when it is that one is using a generic set up within generic class

		std::vector<GenericParamPolicy> _params;
		std::vector<SpecClient*> _clients;//I can store these directly if I use list. Hmm
		std::vector<SpecServer*> _servers;
	};
}