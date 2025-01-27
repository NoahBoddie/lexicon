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
				virtual bool RegisterForLink(LinkCallback* callback, bool catch_up) = 0;
			};
		}

		CURRENT_VERSION(LinkMessenger, 1);
	}


	struct IMPL_SINGLETON(LinkMessenger)
	{
		bool RegisterForLink(LinkCallback* callback, bool catch_up = true) override;


	INTERNAL:
		void Dispatch(LinkFlag flag);

	};
}