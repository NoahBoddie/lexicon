#pragma once

//So I never have to use forward declaration, this needs to include nothing.

namespace LEX
{
	//I think I'll actually be putting the external interface IN external. Nah, that would increase size. But 
	// It should contain a type alias to it's current interface type as convience.

	struct External;
	 
	using ExternalCtor = External * (*)();

	namespace detail
	{
		struct ExternalInterface
		{
			virtual ~ExternalInterface() = default;

			enum struct Version
			{
				V1,

				Current = V1
			};

			enum struct Update
			{
				Invalid,//Query is invalid
				Match,//Update versions match
				Library,//Library is out of date
				Engine//Engine is out of date
				
			};

		};
	}

	namespace Version1
	{
		//This answers questions
		struct ExternalInterface : public detail::ExternalInterface
		{

			//Largely, this doesn't need to be changed between implementations, as someone should always be
			// using Current, and whatever version they're on will sort it out.
			virtual Version GetVersion() final { return Version::Current; }

			//For all functions within the external interface, I could possibly make it so there are rules on
			// operating on new interfaces and such. This may allow one to use newer plugins with older engines.
			// however, this is likely, a bad idea.


			//The Non-virtual Functions

			//This call will use the version given from the 
			Update CheckUpdate()
			{
				//Likely will rename, this function also might be moved to a more static environment.
				if (!this)
					return Update::Invalid;

				auto version = GetVersion();

				if (version == Version::Current)
					return Update::Match;
				else if ((int)version < (int)Version::Current)
					return Update::Library;
				
				else
					return Update::Engine;

			}
		};
	}

	using ExternalInterface = Version1::ExternalInterface;

	
}