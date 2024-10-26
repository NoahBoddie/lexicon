#pragma once

#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Engine/SyntaxRecord.h"
namespace LEX
{
	class RoutineArgument;
	class Component;
	
	ENUM(ComponentFlag, uint8_t)
	{
		None = 0,
		Invalid		= 0b001, //Setting this will fail it to validate, even if it validates later.
		Valid		= 0b010, //Flag to designate primary validation step achieved
		Success		= 0b110, //Flag for validation completion, comprised of success for easier comparison.

		Validation	= 0b111, //All validation flags at once. To get use '&' on flags and get equal to success

		Initialized = 1 << 4, //flag to say that load from view has finished once.
		Linked		= 1 << 5  //Flag determines that a check for linking occured, not entirely that all links are done.
	};

	ENUM(LinkFlag, uint8_t)
	{
		None,
		Loaded		= 1 << 0,   //Used to tell when all scripts have loaded
		Declaration	= 1 << 1,	//Used when all functions have finished being declared, meaning their names exist.
		Definition	= 1 << 2,	//Used when all functions have finished being defined, meaning they can be called. Also links external funcs.
								// (for AVG, called in postpostload)
		Object		= 1 << 3,	//Used for objects who's reference of may not be visable on the spot.

		External	= 1 << 4,	//Used for the reference of properties or formulas and as such must be handled after load.
		
		All			= LinkFlag::Loaded | LinkFlag::Object | LinkFlag::Declaration | LinkFlag::Definition | LinkFlag::External,

		//Final happens when all linking is done, notably, what also happens here is a bid for dependency.
		// Basically by now if it's not loaded properly, it will never be. Final also cannot be manually selected.
		//TODO: So for the above, Component::Link needs a wrapped function.
		Final		= 0xFF, 
	};
	
	enum struct LinkResult
	{
		Success,
		Failure
	};

	struct ComponentType
	{
		ComponentType() = default;

		ComponentType(const std::type_info& id){
			info = &id;
		}

		constexpr bool operator==(const ComponentType& rhs) const{
			return info == rhs.info;
		}

		constexpr bool operator==(const ComponentType&& rhs) const{
			return info == rhs.info;
		}

		constexpr bool operator==(const std::type_info& rhs) const {
			return info == &rhs;
		}


	private:
		const std::type_info* info = nullptr;
	};




	class Component
	{
	private:
	
	public:
		
	public:


		virtual ComponentType GetComponentType()
		{
			return typeid(*this);
		}

		ComponentType FetchComponentType()
		{
			return this ? GetComponentType() : ComponentType{};
		}


		template <std::derived_from<Component> T>
		bool IsComponentType() { return this ? (GetComponentType() == typeid(T)) : false; }

	private:
		//Limit the use of a recordless create by seeing if load from record has been implemented.
		template<class D>
		static D* _Create(SyntaxRecord* rec = nullptr)
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
		static D* Create(SyntaxRecord* rec = nullptr)
		{
			return _Create<D>(rec);
		}

		template<std::derived_from<Component> D>
		static D* Create(SyntaxRecord& rec)
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



















		//////////////
		//new names //
		//////////////
		/*
		Initialize- fires once, create uses, cannot be considered valid what so ever if it's not been fired (to which, it should be known that it's not valid).

		virtual OnLink
		virtual OnXyzLink (This part is manual)

		PartiallyValid->ValidFlag
		ForcedInvalid->InvalidFlag
		IsValid->virtual GetValid
		IsValid

		FinalizeLinkage->Link
		CheckLinkValidation ->
		GetLinkFlag-> GetLinkFlags
		OnLinkage -> OnLink
		IsLinked-


		AttempteValidation -> FlagAsValid
		<No function> -> FlagAsInvalid
		AttemptFullValidation-> delete, whatever used it can just manually check if linking is complete
		AttemptCompleteValidation-> TryValidate

		//*/





		//This name is being taken because the function is used in one place and is small. Better inlined.
		void HandleLinkage()//private
		{
			LinkFlag links = GetLinkFlags();

			if (!!links) {
				//auto& link_entry = _linkerContainer[this];
				_linkerContainer[this] = links;

			}
			else {
				TryValidate();
			}
		}

