#pragma once



namespace LEX
{
	class RoutineArgument;
	class Component;
	//Deprecated
	ENUM(ComponentType_)
	{
		Invalid,		//Type was created outside of factory, and thus not a viable type.
		Subroutine,
		Coroutine,
		GlobalVariable,

		Project,
		Script,
		CommonScript
		//Prototype,
		//Needs the space for the environment bunch.
	};

	ENUM(ComponentFlags, uint8_t)
	{
		Invalid = 0,
		VailidateFailure = 0b001, //Setting this will fail it to validate, even if it validates later.
		ValidateSuccess = 0b010, //Flag to designate primary validation step achieved
		ValidateComplete = 0b110, //Flag for validation completion, comprised of success for easier comparison.

		ValidationFlags = 0b111, //The sum total of all flags used for validation.
		//To check for validation, you use & on the flags and see if it's equal to ValidateComplete

		ErrorFlag = 1 << 3,//Used when an error is spoken to not print it over and over again. May make more.
		LoadedFromRecord = 1 << 4, //flag to say that load from view has finished once.
	};

	ENUM(LinkerFlags, uint8_t)
	{
		None,
		Object		= 1 << 0,	//Used for objects who's reference of may not be visable on the spot.
		Declaration	= 1 << 1,	//Used when all functions have finished being declared, meaning their names exist.
		Definition	= 1 << 2,	//Used when all functions have finished being defined, meaning they can be called. Also links external funcs.
								// (for AVG, called in postpostload)
		External	= 1 << 3,	//Used for the reference of properties or formulas and as such must be handled after load.
		
		All = LinkerFlags::Object | LinkerFlags::Declaration | LinkerFlags::Definition | LinkerFlags::External,

		Total//What the fuck does this do lmao
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


	//IComponent has absolutely no reason to exist further. Slated for deletion
	struct IComponent
	{
		//TODO: Turn off IComponent's RecordConstructible data, and move to main component.
		virtual void ConstructFromRecord(Record& record){}
		//Record* GetRecord() override { return nullptr; }

		//TODO:Remove the IComponent::IsValid from virtual, and add a secondary one to determine extra validity.
		virtual bool IsValid() const { return true; }

		//A lot of delcaration is going to have to be moved here, including load from view
		// of course, all just declarative. I know this is a mite shit, but it'll have to do.


		//This is used to tell if an object is a certain type or not. Useful for delegate that need to know if it's actually storing a subroutine
		// or if the subroutine was shortened down to it's results, or if it was shortened down to a single call.
		//virtual ComponentType GetComponentType() const = 0;

		virtual ~IComponent() = default;
	};


	struct Component : public IComponent
	{
	private:
		struct LinkingData//Kinda useless?
		{
			LinkerFlags flags{};
			size_t count{};
		};
		//I'd like to use this instead.
		//std::map<Component*, LinkingData> _linkerContainer{};
		inline static std::map<Component*, LinkerFlags> _linkerContainer{};

		//To handle link dependency is to handle linking that occurs when things are all complete. I think to that, dependency will fire when _linkCheckFlags
		// equal LinkerFlags::All. 
		//A secondary problem is also when a component dies it may not be able to pull itself out of the component library. To that, I think it would be better
		// to turn this into a vector of pairs, and when something is removed from existing, it will pull it's dependency. I can probably make a function for this, along with
		// a flag. The idea would basically be that if the left hand doesn't exist, we don't worry about depedency, but if the right hand doesn't exist a dependency has been
		// compromised.
		// ^Definitely harder/longer to search but it will only need to be searched once.
		//Core to the concept of this would be to pluck every the pair every time consideration is going off, and then to make the function recursive, and able
		// to resolve other components questions.
		inline static std::map<Component*, std::vector<Component*>> _dependencyContainer{};

		//This is used later on to signify that if All flags have been done, linkage doesn't have to wait.
		inline static LinkerFlags _linkCheckFlags = LinkerFlags::None;


		//inline static std::map<LinkerFlags, std::list<Component*>> linkerMap;
		//inline static std::map <Component*, uint32_t> linkerCount;


		void AttemptCompleteValidation() const;

	protected:
		//A note on these dependencies and validity, ONLY care about whether it itself is valid, and if it isn't, force
		// invalidity on things that depend on it.
		void ClearDependency();
		void AddDependency(Component* component);
		void RemoveDependency(Component* component);
		bool IsDependent(Component* component);


		void AttemptValidation() const;

		void AttemptFullValidation() const;



	public:
		static void FinalizeLinkage(LinkerFlags link);





		//ComponentType GetComponentType() const { return _type; }
 
		bool IsValid() const override;

		//Checks if the object had success, regardless if the object itself is still invalid.
		bool IsPartiallyValid() const;

		//Checks if the object is forcibly invalid due to error.
		bool IsForcedInvalid() const;

		virtual LinkerFlags GetLinkerFlags() const;

		//We return false because if something sets up linker flags but never sets up the link
		// handle, it should fail.
		virtual bool OnLinkage(LinkerFlags link_type);

		void HandleLink(LinkerFlags link_type);


		void LogError(std::string log);

		//Just record?
		//Should take 2 different iterators.
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
		static D* _Create(Record* rec = nullptr)
		{
			D* comp = new D();

			if (rec)
				comp->ConstructFromRecord(*rec);
			else {
				comp->CheckLinkValidation();
			}

			return comp;
		}
	public:
		//TODO: I need creation via enum again, only this time it's got to be done via ExpressionType

		//TODO: Need some way to check if load from record is implemented, controlling if I can do it without
		// a direct record. It should be as relatively simple as checking the Template type.


		template<std::derived_from<Component> D>
		static D* Create(Record* rec = nullptr)
		{
			return _Create<D>(rec);
		}

		template<std::derived_from<Component> D>
		static D* Create(Record& rec)
		{
			return Create<D>(&rec);
		}
		//A final create should able to be done via string view, THAT will be using class factory, but such a thing just isn't needed yet.

		//If I could have a version of this that functions without the factory, that would be ideal.
		

		//Contrary to what I stated would be important, without routine items, there's no reason to have this load from data anymore.
		virtual void LoadFromRecord(Record& rec)
		{
			//should likely be a pure virtual, but holding off.
		}


		//virtual void LoadFromRecordData(RGL::RecordDataIterator& data_view){RGL_LOG(info, "{}", data_view->view);}



		void ConstructFromRecord(Record& rec) override;


	private:
		void CheckLinkValidation();

	public:
		//Feel like I wanna move this. Should likely befriend its factory instead.
		// I should mirror the private constructed class singleton cobb made instead.
		Component() = default;
		Component(const Component&) = delete;
		Component(const Component&&) = delete;
		Component& operator= (Component) = delete;
		Component& operator= (const Component&) = delete;
		Component& operator= (const Component&&) = delete;
		virtual ~Component() { _linkerContainer.erase(this); }

	private:
		//ComponentType _type = ComponentType::Invalid;
		mutable ComponentFlags _flags = ComponentFlags::Invalid;

	};

	//This has 2 more derived classes, one that gets the generic record it derives from, and then another that gets the beginning and end
	// elements of the data. I'll be honest, this is just so I don't have to make an object to store it over and over again.

	//Thinking of hard coding this in templates which one should be done

}
