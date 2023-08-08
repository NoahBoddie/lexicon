#pragma once

#include "Lexicon/Component.h"

namespace LEX
{
	void Component::AttemptCompleteValidation() const
	{
		if (IsForcedInvalid() == true || IsPartiallyValid() == false)
			return;

		_flags |= ComponentFlags::ValidateComplete;
	}

	void Component::AttemptValidation() const { _flags |= ComponentFlags::ValidateSuccess; }

	void Component::AttemptFullValidation() const
	{
		//Basically, if it has no link, it will attempt to jump the whole process all together
		if (GetLinkerFlags() == LinkerFlags::None) {
			AttemptValidation();
			AttemptCompleteValidation();
		}
	}

	void Component::FinalizeLinkage(LinkerFlags link)
	{
		auto end = _linkerContainer.rend();
			
		for (auto it = _linkerContainer.rbegin(); it != end;)
		{
			LinkerFlags& link_tasks = it->second;

			LinkerFlags flag = link & link_tasks;


			bool can_validate = true;

			if (!flag)
				goto _continue;

			it->first->HandleLink(flag);

			//If the validation has failed, it will cease to attempt to validate it.
			can_validate = it->first->IsForcedInvalid();

			if ((link_tasks &= ~flag) &&  !can_validate)
			{
			_continue:
				it++;
			}
			else
			{
				auto del = it++;
				if (can_validate)
					del->first->AttemptCompleteValidation();

				_linkerContainer.erase(del->first);
			}
		}

		_linkCheckFlags |= link;

		RGL_LOG(trace, "Finalized linkage: {}", (int)link);

		//Should it have processed everything it should remove it all.
	}





	ComponentType Component::GetComponentType() const { return _type; }
 
	bool Component::IsValid() const { return (_flags & ComponentFlags::ValidationFlags) == ComponentFlags::ValidateComplete; }

	//Checks if the object had success, regardless if the object itself is still invalid.
	bool Component::IsPartiallyValid() const { return !!(_flags & ComponentFlags::ValidateSuccess); }

	//Checks if the object is forcibly invalid due to error.
	bool Component::IsForcedInvalid() const { return !!(_flags & ComponentFlags::VailidateFailure); }

	LinkerFlags Component::GetLinkerFlags() const { return LinkerFlags::None; }

	//We return false because if something sets up linker flags but never sets up the link
	// handle, it should fail.
	bool Component::OnLinkage(LinkerFlags link_type) { return false; }

	void Component::HandleLink(LinkerFlags link_type)
	{
		//TODO:Component::HandleLink needs something to be able to handle the linking of one object before the linking of another. Mostly environment related.

		bool result = OnLinkage(link_type);


		//Its also possible the impl version of the call can do this for me.
		if (result) {
			_flags |= ComponentFlags::ValidateSuccess;
		}
		else {
			_flags |= ComponentFlags::VailidateFailure;
		}
	}


	void Component::LogError(std::string log)
	{
		//Logs issue then flags the error flag. Basically supposed to be used when a core error is detected, rendering the object no longer
		// usable, but can still fire off.
	}

	//Just record?
	//Should take 2 different iterators.

	Component* Component::Create(Record& rec, ComponentType type)
	{
		Component* comp = ComponentFactory::Create(type);

		comp->_type = type;

		//would i even need such a thing?
		comp->ConstructFromRecord(rec);

		return comp;
	}




	void Component::ConstructFromRecord(Record& rec)
	{
		RGL_LOG(debug, "{}", rec.GetTag());

		//begin = &(*data_view);

		LoadFromRecord(rec);


		//end = &(*data_view);

		CheckLinkValidation();

		_flags |= ComponentFlags::LoadedFromRecord;
	}

	void Component::CheckLinkValidation()
	{
		LinkerFlags links = GetLinkerFlags();

		if (!!links) {
			//auto& link_entry = _linkerContainer[this];
			_linkerContainer[this] = links;
				
		}
		else {
			//No flags taken, immediately validated
			_flags |= ComponentFlags::ValidateComplete;
			//AttemptValidation();
			//AttemptCompleteValidation();
		}
	}
}