		void Initialize(SyntaxRecord* rec)
		{
			if (IsInitialized() == false)
			{
				if (rec)
					OnInit(rec->Transform<SyntaxRecord>());

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


		//With no record, on init shouldn't be used.
		//virtual void OnInit(Record& rec)
		static void Link(LinkFlag flags) 
		{
			//Make sure to remove the linkCheckFlags

			auto end = _linkerContainer.end();

			for (auto it = _linkerContainer.begin(); it != _linkerContainer.end();)
			{
				LinkFlag& tasks = it->second;

				LinkFlag flag = flags & tasks;

				Component* target = it->first;

				bool invalid = false;

				if (!flag)
					goto _continue;


				//Its also possible the impl version of the call can do this for me.
				if (target->OnLink(flag) == LinkResult::Success) {
					target->FlagAsValid();
				}
				else {
					target->FlagAsInvalid();
				}


				//If the validation has failed, it will cease to attempt to validate it.
				invalid = target->InvalidFlag();

				if ((tasks &= ~flag) && !invalid)
				{
				_continue:
					it++;
				}
				else
				{
					auto del = it;
					it++;

					//if (can_validate)
						target->TryValidate();
					//_linkerContainer.erase(del);
					_linkerContainer.erase(del);
				}
			}

			_linkCheckFlags |= flags;

			RGL_LOG(trace, "Finalized linkage: {}", (int)flags);

			//Should it have processed everything it should remove it all.
		}

		static bool HasLinked(LinkFlag flag)
		{
			return flag & _linkCheckFlags;
		}

		virtual LinkResult OnLink(LinkFlag flags) { return LinkResult::Failure; }

		virtual LinkFlag GetLinkFlags() { return LinkFlag::None; }
		
		//This is a function that gets called when the entire linking process completes.
		virtual void OnLinkComplete() {}

		bool ValidFlag() const
		{
			return (_flags & ComponentFlag::Valid);
		}

		bool InvalidFlag() const
		{
			return (_flags & ComponentFlag::Invalid);
		}

		

		//more protected. By default, this is a question about the options of the derived class.
		virtual bool GetValid() const { return true; }
		
		bool IsValid() const
		{
			return (_flags & ComponentFlag::Validation) == ComponentFlag::Success && GetValid();
		}

		//Checks flag and also the linker cache
		bool IsLinked() const
		{
			return _flags & ComponentFlag::Linked && _linkerContainer.contains(const_cast<Component*>(this));
		}
	private:
	
		void FlagAsValid() const
		{
			_flags |= ComponentFlag::Valid;
		}
	public:

		void FlagAsInvalid() const
		{
			_flags |= ComponentFlag::Invalid;
		}

		bool TryValidate()
		{
			if (InvalidFlag() == true || ValidFlag() == false)
				return false;

			_flags |= ComponentFlag::Success;
			
			return true;
		}


	private:
	

	public:
		Component() = default;
		Component(const Component&) = delete;
		Component(const Component&&) = delete;
		Component& operator= (Component) = delete;
		Component& operator= (const Component&) = delete;
		Component& operator= (const Component&&) = delete;
		virtual ~Component() { _linkerContainer.erase(this); AbsolveDependency(); }

	private:
		inline static std::map<Component*, LinkFlag> _linkerContainer{};
		



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
		T& GetComponentData() const
		{
			return reinterpret_cast<T&>(_data);
		}

		private:

		//This is used later on to signify that if All flags have been done, linkage doesn't have to wait.
		inline static LinkFlag _linkCheckFlags = LinkFlag::None;


		//ComponentType _type = ComponentType::Invalid;
		mutable ComponentFlag _flags = ComponentFlag::None;

		//Data usable by any person to store personal data here. After all, it's free space.
		mutable uint32_t _data = 0;
	};

	//This has 2 more derived classes, one that gets the generic record it derives from, and then another that gets the beginning and end
	// elements of the data. I'll be honest, this is just so I don't have to make an object to store it over and over again.

	//Thinking of hard coding this in templates which one should be done

}
