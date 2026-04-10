#pragma once

#include "Lexicon/LinkFlag.h"
#include "Lexicon/ComponentType.h"
#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Engine/SyntaxRecord.h"
#include "Lexicon/Interfaces/IComponentBase.h"
#include "Lexicon/Interfaces/LinkMessenger.h"

namespace LEX
{
	class Component;
	
	ENUM(ComponentFlag, uint8_t)
	{
		None = 0,
		
		Initialized = 1 << 0, //flag to say that load from view has finished once.
		Linking		= 1 << 1,
		Linked		= 1 << 2,  //Flag determines that a check for linking occured, not entirely that all links are done.
	};

	ENUM(ValidationFlag, uint8_t)
	{
		None = 0,
		Invalid = 1 << 0, //Setting this will fail it to validate, even if it validates later.
		Success = 1 << 1, //Flag to designate primary validation step achieved
		Complete = 1 << 2,		
		Valid = ValidationFlag::Complete | ValidationFlag::Success, //Flag for validation completion, comprised of success for easier comparison.

		//success and valid have switched as a single success doesnt make it valid.
	};
	
	enum struct LinkResult
	{
		Success,
		Failure,
	};




	class Component : public IComponentBase
	{
	public:
		DEFINE_COMPONENT_OFFSET(ComponentType::Component)

	private:


		inline static std::recursive_mutex link_mutex;//Used to prevent refresh and regular link from going off at once.
		inline static bool isProcessing = false;
		inline static LinkFlag processingFlags{};//this seems really useless for the most part.
		inline static LinkFlag reprisalFlags{};
		inline static LinkFlag completedFlags{};
		inline static LinkFlag waitingFlags{};//A list of the flags currently waiting to be processed.

		inline static std::list<Component*> g_linkerList;

		//Whenever an error is encountered that prevents this from finishing linking, it will put
		// the message here so attempts to call upon it will preserve what the error was.
		//inline static std::unordered_map<Component*, std::string> errorTable;

		using iterator = decltype(g_linkerList)::iterator;


		//To handle link dependency is to handle linking that occurs when things are all complete. I think to that, dependency will fire when _linkCheckFlags
		// equal LinkFlag::All. 
		//A secondary problem is also when a component dies it may not be able to pull itself out of the component library. To that, I think it would be better
		// to turn this into a vector of pairs, and when something is removed from existing, it will pull it's dependency. I can probably make a function for this, along with
		// a flag. The idea would basically be that if the left hand doesn't exist, we don't worry about depedency, but if the right hand doesn't exist a dependency has been
		// compromised.
		// ^Definitely harder/longer to search but it will only need to be searched once.
		//Core to the concept of this would be to pluck every the pair every time consideration is going off, and then to make the function recursive, and able
		// to resolve other components questions.
		inline static std::map<Component*, std::vector<size_t>> _dependencyMap{};

		inline static std::vector<Component*> _dependeeList{};


		//Limit the use of a recordless create by seeing if load from record has been implemented.
		template<class D>
		static D* CreateImpl(SyntaxRecord* rec = nullptr) requires(!std::is_abstract_v<D>)
		{
			D* comp = new D();
			
			comp->Initialize(rec);

			return comp;
		}
	public:
		//TODO: I need creation via enum again, only this time it's got to be done via ExpressionType

		//TODO: Need some way to check if load from record is implemented, controlling if I can do it without
		// a direct record. It should be as relatively simple as checking the Template type.


		template<std::derived_from<Component> D>
		static D* Create(SyntaxRecord* rec = nullptr) requires(!std::is_abstract_v<D>)
		{
			return CreateImpl<D>(rec);
		}

		template<std::derived_from<Component> D>
		static D* Create(SyntaxRecord& rec) requires(!std::is_abstract_v<D>)
		{
			return Create<D>(&rec);
		}
		
		template<std::derived_from<Component> D>
		static D* Create(SyntaxRecord&& rec) requires(!std::is_abstract_v<D>)
		{
			return Create<D>(&rec);
		}
		//A final create should able to be done via string view, THAT will be using class factory, but such a thing just isn't needed yet.

		//If I could have a version of this that functions without the factory, that would be ideal.
		

