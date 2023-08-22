#pragma once

#include "Lexicon/Component.h"
#include "ExpressionType.h"
#include "Exception.h"
namespace LEX
{
	class Prototype;
	class Coroutine;

	//Make independent
	enum struct FieldQuery
	{
		Success,
		Failure,
		Ambiguous,
	};



	//*
	//Should be expressly abstract.
	struct Environment : public Component
	{
		//Note, the evironment itself is very interested in doing things in post declaration, so it can load it's script directives.




		//To pointer or not to pointer?
		//Record* header = nullptr;
		Record header;



		//The environment is the representation and holding body of it's functions and properties.
		//Environments have no parentage, as that would imply other top level scopes, or functions being used as environments.

		//What an environment will have is other environments that are also stored and managed, as sibling environments

		//It actually may have child environments, and this environment is likely going to be for structs (if I ever fucking make those).
		// But generally, an environment is going to be a 1 class show, not alone expecting any parents.



		//This might have some different way of accounting.
		std::vector<Coroutine*> functions;//





		//Need to make a GetFunction here to work here, so I can do something along the lin
	};
	//*/


	namespace TF
	{
		class Script;//define
		class Function;
		class Variable_;//Would be closer to global
	
		class Subroutine__;
		class TypePolicy;

	
		class Scope;//Defined

		//AKA VarData, this should also store the subroutine that functions as the default value of the complex.
	

		//This is something I don't know how or if I want to do yet. At least, private protected and public.
		enum struct AccessModifier
		{
			Private,	//Only a given scope can observe it
			Protected,	//Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
			Internal,	//Only the script can observe it.
			ProtectedInternal,//extends internal to derived scripts. In a class functions much like internal.
			Public


			//Protected		Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
			//				script block usage.
		};


		enum struct InfoType
		{
			Variable,
			Parameter,
			Field,
		};

		struct VarInfo
		{

			VarInfo() = default;
		protected:
			VarInfo(InfoType i) : info{ i }{}

		public:
			//Will allow conversion without a vtable, for what little worth that is.
			const InfoType info = InfoType::Variable;

			//This version is never used I don't think
			std::string name;
			TypePolicy* type;//I would actually like to use TypeID or something.
			//Mainly used for members and function variables. But since I've yet to see anything that remotely 
			// outside these 2 uses, I think this suits just fine.
			uint32_t index = max_value<uint32_t>;

		};
	
		struct ParamInfo : public VarInfo
		{
			ParamInfo() : VarInfo{ InfoType::Parameter } {}

			//Similar to VarInfo, but has areas for qualifers specific to parameters.

			bool hasDefault = false;

		};

		struct FieldInfo : public VarInfo
		{
			FieldInfo() : VarInfo{ InfoType::Field } {}

			//like param info but holds a place where a subroutine can go.
			bool isStatic = false;//Not exploring right now.
			AccessModifier access = AccessModifier::Public;


		};

		//AKA, prototype which didn't actually fit the proper functionality.
		class TypeInfo;

		//class TypePolicy;
		//using TypePolicy = Hierarchy;
		class Project;//Defined

		class MaybeScript;//Defined




		struct TypePolicy
		{
			//Refer to hierarchy for documentation.

			//This object is VERY simple, as there's really not much of a reason to go crazy with how it works.
			TypePolicy* _extends;
			uint32_t _length;//It shouldn't ever get that fucking long anyhow, and if I do this I can fit the object type in there.
			ObjectID id;


			TypeInfo* AsPrototype()
			{
				//Use the struct index btw.
				if (id.value[0] == 0xFF) {
					//I think this should work because it's the main inheritence?
					return reinterpret_cast<TypeInfo*>(this);
				}

				return nullptr;
			}


			bool DerivedFrom(const TypePolicy* from) const
			{
				if (!_length || from->_length >= _length)
					return false;

				//Here we check if one inherits the other by linear checking. We check the longer one because there's no way the shorter hierarchy derives
				// from one that is longer than itself.


				//STOP, this actually is incorrect, if focus is this, and we're trying to see if to derives from it,
				// (So focus is a ObjectReference, but To is Actor) it will be true that it derives from it, but not that 
				// it can turn focus into the To class.
				const TypePolicy* focus = this;

				while (focus = focus->_extends)
				{
					if (focus == from)
						return true;
				}

				return false;
			}

