#include "Lexicon/Interfaces/LinkMessenger.h"

#include "Lexicon/Engine/Component.h"

namespace LEX
{
	static std::vector<LinkCallback*> callbackList;


	bool LinkMessenger::RegisterForLink(LinkCallback* callback, bool catch_up)
	{
		if (!callback) {
			return false;
		}

		callbackList.push_back(callback);


		if (catch_up)
		{
			if (auto linked = Component::FlagsLinked())
			{
				bit_loop(linked) {
					for (auto callback : callbackList)
						callback(i);
				}
			}
		}
		//if catch up is true it will try to play catch up with existing links.

		return true;
	}

	void LinkMessenger::Dispatch(LinkFlag a_flags)
	{
		bit_loop(a_flags) {
			for (auto callback : callbackList)
				callback(i);
		}
	}

}