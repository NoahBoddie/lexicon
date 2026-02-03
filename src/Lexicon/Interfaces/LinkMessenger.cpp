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
			if (Component::HasInit())
			{
				callback(LinkFlag::Init);
			}
			
			if (auto linked = Component::FlagsLinked())
			{
				for (auto flag = (LinkFlag)1; flag != LinkFlag::None; flag <<= 1)
				{
					if (linked & flag) {
						callback(flag);
					}
				}
			}
		}
		//if catch up is true it will try to play catch up with existing links.

		return true;
	}

	void LinkMessenger::Dispatch(LinkFlag a_flags)
	{
		if (!a_flags && Component::HasInit()) {
			for (auto callback : callbackList)
				callback(LinkFlag::Init);
		}
		for (auto flag = (LinkFlag)1; flag != LinkFlag::None; flag <<= 1)
		{
			if (flag & a_flags)
			{
				for (auto callback : callbackList)
					callback(flag);
			}
		}
	}

}