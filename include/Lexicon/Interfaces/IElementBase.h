#pragma once

#include "Lexicon/ComponentType.h"
#include "Lexicon/Interfaces/Interface.h"

#include "Lexicon/ElementType.h"

#include "Lexicon/Impl/ComponentDetails.h"

#include "Lexicon/Interfaces/IComponentBase.h"




namespace LEX
{
    struct TypeInfo;
    struct Component;
	struct IElement;
	struct IProject;
	struct IScript;
	struct IDirectory;
	struct IRepository;
	struct IEnvironment;

	struct Element;
	class Project;
	class Script;
	struct Directory;
	struct Repository;
	struct Environment;

	struct SignatureBase;
	struct OverloadArgument;
	namespace Version
    {
        namespace _1
        {
            struct M_INTERFACE_VERSION(IElementBase)
            {
			protected:
				virtual IScript* GetScriptInfc() = 0;
				virtual IProject* GetProjectInfc() = 0;
				virtual IDirectory* GetParentInfc() = 0;
				virtual IEnvironment* GetEnvironmentInfc() = 0;
				virtual IDirectory* GetDirectoryInfc() = 0;
				virtual IRepository* GetRepositoryInfc() = 0;
				virtual IScript* GetCommonsInfc() = 0;
				virtual IElement* GetElementFromPathInfc(std::string_view path, ElementType elem, SignatureBase * sign = nullptr) = 0;

			public:
				virtual bool IsGenericElement() const = 0;
				

            };

			#define DEF_FUNC_IMPL_ELEMENT_1 \
			MAP_UD(DEF_USING_IMPL,Element,\
			IsGenericElement)
        }

        CURRENT_VERSION(IElementBase, 1);
    }


    struct IMPL_VERSION(IElementBase)
    {
	private:
#ifdef LEX_SOURCE
		using sign_t = OverloadArgument;
#else
		using sign_t = SignatureBase;
#endif


	protected:
		IScript* GetScriptInfc() override final;
		IProject* GetProjectInfc() override final;
		IDirectory* GetParentInfc() override final;
		IEnvironment* GetEnvironmentInfc() override final;
		IDirectory* GetDirectoryInfc() override final;
		IRepository* GetRepositoryInfc() override final;
		IScript* GetCommonsInfc() override final;
		IElement* GetElementFromPathInfc(std::string_view path, ElementType elem, SignatureBase* sign = nullptr) override final;



    public:

		bool IsGenericElement() const override { return false; }
		

		

		//TODO: For GetElementFromPath I'd like to have a version that can do the casting without knowing the true type

		/*
		template<typename T>
		const T* GetElementFromPath2(std::string_view path, ElementType elem, SignatureBase* sign = nullptr) requires(requires() { { T::COMPONENT_TYPE } -> std::convertible_to<ComponentType>; })
		{
			static_assert(sizeof(T) > 0, "T is an incomplete type, requires definition to be used.");
			static_assert(sizeof(IElement) > 0, "IElement is an incomplete type, requires definition to be used.");
			IElement* result = GetElementFromPath(path, elem, sign);
			//It's upto
			return result->As<T>();
		}
		//*/

		auto GetScript()
		{
#ifdef LEX_SOURCE
			return GetScriptImpl();
#else
			return GetScriptInfc();
#endif
		}
		auto GetProject()
		{
#ifdef LEX_SOURCE
			return GetProjectImpl();
#else
			return GetProjectInfc();
#endif
		}
		auto GetParent()
		{
#ifdef LEX_SOURCE
			return GetParentImpl();
#else
			return GetParentInfc();
#endif
		}
		auto GetEnvironment()
		{
#ifdef LEX_SOURCE
			return GetEnvironmentImpl();
#else
			return GetEnvironmentInfc();
#endif
		}
		auto GetDirectory()
		{
#ifdef LEX_SOURCE
			return GetDirectoryImpl();
#else
			return GetDirectoryInfc();
#endif
		}

		auto GetRepository()
		{
#ifdef LEX_SOURCE
			return GetRepositoryImpl();
#else
			return GetRepositoryInfc();
#endif
		}
		
		auto GetCommons()
		{
#ifdef LEX_SOURCE
			return GetCommonsImpl();
#else
			return GetCommonsInfc();
#endif
		}

		auto GetElementFromPath(const std::string_view& path, ElementType elem, sign_t* sign = nullptr)
		{
#ifdef LEX_SOURCE
			return GetElementFromPathImpl(path, elem, sign);
#else
			return GetElementFromPathInfc(path, elem, sign);
#endif
		}




	protected:

#ifdef LEX_SOURCE
		virtual Script* GetScriptImpl() = 0;
		virtual Project* GetProjectImpl() = 0;
		virtual Directory* GetParentImpl() = 0;
		virtual Environment* GetEnvironmentImpl() = 0;
		virtual Directory* GetDirectoryImpl() = 0;
		virtual Repository* GetRepositoryImpl() = 0;
		virtual Script* GetCommonsImpl() = 0;


		virtual Element* GetElementFromPathImpl(std::string_view path, ElementType elem, OverloadArgument* sign = nullptr) = 0;
		
		/*
		template<typename T>
		const T* PullElementFromPath2(std::string_view path, ElementType elem, SignatureBase* sign = nullptr) requires(requires() { { T::COMPONENT_TYPE } -> std::convertible_to<ComponentType>; })
		{
			//This can detect unuse.
			//requires() { sizeof(T) > 0; } &&

			using ResultType = Element;
			static_assert(sizeof(T) > 0, "T is an incomplete type, requires definition to be used.");
			static_assert(sizeof(ResultType) > 0, "IElement is an incomplete type, requires definition to be used.");
			Element* result = PullElementFromPath(path, elem, sign);
			return result->As<T>();
		}
		//*/
#endif

    };

	#define DEF_FUNC_IMPL_ELEMENT_MAIN \
	MAP_UD(DEF_USING_IMPL,Element,\
	GetScript,GetProject,GetParent,GetEnvironment,GetDirectory,GetRepository,GetCommons,GetElementFromPath)

#define DECL_IMPL_FUNC_ELEMENT DEF_FUNC_IMPL_ELEMENT_MAIN DEF_FUNC_IMPL_ELEMENT_1 DECL_IMPL_FUNC_COMPONENT

	
}

