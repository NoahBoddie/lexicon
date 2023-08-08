#pragma once



namespace LEX
{
	class RoutineArgument;
	class Component;

	ENUM(ComponentType)
	{
		Invalid,		//Type was created outside of factory, and thus not a viable type.
		Subroutine,
		Coroutine,
		GlobalVariable,
		//Prototype,
	};

	ENUM(ComponentFlags, uint8_t)
	{
		Invalid = 0,
		VailidateFailure = 0b001, //Setting this will fail it to validate, even if it validates later.
		ValidateSuccess = 0b010, //Flag to designate primary validation step achieved
		ValidateComplete = 0b110, //Flag for validation completion, comprised of success for easier comparison.

		ValidationFlags = 0b111, //The sum total of all flags used for validation.


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
	

	using ComponentFactory = ClassFactory<Component, ComponentType>;


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
		virtual ComponentType GetComponentType() const = 0;

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

		//This is used later on to signify that if All flags have been done, linkage doesn't have to wait.
		inline static LinkerFlags _linkCheckFlags = LinkerFlags::None;


		//inline static std::map<LinkerFlags, std::list<Component*>> linkerMap;
		//inline static std::map <Component*, uint32_t> linkerCount;


		void AttemptCompleteValidation() const;

	protected:

		void AttemptValidation() const;

		void AttemptFullValidation() const;



	public:
		static void FinalizeLinkage(LinkerFlags link);





		ComponentType GetComponentType() const override;
 
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

		static Component* Create(Record& rec, ComponentType type);

		template<std::derived_from<Component> D>
		inline static D* Create(Record& rec)
		{
			//I prefer this.
			if constexpr (!std::is_same<D, D::Class>)
			{
				//Actually supposed to be invalid if so. 
				return nullptr;
			}
			else
			{
				constexpr ComponentType type = D::FactoryType;

				return dynamic_cast<D*>(Create(rec, type));

			}
		}

		template<std::derived_from<Component> D>
		inline static D* CreateAs(Record& rec)//, ComponentType type)
		{
			if constexpr (!std::is_same<D, D::Class>)
			{
				//Actually supposed to be invalid if so. 
				return nullptr;
			}
			else
			{
				constexpr ComponentType type = D::FactoryType;

				return dynamic_cast<D*>(Create(rec, type));

			}
		}

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
		//Feel like I wanna move this.
		Component() = default;
		Component(const Component&) = delete;
		Component(const Component&&) = delete;
		Component& operator= (Component) = delete;
		Component& operator= (const Component&) = delete;
		Component& operator= (const Component&&) = delete;
		~Component() { _linkerContainer.erase(this); }

	private:
		ComponentType _type = ComponentType::Invalid;
		mutable ComponentFlags _flags = ComponentFlags::Invalid;

	};


	//This has 2 more derived classes, one that gets the generic record it derives from, and then another that gets the beginning and end
	// elements of the data. I'll be honest, this is just so I don't have to make an object to store it over and over again.

	//Thinking of hard coding this in templates which one should be done

}
