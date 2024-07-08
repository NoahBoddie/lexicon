#pragma once

namespace LEX
{

	//Give own file.
	enum struct Update
	{
		Missing,//desired plugin doesn't even exist.
		Match,//Update versions match
		Invalid,//Query is invalid
		Library,//Library is out of date
		Engine,//Engine is out of date

	};

	struct Interface
	{
		virtual ~Interface() = default;

		//Used to help see if a request is valid. This should only be incrementing when the object has a new iteraction of
		// it's vtable basically. There's an incompatibility for requests newer than can be provided.
		virtual uintptr_t Version() const = 0;

		//Used for specific handling of versions. Say a function has to change functionality and the expectation is no longer valid.

		virtual bool CanHandle(uintptr_t) const { return true; }
	
	protected:
		Interface() = default;

		Interface(Interface&&) = delete;
		Interface(const Interface&) = delete;

		Interface& operator=(Interface&&) = delete;
		Interface& operator=(const Interface&) = delete;
	};

}