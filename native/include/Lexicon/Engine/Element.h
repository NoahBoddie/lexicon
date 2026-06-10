#pragma once

#include "Lexicon/Engine/Component.h"
#include "RGL/Impl/Record.h"
#include "Lexicon/Exception.h"//May move to src

#include "Lexicon/Interfaces/IElementBase.h"


#include "Lexicon/TypeAliases.h"

#include "Lexicon/Engine/SyntaxRecord.h"


//*src
#include "Lexicon/Interfaces/IProject.h"
#include "Lexicon/Interfaces/IScript.h"
//#include "Lexicon/Interfaces/IEnvironment.h"
#include "Lexicon/Engine/Overload.h"
#include "Lexicon/Engine/OverloadKey.h"
#include "Lexicon/Engine/QualifiedField.h"
#include "Lexicon/Interfaces/ProjectManager.h"
#include "Lexicon/RelateType.h"
#include "Lexicon/Engine/FunctionNode.h"


namespace LEX
{
	struct Environment;
	class Project;
	class Script;
	class CommonScript;
	
	struct FunctionInfo;

	struct GenericBase;

	struct ITemplateInserter;

	struct QualifiedName;
	struct QualifiedElement;

	struct TypeNode;

	struct Directory;
	
	struct Element : public Component, public IElementBase
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::Element)

		using IElementBase::GetElementFromPath;

		using ElementSearch = bool(std::vector<QualifiedElement>&);

		using EnvironSearch = bool(std::vector<QualifiedName>&);

		using SearchFunction = std::variant<std::function<EnvironSearch>, std::function<ElementSearch>>;


#define ELEM_ENUM using Prev = Flag; enum Flag; Flag& GetFlags() const { return GetComponentData<Flag>(); } enum Flag : std::underlying_type_t<Prev>
#define ELEM_FLAG(mc_name, mc_index) mc_name = 1 << (Prev::_next + mc_index)
#define ELEM_NEXT  _last, _next = std::bit_width<uint32_t>(_last), None = 0
		//#define ELEM_FLAG(mc_name, mc_expr) mc_name = static_cast<std::underlying_type_t<Prev>>(mc_expr) << Prev::_next


		enum Flag 
		{
			None		= 0 << 0,
			Attached	= 1 << 0,  //When parent is set. If not attached, will not be set.


			_last, 
			_next = std::bit_width<uint32_t>(_last),

		};


	
		//TODO: I'd like to devise a situation where Element wasn't derived from IElement, just so the inheritance wouldn't have
		// to be virtual


	public:
		//This should take over
		virtual std::string_view GetName() const = 0;

		virtual SyntaxRecord* GetSyntaxTree() = 0;

		virtual GenericBase* AsGenericElement() { return nullptr; }

		bool IsGenericElement() const override final { return unconst(this)->AsGenericElement(); }


	private:
		Element* GetElementFromPathImpl(std::string_view path, ElementType elem, OverloadArgument* sign = nullptr) override
		{
			return GetElementFromPath(this, path, elem, sign);
		}

		Script* GetScriptImpl() override;

		Project* GetProjectImpl() override;

		Script* GetCommonsImpl() override;
		
		Directory* GetDirectoryImpl() override
		{
			return GetParent();
		}
		
		Repository* GetRepositoryImpl() override;

	public:



		static Element* GetElementFromPath(Element* a_this, std::string_view path, ElementType elem, OverloadArgument* sign = nullptr);


		
	public:

#pragma region NewSearch

		//THis can be destroyed
		virtual Environment* FindEnvironment(SyntaxRecord& record, ITemplateInserter& inserter) { return nullptr; }


		//the associated should maybe be a bool or just reject any other than include and import.








		static TypeNode SearchTypePath(Element* a_this, SyntaxRecord& _path);



		TypeNode SearchTypePath(SyntaxRecord& _path);




		//TODO: Make this take pointers to overload stuff. The idea being if no overload is provided it fails when trying 
		// to handle multiple different functions.
		static FunctionNode SearchFunctionPath(Element* a_this, SyntaxRecord& path, OverloadArgument& key, Overload& out);

		static FunctionNode SearchFunctionPath(Element* a_this, SyntaxRecord& path, OverloadArgument& key)
		{
			Overload out{};
			auto result = SearchFunctionPath(a_this, path, key, out);
			return result;
		}

		FunctionNode SearchFunctionPath(SyntaxRecord& path, OverloadArgument& key, Overload& out) { return SearchFunctionPath(this, path, key, out); }

		FunctionNode SearchFunctionPath(SyntaxRecord& path, OverloadArgument& key) { return SearchFunctionPath(this, path, key); }




		static QualifiedField SearchFieldPath(Element* a_this, SyntaxRecord& path);

		QualifiedField SearchFieldPath(SyntaxRecord& path)
		{
			return SearchFieldPath(this, path);
		}


		static Script* SearchScriptPath(Element* a_this, SyntaxRecord& path);


		Script* SearchScriptPath(SyntaxRecord& path)
		{
			return SearchScriptPath(this, path);
		}

#pragma endregion





		//This will generally get parent script rather than parent environment. If it is a script, it will return script.
		

		//If it is a project, it will return project.
		

		//Basically, like get class, but can be suitable for both scripts and classes and such, and is better
		// suited to use SearchField/SearchEnvironment on than just script.
		//Make pure to search for non implementers.
		




		//Querying the lot of these without them being the type that actually expects it should cause a compiling error.

		//returns true if it's project is a shared project.
		bool IsShared()
		{
			return GetProject() == GetShared();
		}

		static Project* GetShared();

		virtual void SetSyntaxTree(SyntaxRecord&) = 0;//This is to be made on the abstract classes.

		virtual void LoadFromRecord(SyntaxRecord& rec)
		{
			//should likely be a pure virtual, but holding off.
		}

		virtual void OnAttach()
		{
			//This is specifically when parentage is established.
		}

		void OnInit(SyntaxRecord& rec) final override
		{
			SetSyntaxTree(rec);
			//We get the node like this so the target is a viable syntax tree. Other wise, it may use a tree that doesn't exist anymore.
			auto& node = *GetSyntaxTree();
			
			LoadFromRecord(node);
			node.SetParent(this);//Doing this last allows the base most ones to set their parent first.
		}

		//virtual bool CanStack(Element* other)
		//{
		//	return false;
		//}

		bool ShouldLink(LinkFlag flag) override;


		bool IsAttached()
		{
			return GetFlags() & Flag::Attached;
		}

		void DeclareParentTo(Element* child);

		void DeclareOrphan()
		{
			//This simply removes the attach check by confirming it's already attached.
			if (IsAttached() == false)
			{
				GetFlags() |= Flag::Attached;
				OnAttach();
			}

		}


	protected:


		bool HasAttributes() override final
		{
			if (SyntaxRecord* syntax = GetSyntaxTree()) {
				return syntax->FindChild(parse_strings::attributes);
			}

			return false;
		}
		
		virtual void SetParent(Directory*) = 0;

		Flag& GetFlags() const
		{
			return GetComponentData<Flag>();
		}
		

	public:
	};


	class SecondaryElement : public Element
	{
		//Function and Global
		//std::string name;//may include name later, depending.
		
	public:
		SyntaxRecord* GetSyntaxTree() override
		{
			return _syntax;
		}

		void SetSyntaxTree(SyntaxRecord& rec) final override
		{
			if (!_syntax)
				_syntax = &rec.Transform<SyntaxRecord>();
		}

		

	protected:

		void SetParent(Directory* par) override;


	private:

		Environment* GetEnvironmentImpl() override;
		Directory* GetParentImpl() override;


	private:
		SyntaxRecord* _syntax = nullptr;
		Environment* _parent = nullptr;
	};
}