		//Contrary to what I stated would be important, without routine items, there's no reason to have this load from data anymore.
		virtual void OnInit(SyntaxRecord& rec)
		{
			//should likely be a pure virtual, but holding off.
		}








		void Initialize(SyntaxRecord* rec)
		{
			if (IsInitialized() == false)
			{
				if (rec)
					OnInit(*rec);

				_flags |= ComponentFlag::Initialized;

				HandleLinkage();
			}
			
		}
		
		void Initialize(SyntaxRecord& rec)
		{
			return Initialize(&rec);
		}

		bool IsInitialized() const
		{
			return _flags & ComponentFlag::Initialized;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		static auto& GetProcessingFlags()
		{
			return reinterpret_cast<std::atomic<LinkFlag>&>(processingFlags);
		}

		static LinkFlag CurrentFlags()
		{
			return completedFlags | processingFlags;
		}

		static bool IsProcessing()
		{
			return isProcessing || GetProcessingFlags();
		}


		//Registers component for linking. Returns false if no linking is required.
		bool RegisterLinkComponent()
		{
			//if it has tasks but isn't linking it failed, and isn't valid to try to link anymore.
			if (_tasks) {
				return IsLinking();
			}

			//return early if already registered.

			LinkFlag links = GetLinkFlags();

			if (links) {
				//std::lock_guard lock(link_mutex);
				//TODO:Set up reprisal and waiting here
				FlagLinking(true);
				_tasks = links;


				waitingFlags |= links;

				bool current_can_handle = get_front_flag(links) & processingFlags;

				if (IsProcessing() && !current_can_handle) {
					reprisalFlags |= links & CurrentFlags();
				}

				g_linkerList.emplace_back(this);
			}

			return links;
		}

		static auto UnregisterLinkComponent(const iterator& it)
		{
			std::lock_guard lock(link_mutex);

			if (g_linkerList.end() != it) {				
				(*it)->FlagLinking(false);
				return g_linkerList.erase(it);
			}

			return it;
		}


		auto FindLinkEntry()
		{

			return std::find_if(g_linkerList.begin(), g_linkerList.end(),
				[this](Component* it) {return it == this; });

		}

		auto UnregisterLinkComponent()
		{
			std::lock_guard lock(link_mutex);

			auto it = FindLinkEntry();

			return UnregisterLinkComponent(it);
		}






		static bool LinkComponent(iterator& it, LinkFlag flags, bool grouped)
		{
			//flags &= processingFlags | completedFlags;

			bool complete = false;

			bool inc = true;

			if (flags)
			{

				std::lock_guard lock(link_mutex);
				Component* target = *it;
				LinkFlag& tasks = target->_tasks;
				

				//If there are tasks the component has not processed yet it has reached this stage,
				// it will attempt to play catch up.

				auto prim = grouped ? flags : tasks;
				auto aux = grouped ? tasks : flags;

				bit_loop(prim)
				{
					if (grouped && get_front_flag(tasks) != i) {
						bit_break;
					}

					bool flag_allowed = aux & i;

					//auto should = target->ShouldLink(i);
					if (flag_allowed) //&& target->ShouldLink(i) == true)
					{
						LinkResult result = LinkResult::Failure;

						logger::trace("Linking {}: {}", target->GetName(), magic_enum::enum_name(i));

						if (SafeInvoke<Error>(true, [&]() {result = target->OnLink(i); }) == true)
						{
							report::link::warn("Component '{}' has suffered an error and failed the {} link stage.", target->GetName(), magic_enum::enum_name(flags));
						}

						bool invalid;

						//Its also possible the impl version of the call can do this for me.
						if (result == LinkResult::Success) {
							target->FlagAsSuccess();
							invalid = false;
						}
						else {
							target->FlagAsInvalid();
							invalid = true;
						}

						tasks &= ~i;

						bool is_done = !(tasks & LinkFlag::Complete);
						//This isn't to fire on links like final or exit.
						bool public_link = (i & LinkFlag::Complete);

						if (is_done || invalid)
						{
							if (is_done && public_link)
								complete = true;

							if (!invalid)
								target->TryValidate();

							if (!tasks) {
								it = UnregisterLinkComponent(it);
								inc = false;
							}
						}
					}
				}

				if (!grouped && complete) {
					target->OnLinkComplete();
					complete = false;
				}
			}
			
			if (inc) {
				it++;
			}


			return complete;
		}






		static void LinkComponentsImpl(LinkFlag flags)
		{
			processingFlags = flags;
			

			//Multiple different threads can use this

			std::vector <Component*> finished{};

			bit_loop(flags)
			{
				if (~completedFlags & i) {
					report::link::info("Starting link stage: {} ", magic_enum::enum_name(i));
				}

				bool remove_waiting = true;

				for (auto it = g_linkerList.begin(); it != g_linkerList.end();)
				{
					std::lock_guard lock(link_mutex);

					Component* target = *it;

					if (LinkComponent(it, i, true) == true) {
						finished.push_back(target);
					}

					//If remaining still contains the flag we're processing we
					if (target->_tasks & i) {
						remove_waiting = false;
					}
				}

				if (remove_waiting) {
					waitingFlags &= ~i;
				}

				if (~completedFlags & i) {
					report::link::info("Finalized link stage: {} ", magic_enum::enum_name(i));
				}

				completedFlags |= i;
				processingFlags &= ~i;

				LinkMessenger::instance->Dispatch(i);
			}

			//This removes messages for stuff we already sent.

			


			for (auto& target : finished) {
				target->OnLinkComplete();
			}

			
		}

		static void LinkComponents(LinkFlag flags)
		{
			//This lock (hopefully) will only allow one thing to check 

			//static std::mutex mutex;

			//I'd like this to use recursive

			LinkFlag send;

			//I'd like this to be locked to one thread when examining, and then released
			// to allow other threads to see that we are currently processing, and for them
			// to reprise later.
			{
				static std::mutex mutex;

				std::lock_guard lock(mutex);

				//This ensures that lesser link flags will be executed, 
				// but also that completed flags won't be repeated unless it's also waited upon.
				send = LinkFlag((1 << std::bit_width<std::underlying_type_t<LinkFlag>>(flags)) - 1);
				
				LinkFlag remove = completedFlags & ~waitingFlags;
				send &= ~(remove);

				if (IsProcessing() == true) {
					reprisalFlags |= send;
					return;
				}

				
				isProcessing = true;
			}

			while (send)
			{
				LinkComponentsImpl(std::exchange(send, LinkFlag::None));

				if (reprisalFlags) {
					send = std::exchange(reprisalFlags, LinkFlag::None);
				}
			}
			
			isProcessing = false;
		}

		static void RelinkComponents()
		{
			if (!completedFlags)
				return;

			//Should I be locking this? I want to prevent the completed from changing during this point.
			//std::lock_guard lock(link_mutex);

			LinkComponents(completedFlags);

		}


		void RelinkComponent()
		{
			auto it = FindLinkEntry();

			if (g_linkerList.end() != it) {
				LinkComponent(it, completedFlags, false);
			}
		}




		//This name is being taken because the function is used in one place and is small. Better inlined.
		void HandleLinkage()//private
		{
			if (RegisterLinkComponent() == false) {
				FlagAsSuccess();
				TryValidate();
			}
		}


		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		static bool HasLinked(LinkFlag flag)
		{
			return flag & completedFlags;
		}


		static LinkFlag FlagsLinked()
		{
			return completedFlags;
		}

		virtual std::string_view GetName() const = 0;

		virtual LinkResult OnLink(LinkFlag flags) { return LinkResult::Failure; }

		virtual LinkFlag GetLinkFlags() { return LinkFlag::None; }
		
		virtual bool ShouldLink(LinkFlag)
		{
			return true;
		}

		//This is a function that gets called when the entire linking process completes.
		virtual void OnLinkComplete() {}

		bool SuccessFlag() const
		{
			return (_valid & ValidationFlag::Success);
		}

		bool InvalidFlag() const
		{
			return (_valid & ValidationFlag::Invalid);
		}

		

		//more protected. By default, this is a question about the options of the derived class.
		virtual bool GetValid() const { return true; }
		
		bool IsFlaggedValid() const
		{
			return _valid == ValidationFlag::Valid;
		}
		bool IsValid() const
		{
			bool result = IsFlaggedValid();
			bool was_valid = result;

			if (result)
				result = GetValid();

			if (result != was_valid) {
				FlagAsInvalid();
				//Make some notification
			}
			

			return result;
		}

		
		//Gets if component is currently waiting on a link stage
		bool IsLinking() const
		{
			return _flags & ComponentFlag::Linking;
		}


		

	private:
		void FlagLinking(bool value)
		{
			if (value)
				_flags |= ComponentFlag::Linking;
			else
				_flags &= ~ComponentFlag::Linking;
		}


		void FlagAsSuccess() const
		{
			_valid |= ValidationFlag::Success;
		}
	public:

		void FlagAsInvalid() const
		{
			_valid |= ValidationFlag::Invalid;
		}

		bool TryValidate()
		{
			if (InvalidFlag() == true || SuccessFlag() == false)
				return false;

			_valid |= ValidationFlag::Complete;
			
			return true;
		}


	private:
	

	public:
		Component() = default;
		Component(const Component&) = delete;
		Component(const Component&&) = delete;
		Component& operator= (const Component&) = delete;
		Component& operator= (const Component&&) = delete;
		virtual ~Component() { UnregisterLinkComponent(); AbsolveDependency(); }

	private:
		
		



public:
		void AbsolveDependency() const
		{
			_dependencyMap.erase(const_cast<Component*>(this));
			
			if (auto it = std::find(_dependeeList.begin(), _dependeeList.end(), this); _dependeeList.end() != it)
				*it = nullptr;
		}

		static void ClearDependencies() 
		{
			_dependencyMap.clear();
			_dependeeList.clear();
			_dependeeList.shrink_to_fit();

		}

		size_t GetDependee() const
		{
			if (this)
			{
				size_t size = _dependeeList.size();

				for (size_t i = 0; i < size; i++)
				{
					if (_dependeeList[i] == this)
					{
						return i;
					}
				}
			}

			return max_value<size_t>;
		}

		size_t ObtainDependee()
		{
			size_t dep = GetDependee();

			if (dep == max_value<size_t>)
			{
				dep = _dependeeList.size();
				_dependeeList.push_back(this);
			}

			return dep;
		}

		void AddDependency(Component* component)
		{
			if (component)
			{
				size_t index = component->ObtainDependee();

				if (auto it = _dependencyMap.find(this); _dependencyMap.end() != it)
				{
					auto& list = it->second;

					if (auto res = std::find(list.begin(), list.end(), index); list.end() != res) {
						list.push_back(index);
					}
				}
			}
		}

		void RemoveDependency(Component* component)
		{
			size_t index = component->GetDependee();

			if (auto it = _dependencyMap.find(this); _dependencyMap.end() != it)
			{
				auto& list = it->second;

				auto res = std::find(list.begin(), list.end(), index);

				list.erase(res);
			}
		}


		bool IsDependeeOf(Component* component) const
		{
			component->IsDependentTo(this);
		}

		bool IsDependentTo(const Component* component) const
		{
			if (component)
			{
				size_t index = component->GetDependee();

				if (index != max_value<size_t>)
				{
					if (auto it = _dependencyMap.find(const_cast<Component*>(this)); _dependencyMap.end() != it)
					{
						auto& list = it->second;

						auto res = std::find(list.begin(), list.end(), index);

						return list.end() != res;
					}
				}
			}

			return false;
		}

		template <typename T>
		T& GetComponentData() const noexcept
		{
			return reinterpret_cast<T&>(_data);
		}

	private:




		//TODO: Get rid of this any anything that uses it.
		mutable ComponentFlag _flags = ComponentFlag::None;
		mutable ValidationFlag _valid = ValidationFlag::None;
		mutable LinkFlag _tasks = LinkFlag::None;
		//I may store link flags here too.
		//Data usable by any person to store personal data here. After all, it's free space.
		mutable uint32_t _data = 0;
	};

	//This has 2 more derived classes, one that gets the generic record it derives from, and then another that gets the beginning and end
	// elements of the data. I'll be honest, this is just so I don't have to make an object to store it over and over again.

	//Thinking of hard coding this in templates which one should be done

}
