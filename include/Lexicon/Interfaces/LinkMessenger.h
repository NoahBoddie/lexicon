#pragma once

#include "Lexicon/LinkFlag.h"


namespace LEX
{
	using LinkCallback = void(LinkFlag);


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(LinkMessenger)
			{
			private:
				virtual bool RegisterForLinkImpl(LinkCallback* callback, bool catch_up) = 0;
			};
		}

		CURRENT_VERSION(LinkMessenger, 1);
	}

	//TODO: Make a waiting system so early calls get procced for later
	struct IMPL_SINGLETON(LinkMessenger)
	{
	private:
		bool RegisterForLinkImpl(LinkCallback* callback, bool catch_up = true) override;
	
	public:

		static bool RegisterForLink(LinkCallback* callback, bool catch_up = true)
		{

#ifndef LEX_SOURCE
			static std::vector<LinkCallback*> earlyCallbacks{};


			if (!instance) {
				earlyCallbacks.push_back(callback);
				return false;
			}
			else if (earlyCallbacks.size()) {
				for (auto early : earlyCallbacks)
				{
					instance->RegisterForLinkImpl(early, true);
				}
			}
#endif

			return instance->RegisterForLinkImpl(callback, catch_up);
		}

	INTERNAL:
		void Dispatch(LinkFlag flag);

	};
}