			bool IsRelated(const TypePolicy* other) const
			{

				//If the length of one is 0, this is the core object of the hierarchy. If the other isn't too, this thing is invalid.
				// Additionally, if both have the same length, neither could possibly inherit from the other.
				if (!_length || other->_length == _length)
					return false;

				//Here we check if one inherits the other by linear checking. We check the longer one because there's no way the shorter hierarchy derives
				// from one that is longer than itself.


				//STOP, this actually is incorrect, if focus is this, and we're trying to see if to derives from it,
				// (So focus is a ObjectReference, but To is Actor) it will be true that it derives from it, but not that 
				// it can turn focus into the To class.
				return other->_length > _length ? DerivedFrom(other) : other->DerivedFrom(this);
			}

			bool CanConvertTo(TypePolicy* to)
			{
				//I think I want this to fail so if need be I can flush it out.
				//if (!to)
				//	return false;

				//This is kept in some kind of map in association with the type map.
				// How we find out if one is like the other, we first get the codes. Do their index bytes match? 
				// If not they are incompatible types. (I can use this concept for literals by having it be the byte under it that does this job).
				// Before we check for failure, an immediate pass is if the GoingTo Hierachy is the base (so XX000000). Should make an easy function to do that.
				// Then we check their lengths. If they are both the same then there's no way one could inherit from the other.
				// If it does, we check if any of the extends of the longer one are equal to the hierarchy here.
				//And that's how conversion is measured.

				//Note, the last part MAY not exactly be accurate.

				if (id.value[0] != to->id.value[0])
					return false;

				uint32_t to_val = to->id;

				//If it's expecting the base type with no other strings attached, so since they're in
				// the same category, free pass.
				if (to->id.GetSpecifier() == 0)
					return true;

				//Break the above into it's own sort of function

				return to->DerivedFrom(this);




			}
		};


		/*
		//Unordered if I can. Gonna re order and tend to later.
		//std::map<ObjectID, Hierarchy*> TEMP(hierarchyMap);

		std::map<std::string, ObjectID> typeNameMap;

		bool operator >> (ObjectID a_lhs, ObjectID a_rhs)
		{
			return false;

			if (a_lhs.IsArray() != a_rhs.IsArray())
				return false;

			//Hierarchy* l_type = temp_hierarchyMap[a_lhs];
			//Hierarchy* r_type = temp_hierarchyMap[a_rhs];

			//return l_type->CanConvertTo(r_type);
		}
		//*/

		//class HierarchyManager{};




		Project* shared = nullptr;

		struct EnvironComponent : public Component
		{
			//The idea is this is a base class for global variables and such to share. It's also where I'd put access modifier stuff too.
		};

		struct Environment : public Component
		{
			//Might have taken names here, along with component type or something
			// or maybe make it a part of "Most" environments, but all

			//GetCommons-virtual. Implemented differently, by default getting the commons will be getting the project and getting commons from it. GettingCommons
			// otherwise is pulling a specific script from project, the "commons". Commons should still be found via string if needed.
			//GetShared-Static, the project called shared


			//TODO: For all of these, there should be an "errorless" version, that just wraps the function and returns some result enum or something.

			//Querying the lot of these without them being the type that actually expects it should cause a compiling error.

			//returns true if it's project is a shared project.
			bool IsShared()
			{
				return GetProject() == GetShared();
			}

			static Project* GetShared()
			{
				return shared;
			}
			virtual Script* GetCommons()
			{
				//This 
				return nullptr;//GetScript()->GetCommons();
			}

			virtual Project* GetProject()
			{
				//If it is a project, it will return project.
				throw EnvironmentError("No implementation 'GetMember'.");
				return nullptr;
			}

