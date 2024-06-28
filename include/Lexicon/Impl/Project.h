#pragma once

#include "RGL/Impl/Record.h"
#include "Exception.h"//May move to src

#include "Lexicon/Component.h"
#include "Element.h"

namespace LEX
{
	struct Script;

	struct Format
	{
		std::string formatName;
		std::string formatContent;
		LEX::Script* formatScript = nullptr;
	};



	struct Project : public Element
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

		friend class SharedProject;

		//Instead of doing this shit, just tack it onto an existing script's AST or something.
		//ScriptStack* stack = nullptr;

		//Project Environment. Keeps all scripts and is the scope for a given set of scripts. Interaction outside of is prohibited unless in shared project.

		CommonScript* _commons = nullptr;

		std::string _name;

		std::vector<Script*> _scripts;
		std::vector<Format> formatList;


		
		//Linkage status, needs to link later depending on the performance of its commons

		//Also, a project COULD be responsible for making it's commons. I sorta like that idea personally.
		// But probably JUST it's commons.




		std::string GetName() override
		{
			return _name;
		}

		void SetName(std::string name)
		{
			_name = name;
		}
		
		void AddScript(Script* script);
		//*/


		Project* GetProject() override
		{
			return this;
		}
		
		Element* GetParent() override
		{
			auto share = GetShared();
			//Should return null on shared projects.
			return share == this ? nullptr : share;
		}

		

		Script* GetCommons() override;



		Environment* GetEnvironment() override
		{
			//It has no environment.
			return nullptr;
		}


		void SetParent(Element*) override
		{
			//nothing. maybe an error?
		}


		Record* GetSyntaxTree() override
		{
			//It has no syntax tree.
			return nullptr;
		}

		void SetSyntaxTree(Record& rec) final override {}

		ComponentType GetComponentType() override
		{
			return typeid(Project);
		}

		virtual void AddFormat(std::string& name, std::string& content, Script* source)
		{
			Format format;
			format.formatName = name;
			format.formatContent= content;
			format.formatScript = source;

			formatList.push_back(format);

			report::debug("adding format {}", name);
		}

		Script* FindScript(std::string name);
	};


	namespace ExperimentZone
	{

		struct SharedProject : public Project
		{


			//void AddFormat(std::string& name, std::string& content, Script* source) override
			//{
			//	//report::error("Shared project cannot use any format");
			//}

			//Projects might be handled else where actually
			
		};
	}

}