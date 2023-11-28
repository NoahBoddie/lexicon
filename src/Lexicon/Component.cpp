#pragma once

#include "Lexicon/Component.h"

namespace LEX
{

	void Component::ClearDependency()
	{
		_dependencyContainer.erase(this);
	}

	void Component::AddDependency(Component* component)
	{
		if (component)
		{
			if (auto it = _dependencyContainer.find(this); _dependencyContainer.end() != it)
			{
				auto& list = it->second;

				if (auto res = std::find(list.begin(), list.end(), component); list.end() != res) {
					list.push_back(component);
				}
			}
		}
	}

	void Component::RemoveDependency(Component* component)
	{
		if (auto it = _dependencyContainer.find(this); _dependencyContainer.end() != it)
		{
			auto& list = it->second;
			
			auto res = std::find(list.begin(), list.end(), component);

			list.erase(res);
		}
	}

	bool Component::IsDependent(Component* component)
	{
		if (component)
		{
			if (auto it = _dependencyContainer.find(this); _dependencyContainer.end() != it)
			{
				auto& list = it->second;

				auto res = std::find(list.begin(), list.end(), component);

				return list.end() != res;
			}
		}

		return false;
	}


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
	/*
	Component* Component::Create(ComponentType type, Record* rec)
	{
		Component* comp = ComponentFactory::Create(type);
		//I would like to move this into another function called init
		comp->_type = type;

		if (rec)
			comp->ConstructFromRecord(*rec);
		else {
			comp->CheckLinkValidation();
		}
		return comp;
	}

	Component* Component::Create(ComponentType type, Record& rec)
	{
		return Create(type, &rec);
	}
	//*/



	void Component::ConstructFromRecord(Record& rec)
	{
		RGL_LOG(debug, "{}", rec.GetTag());

		LoadFromRecord(rec);

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