			virtual Script* GetScript()
			{
				//Gets parent script rather than parent environment. If it is a script, it will return script.
				throw EnvironmentError("No implementation 'GetMember'.");
				return nullptr;
			}

			//TODO:Add Get AccessModifier function. Operational by default, set to public. Likely, should derive from some pivot that can be shared with global and other components.

			virtual Script* FindScript(std::string name)
			{
				//Finds a script, only used within project as of now
				throw EnvironmentError("No implementation 'GetMember'.");
				return nullptr;
			}

		

			//Considering implementing memory in the environment, but unsure if that will be very useful. Noting for now.
			//virtual void AddMemory(Record& rember)
			//{
			//	throw EnvironmentError("No implementation 'GetMember'");  
			//}

			virtual Environment* GetParent()
			{
				throw EnvironmentError("No implementation 'GetMember'");
				return nullptr;
			}
		

			virtual TypeInfo* FindTypeInfo(std::string name)
			{
				throw EnvironmentError("No implementation 'GetMember'");
			}

		
			virtual Function* FindFunction(std::string name, TypePolicy* policy)
			{
				//Needs a way to differentiate between these

				// _GetInternalFunction
				// _GetExtneralFunction?
				// _GetStaticFunction
				// _GetExtensionFunction

				throw EnvironmentError("No implementation 'GetMember'");
			}

		
			virtual Variable_* FindVariable(std::string name)
			{
				//Finding a variable might take a runtime, this way, I can use the actual function runtime
				throw EnvironmentError("No implementation 'GetMember'");
			}
		


			virtual VarInfo* FindVariableInfo(std::string name)
			{
				throw EnvironmentError("No implementation 'FindVariableInfo'.");
			}
			//Find variable is about variables already defined, varinfo is about data used to create new variables during a runtime.


			virtual Record* GetSyntaxTree()
			{
				throw EnvironmentError("No implementation of Syntax Tree.");
			}

			//Will likely remain unused, the ComponentType might work more suitably.
			//template <class Self>
			//constexpr std::string_view GetName(this Self&& self) {
			//	return typeid(Self).name();
			//	//return TypeName<Self>::value;
			//}

			virtual std::string GetName() = 0;

			//static
			//The functions that will need to search will be Function
			//To the search functions, search internal, and search external are 2 things I'll make, but not virtual.

		
			//This doesn't need to exist, just is a pivot between get project and get shared.
			//Environment* _SearchProject(std::string name);
		
			//This is used for being able to search scripts
			virtual Environment* _SearchScript(bool is_shared, std::string name)
			{
				//While you can search other projects too, the only viable excepted use will 
				return nullptr;
			}

			Environment* SearchEnvironment(Record& ast)
			{
				//BREAK THIS DOWN INTO PIECES. GetShared, GetScript, GetNext thing.

				//Can only percieve Keyword{shared}, script name.
				//The picked environment shouldn't need to be only one type, the type doesn't matter.
				// it's basically a scopedenvironment or a public environment. {Scripts, Projects (maybe, via keyword only, and only shared), classes, structs}
				// when it detects a scope that isn't a script, the loop should expect that to be the end.
				// to do this, I could search for an evironment type like the above, and continuously  looping through each type, catching environment errors. until I get my answer
		
				//What this doesn't do is get functions, at least not exactly.

				return nullptr;
			}

			//These 2 will exist as sort of wrapper functions get function and co, basically to help print errors and such. Find exists when such errors are not required.
			//SearchFunction
			//SearchVariable

