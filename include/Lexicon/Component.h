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
	

	//using ComponentFactory = ClassFactory<Component, ComponentType>;
	//template <class Derived, ComponentType I>
	//using ComponentMaker = FactoryComponent<Derived, Component, I>;

	//IComponent has absolutely no reason to exist further.
	// But theres no reason to get rid of it yet.
	struct IComponent : public IRecordConstructible
	{
		//TODO: Turn off IComponent's RecordConstructible data, and move to main component.
		void ConstructFromRecord(Record& record) override {}
		Record* GetRecord() override { return nullptr; }

		//TODO:Remove the IComponent::IsValid from virtual, and add a secondary one to determine extra validity.
		virtual bool IsValid() const { return true; }

		//A lot of delcaration is going to have to be moved here, including load from view
		// of course, all just declarative. I know this is a mite shit, but it'll have to do.


		//This is used to tell if an object is a certain type or not. Useful for delegate that need to know if it's actually storing a subroutine
		// or if the subroutine was shortened down to it's results, or if it was shortened down to a single call.
		//virtual ComponentType GetComponentType() const = 0;

		virtual ~IComponent() = default;
	};




	template<typename T> concept HasComponentType = requires 
	{ 
		requires std::derived_from<T, typename T::ComponentType>; 
	} && std::derived_from<T, Component>;


	struct ComponentID
	{
	public:
		ComponentID() = default;

	private:
		ComponentID(uint32_t id) : _index{ id } {}
	public:
		

		static ComponentID GetCreateID(std::string_view str)
		{
			auto size = _componentIDs.size();

			for (uint32_t i = 0; i < size; i++)
			{
				if (_componentIDs[i] == str)
					return i;
			}
			
			_componentIDs.push_back(str);

			return static_cast<uint32_t>(size);
		}

		std::string_view GetStringID()
		{
			if (full_value<uint32_t> == _index || _componentIDs.size() <= _index)
				return "";

			return _componentIDs[_index];
		}

	private:
		inline static std::vector<std::string_view> _componentIDs;

		uint32_t _index = full_value<uint32_t>;	
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

		/*
		static Component* Create(ComponentType type, Record* rec = nullptr);


		static Component* Create(ComponentType type, Record& rec);

		//A note about these, these shouldn't be factory functions, instead they should just regularly create the damn things, and assign
		// them with whatever factory id they have.
		template<std::derived_from<Component> D>
		inline static D* Create(Record* rec = nullptr)
		{
			//There's no reason to be making this off of a factory. Either make it manually, or have a function do so.

			//This should actually be asking if this thing is a factory base
			if constexpr (std::derived_from<D, ComponentFactory> == false)
			{
				//Actually supposed to be invalid if so. 
				return nullptr;
			}
			else
			{
				if constexpr (!std::is_same_v<D, D::Class>) {
					//Or throw.
					return nullptr;
				}
				constexpr ComponentType type = D::FactoryType;

				return dynamic_cast<D*>(Create(type, rec));

			}
		}

		template<std::derived_from<Component> D>
		inline static D* Create(Record& rec)
		{
			return Create<D>(&rec);
		}
		//*/

		//The concept of this create is going to be different. Instead, it will store a string_view to a given id (just an index). Then if we want to access
		// the string view we can just refer to the id.
		//There is a problem with this though, the construction will only allow it to turn into it's true type, nothing it derives from. So to pick an example
		// it's like if player character couldn't be seen as Character or actor. But I think I can solve that either with a second template parameter.

		//Lastly it's notable that the factory had this same issue with creation, as there's only one thing to create per each one. Notably, the factory system
		// has lost a good bit of it's worth since routine items, the main reason I needed the generic creation system, no longer exists.
		
		//Make all this shit it's own class or struct.
		
		//This should be a mutable const probably
		ComponentID id;
		template <std::derived_from<Component> T>
		bool IsComponentType() { return id.GetStringID() == typeid(T).name(); }

	private:
		//Limit the use of a recordless create by seeing if load from record has been implemented.
		template<class D, class B>
		static D* _Create(Record* rec = nullptr)
		{
			//I'm wondering if the type name should go through the wringer here to resolve not being component.

			std::string_view name = typeid(B).name();

			D* comp = new D();
			//This part can be a single function, make a function for it to use.
			comp->id = ComponentID::GetCreateID(name);

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

		template<HasComponentType D>
		static D* Create(Record* rec = nullptr)
		{
			using B = D::ComponentType;

			//I'm wondering if the type name should go through the wringer here to resolve not being component.
			return _Create<D, B>();
		}

		template<std::derived_from<Component> D>
		static D* Create(Record* rec = nullptr)
		{
			return _Create<D, D>(rec);
		}

		template<std::derived_from<Component> D>
		static D* Create(Record& rec)
		{
			return Create<D>(&rec);
		}
		//A final create should able to be done via string view, THAT will be using class factory, but such a thing just isn't needed yet.

		//If I could have a version of this that functions without the factory, that would be ideal.

		/*
		template <class Self>
		constexpr auto&& value(this Self&& self) {
			if (self.has_value()) {
				return std::forward<Self>(self).m_value;
			}
			throw bad_optional_access();
		}
		//*/

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
		~Component() { _linkerContainer.erase(this); }

	private:
		//ComponentType _type = ComponentType::Invalid;
		mutable ComponentFlags _flags = ComponentFlags::Invalid;

	};

	//This has 2 more derived classes, one that gets the generic record it derives from, and then another that gets the beginning and end
	// elements of the data. I'll be honest, this is just so I don't have to make an object to store it over and over again.

	//Thinking of hard coding this in templates which one should be done

}