			//Search field isn't going to work with this, too many things it can't work with, so I need to be specific.
			// Actually, in though, all of the legwork is done through the actual functions, so I'mma prolly just use that instead.
			Environment* SearchField(std::string name, Impl::ExpressionType expr, Environment* spec_env = nullptr, TypePolicy* policy = nullptr)
			{
				//mem_env, or member environment isn't quite correct. What it's supposed to carry is the 

				using ExpressionType = Impl::ExpressionType;


				Environment* search = spec_env ? spec_env : this;


				//If speci

				//What is it this should do? It should search an environment, either it's own, or a specified environment. In this, the member environment isn't entirely correct.
				// What it should be is something like a something that contains hierarchy information, however that isn't So I think there should be a function to transfer environments
				// into a a hierarchy.


				//For using find function, It should take a type policy. See, this allows us to specify the difference


				//perhaps this should take expression instead of expression type.
				//This could and should probably use 
				switch (expr)
				{
				case ExpressionType::Function:
					//return search->FindFunction(name, policy);
				case ExpressionType::VarDeclare:
					return nullptr;//search->FindVariable(name);

				default:
					throw EnvironmentError("Expression type not valid.");
				}

			
				return nullptr;
			}

			//Environment* GetEnvironFromName(std::string name, Record* member, Record* )

			//Setters


		

			//Also, maybe aliases? Kinda fluff stuff ngl.
		protected:
			virtual void SetName(std::string name)
			{
				throw EnvironmentError("No implementation 'SetName'");
			}

			virtual void SetParent(Environment*)
			{
				throw EnvironmentError("No implementation 'SetParent'");
			}

			//Likely done some other way, likely set parent.
			virtual void SetProject(Project*)
			{
				throw EnvironmentError("No implementation 'SetProject'.");
			}
			virtual void SetScript(Script*)
			{
				throw EnvironmentError("No implementation 'SetScript'.");
			}
		public:

		
			virtual void AddScript(Script*)
			{
				//If it is a script, it will return script.
				throw EnvironmentError("No implementation 'AddScript'.");
			}


		


			virtual void AddTypeInfo(TypeInfo*)
			{
				throw EnvironmentError("No implementation 'GetMember'");
			}


			virtual void AddFunction(Function*)
			{
				//Needs a way to differentiate between these

				// _GetInternalFunction
				// _GetExtneralFunction?
				// _GetStaticFunction
				// _GetExtensionFunction

				throw EnvironmentError("No implementation 'AddFunction'");
			}


			virtual void AddVariable(Variable_*)
			{
				throw EnvironmentError("No implementation 'AddVariable'");
			}

			virtual void AddVariableInfo(VarInfo)
			{
				throw EnvironmentError("No implementation 'AddVariableInfo'.");
			}

			virtual void SetSyntaxTree(Record& ast)
			{
				throw EnvironmentError("No implementation of 'SetSyntaxTree'.");
			}
		};

		/*


		//New component types are
		// Project
		// Script

		struct SyntaxEnvironment : public Environment
		{
			Record* _syntaxNode;

			virtual void SetSyntaxTree(Record& ast)
			{
				throw EnvironmentError("No implementation of 'SetSyntaxTree'.");
			}
		};


		struct UnnamedEnvironment : public Environment
		{
			Record* _syntaxNode;

			std::string GetName() override
			{
				//Should this throw an exception isntead?
				return "";
			}
		};

		//Very temp name and temp idea.
		struct HierEnvironment : public Environment, public Hierarchy
		{
			//The idea of this is to be a pivot for environments that are also hierarchies, structs, classes, and enums (even if enums can't derive from anything)
			Record* _syntaxNode;

			Hierarchy* AsHierarchy()//override
			{
				return this;
			}

			std::string GetName() override
			{
				//Should this throw an exception isntead?
				return "";
			}
		};



		struct Function : public Environment
		{
			//note, dis broke
		};



		//derives from type policy
		struct TypeInfo : public Hierarchy, public Environment
		{
			//As a hierarchy object, this should only have hierarch to the core object it derives from

			//So prototype will work with heirachy in order to establish its parent. This is helpful because if I do make them able to extend 
			//objects. So another aspect to wonder is, how will I be able to tell which it is? I'll use object type.
			LEX::Script* home = nullptr;//This equals is grandparent environment, the script it came from.

			//While I could cover this with the type policy, I think it's better to have it for easy transfer to it's object form
			Prototype* _extends = nullptr;

			//TODO:Prototype needs the ability to restrict any variable that has it's own type from existing within itself.
			// It's important to note, structs are a lot less like objects, and more like value types, even if nothing is used, it will instantiate a value.
			// To get around this, you can use the construct type in order to get away with it


			//The concept of the prototype would be the struct prototype, it in of itself is an environment, it owns functions, and makes ones of its own first.
			// Then it will import functions from it's parent environments, and if it finds one with the same name, it will not import it (basically, a virtual function).

			//So, the when a function is called from a struct, first it checks the prototype, then the scope it's from. Then it checks the scopes its included.
			// Then it checks the scopes it's imported.

			struct VarData
			{
				std::string name;
				LEX::ObjectID varType;
			};

			//NOTE: Functions should probably have a hash I can use for their names, that can be referenced when doing a dynamic call, this way I don't have to store
			// the actual name of the function in the instruction


			//If I could have this fulfill it's name and sorta have


			//This is what is used when searching 
			std::vector<VarData> variableData;

			//This is important to use to include and know what offset it's variables are from where you should be looking.
			// Example @ $VarIndex
			uint32_t _varIndex;//I'll never need THAT much, and I can use the rest of this value for base id.

			ObjectID baseID;//Used so I can tell if it derives from a real object better. NOTE, hierarchy covers that


			Coroutine* GetConstructor()
			{
				//return functions.back();
			}

			Coroutine* GetMethod(std::string name)
			{
				//Get method should go the a similar song and dance that Script does looking for extension functions.
				return nullptr;
			}


			size_t GetMember(std::string name)
			{
				//The idea is it should give you the prototype of the variable data
				// this instead will simply give you the index of said place.
				// If invalid it will be equal to npos.


				return full_value<size_t>;
			}

			Prototype* GetExtends()
			{
				return nullptr;
			}

			bool ExtendsObject()
			{
				if (Hierarchy::_extends)
					return true;
				//TODO:0xFF used for struct, please cahnge
				return baseID.value[0] != 0xFF && baseID.value[0] != 0x0;
			}



			//$VarIndex: Say I have 2 structs. A comes first, and has 4 variables. Then struct B extends it, and has 5 variables.
			// Someone is interacting with struct B, and is trying to find ITS 4th variable. It finds it, but in the actual struct,
			// its index is 8, because the first 4 variables belong to A. So instead what it will do once it finds the forth, it adds the varIndex, correcting
			// it for the structs it extends.

		};



	
		struct Project : public Environment
		{
			struct CrudeAddon
			{
				//The unrefined manual addons to tack to a script once it's finished. Once a script is made, the primary won't be needed, but this crude addon will be.
				// If the script already exists within it, it will just place that there instead.
		

				//The records produced by saying it wants to be compiled in occordance with a script. These will be handed over to the 
				std::vector<Record> auxilary;
			};

			struct CrudeScript : public CrudeAddon
			{
				//Uncompiled, uninitialized scripts. Records haven't been given to the actual script yet.

				//The actual record produced from the script
				Record primary;

			};

			struct AddonStack
			{
				std::list<CrudeAddon> queue;
			};
			struct ScriptStack
			{
				std::list<CrudeScript> queue;
			};

			ScriptStack* stack = nullptr;

			//Project Environment. Keeps all scripts and is the scope for a given set of scripts. Interaction outside of is prohibited unless in shared project.

			Script* _commons = nullptr;

			std::string _name;

			std::vector<Script*> _scripts;

			std::string GetName() override
			{
				return _name;
			}
			void SetName(std::string name) override
			{
				_name = name;
			}


			void AddScript(Script* script)
			{
				//Should search for script, throwing if within, then call SetParent on the script.

				auto end = _scripts.end();

				//TODO: Report name when Project::AddScript is used, seems pretty important.
				if (auto it = std::find(_scripts.begin(), end, script); it == end)
					throw EnvironmentError("Script already added.");

				//Call add parent function

				_scripts.push_back(script);
			}



			Project* GetProject() override
			{
				return this;
			}


			Script* GetCommons() override
			{
				return _commons;
			}


		
			Script* FindScript(std::string name) override
			{
				auto end = _scripts.end();
			
				//Proper version of Script not implement
				auto it = end;//std::find_if(_scripts.begin(), end, [&](Script* search) { return search->GetName() == name; });
			
				if (it != end) {
					return *it;
				}
			
				return nullptr;
			}
		};



		struct MaybeScript : public Environment
		{
			//Notes if it was specified like Shared::GeneralUtil::GetObjectByString(), it would be putting it into shared.
			bool _isShared = false;

			std::string _name;
			Script* _parent = nullptr;
			Script* _mirror = nullptr;

		

			//So notably for this, functions and extend methods exist obvs, but I think having globals is actually a really interesting thing I can do.
			// the main reason is you can have this as an interface between 2 different scripts to tell if it's installed by having a maybe global set up.
			// maybe globals should come in pairs.

			//Also, maybe some core pivot with script, since they share so much
			std::vector<Function*> _functions;
			std::vector<Variable_*> _globals;
			std::map<TypePolicy*, std::vector<Function*>> _methods;

			//When using, it's very important to treat this like an imported script.


			//Just about all functions are mirrored, but the only things that aren't really mirrored are are things like get script and get parent
			// 
			Script* GetScript() override
			{
				return _parent;
			}



		};
	

		struct Function : public Environment
		{

		};
	
		struct Scope : public Environment
		{
			//A helper class that *might* be used by functions (but not subroutines) to organize what variables can and cannot be addressed during compile time.
			// Issue is, it's going to actually be a temporary object. And when I say temporary, I mean VERY temporary, kinda like how process chains work.
			//The representation of scope will then probably not be very relevant outside of compiling.

			//A nameless temporary environment used for the orchestration of function variables. Likely to be default created


			Function* _parent = nullptr;

		};

		//No global is ever an environment, not doing that.
		//struct GlobalVariable_ : public Environment
		//{
		//
		//};

		struct Script : public Environment
		{
			//Scripts have functions/globals(vars)/types/(parents/projects)
		private:
			enum struct ScriptAffinity
			{
				None, 
				Required,
				Imported,
				Included,
			};
		
		
			Project* _parent = nullptr;


			bool _defined = false;

			Record _syntaxTree;

			//The idea is that I use this as a shorthand for functions variables etc within a scope to search for. Like, checking if a name exists already or not. And if not,
			// it will make one.
			//std::unordered_map<std::string, Component*> TEMP(nameStorage);

			//Unsure if I should seperate by function type or not.
			std::vector<Function*> _functions;
			std::vector<Variable_*> _globals;
			std::vector<TypeInfo*> _types;

			std::map<TypePolicy*, std::vector<Function*>> _methods;

			//This is where scripts are refered
			std::unordered_map<Script*, ScriptAffinity> _affinityMap;
		public:

			Script* GetCommons() override
			{
				//This 
				return _parent->GetCommons();
			}


			Script* GetScript() override
			{
				return this;
			}


			virtual Record* GetSyntaxTree()
			{
				if (!_defined)
					throw EnvironmentError("Syntax Tree not defined.");

				return &_syntaxTree;
			}

			void SetSyntaxTree(Record& ast)
			{
				if (_defined)
					throw EnvironmentError("No Syntax Tree already.");

				//Should chec
				_defined = true;
				_syntaxTree = ast;
			}

			std::string GetName() override
			{
				if (!_defined)
					throw EnvironmentError("Syntax Tree not defined, script is nameless.");

				return _syntaxTree.GetTag();
			}
			//SetName will resume having no use here.



			void AddVariable(Variable_* var)
			{
				//Should search for script, throwing if within, then call SetParent on the script.

				auto end = _globals.end();

				//TODO: Report name when Project::AddScript is used, seems pretty important.
				if (auto it = std::find(_globals.begin(), end, var); it == end)
					throw EnvironmentError("Variable already added.");

				//Call add parent function

				_globals.push_back(var);
			}

			Variable_* FindVariable(std::string name) override
			{
				//if the last character of the variable to 

				auto end = _globals.end();

				//Proper version of Script not implement
				auto it = end;//std::find_if(_scripts.begin(), end, [&](Script* search) { return search->GetName() == name; });

				if (it != end) {
					return *it;
				}

				return nullptr;
			}

			//Includes/Imports/Requires need to be included as concepts.
			// Script is the only thing that uses these, so no reason to branch out.
			// Also, I may not need to "Find" them for that reason.
			// Instead, asking if something is one of these things might be better.
			Script* FindRequired(bool shared, std::string name)
			{
				return {};
			}

			Script* FindImported(bool shared, std::string name)
			{
				return {};
			}

			Script* FindIncluded(bool shared, std::string name)
			{
				return {};
			}

			void AddRequired(Script*)
			{
			
			}

			void AddImported(Script*)
			{
			
			}

			void AddIncluded(Script*)
			{
			
			}


			//Going to remain unimplemented for a while.
			virtual void AddFunction(Function*)
			{
				//Needs a way to differentiate between these

				// _GetInternalFunction
				// _GetExtneralFunction?
				// _GetStaticFunction
				// _GetExtensionFunction

				throw EnvironmentError("No implementation 'AddFunction'");
			}

			Function* FindFunction(std::string name, TypePolicy* policy) override
			{
				if (policy)
				{
					//Search member functions
				}
				else
				{
					//Search static functions.
				}
				//Should this throw?
				return nullptr;
			
				//Needs a way to differentiate between these

				// _GetInternalFunction
				// _GetExtneralFunction?
				// _GetStaticFunction
				// _GetExtensionFunction

				throw EnvironmentError("No implementation 'GetMember'");
			}
		

			void AddTypeInfo(TypeInfo* type)
			{
				//Should search for script, throwing if within, then call SetParent on the script.

				auto end = _types.end();

				//TODO: Report name when Project::AddScript is used, seems pretty important.
				if (auto it = std::find(_types.begin(), end, type); it == end)
					throw EnvironmentError("Variable already added.");

				//Call add parent function

				_types.push_back(type);
			}

			TypeInfo* FindTypeInfo(std::string name) override
			{
				auto end = _types.end();

				//Proper version of Script not implement
				auto it = end;//std::find_if(_types.begin(), end, [&](TypeInfo* search) { return search->GetName() == name; });

				if (it != end) {
					return *it;
				}

				return nullptr;
			}

			void SetParent(Environment* env) override
			{
				//TODO: Script::SetParent is actually supposed to ask if new parent isn't a project. Attend to that when enum is added.
				//TODO: EnvironmentError in this situation is exclusively an error on my part, need a new exception for that.
				if (env->GetComponentType() != ComponentType::Invalid)
					throw EnvironmentError("Parent of script must be a project.");

				_parent = env->GetProject();
			}
		


		
			Environment* _SearchScript(bool is_shared, std::string name) override
			{
				//What this will do is search for the named script, but instead it will search through the maybe scripts first, 
				// then it will get the actual scripts.
				return nullptr;
			}


		};


		struct InvalidScript
		{
			//A concept for searching an environment when it's no longer flagged as valid. It's basically created when something goes critical, and it can no longer be properly
			// referenced. Though, this is for top level stuff like scripts rather than for things like functions and such.

			//This can mainly exist as something that's created to communicate aptly and naturally when it comes around to it the failures of such a script.
			// such scripts are caried on the actual script, but should be submitted in the places scripts can, the same as maybescripts.

			//For this reason, it's highly likely I'll make a joint abstraction for them all to share, so GetScript can return this error object.


			//Also, unlike maybe this will likely mirror Script a lot closer, though mainly should have all of it's functions send descriptive exceptions about why this is an
			// invalid script.


			//REVISION
			//Actual, scratch that. This should be somewhere on the environment, but I think instead I'll accumulate it within the environment some how. For now, I won't worry about it,
			// but in concept the idea should just be that it's a temporary store of data that when something is being gotten and it's wondering why it's not there (because if it has an
			// error I'll be deleting it) it will print an error message, possibly with the invalidity object attached, so a new invalidity message referencing this invalidity message can
			// be used.fs
		};

		//*/
	}
}
