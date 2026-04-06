#pragma once

#include "Lexicon/ComponentType.h"
#include "Lexicon/Interfaces/Interface.h"

#include "Lexicon/ElementType.h"

#include "Lexicon/Impl/ComponentDetails.h"

#include "Lexicon/Interfaces/IDirectoryBase.h"



namespace LEX
{
	struct IEnvironment;
	struct Environment;
	struct IScript;
	struct Script;
	struct ISubdirectory;
	struct Subdirectory;


	namespace Version
	{
		namespace _1
		{
			struct M_INTERFACE_VERSION(IRepositoryBase)
			{
			public:
				virtual std::string_view GetFilepath() const = 0;

			private:
				

				virtual IScript* FindScriptInfc(const std::string_view& name)= 0;
				virtual ISubdirectory* FindSubdirectoryInfc(const std::string_view& name) = 0;
				virtual IScript* CreateScriptInfc(const std::string_view& name, std::span<std::string_view> options, const std::string_view& path, std::optional<std::string_view> content) = 0;
				virtual ISubdirectory* CreateSubdirectoryInfc(const std::string_view& name, Script* sub_to, std::span<std::string_view> options, std::string_view path) = 0;
			};

#define DEF_FUNC_IMPL_REPOSITORY_1 \
			MAP_UD(DEF_USING_IMPL,Repository, GetFilepath)
		}

		CURRENT_VERSION(IRepositoryBase, 1);
	}


	struct IMPL_VERSION(IRepositoryBase)
	{
	private:
#ifdef LEX_SOURCE
		using script_t = Script;
#else
		using script_t = IScript;
#endif

	public:

		auto FindSubdirectory(const std::string_view& name)
		{
#ifdef LEX_SOURCE
			return FindSubdirectoryImpl(name);
#else
			return FindSubdirectoryInfc(name);
#endif
		}

		auto CreateSubdirectory(const std::string_view& name, script_t* sub_to = nullptr, std::span<std::string_view> options = {}, const std::string_view& path = "")
		{
#ifdef LEX_SOURCE
			return CreateSubdirectoryImpl(name, sub_to, options, path);
#else
			return CreateSubdirectoryInfc(name, sub_to, options, path);
#endif
		}


		auto ObtainSubdirectory(const std::string_view& name)
		{
			auto subdirectory = FindSubdirectory(name);

			if (!subdirectory)
				subdirectory = CreateSubdirectory(name, nullptr);
		}




		auto FindScript(const std::string_view& name)
		{
#ifdef LEX_SOURCE
			return FindScriptImpl(name);
#else
			return FindScriptInfc(name);
#endif
		}


		auto CreateScript(const std::string_view& name, std::span<std::string_view> options = {}, const std::string_view& path = "", std::optional<std::string_view> content = std::nullopt)
		{
#ifdef LEX_SOURCE
			return CreateScriptImpl(name, options, path, content);
#else
			return CreateScriptInfc(name, options, path, content);
#endif
		}



	protected:
#ifdef LEX_SOURCE
		virtual Script* FindScriptImpl(const std::string_view& name) = 0;
		virtual Subdirectory* FindSubdirectoryImpl(const std::string_view& name) = 0;
		virtual Script* CreateScriptImpl(const std::string_view& name, std::span<std::string_view> options, const std::string_view& path, std::optional<std::string_view> content) = 0;
		virtual Subdirectory* CreateSubdirectoryImpl(const std::string_view& name, Script* sub_to, std::span<std::string_view> options, std::string_view path) = 0;

#endif

		IScript* FindScriptInfc(const std::string_view& name) override final;
		ISubdirectory* FindSubdirectoryInfc(const std::string_view& name) override final;
		IScript* CreateScriptInfc(const std::string_view& name, std::span<std::string_view> options, const std::string_view& path, std::optional<std::string_view> content) override final;
		ISubdirectory* CreateSubdirectoryInfc(const std::string_view& name, Script* sub_to, std::span<std::string_view> options, std::string_view path) override final;


	};

#define DEF_FUNC_IMPL_REPOSITORY_MAIN \
	MAP_UD(DEF_USING_IMPL,Repository,\
	FindScript, CreateScript, CreateSubdirectory, FindSubdirectory)

#define DECL_IMPL_FUNC_REPOSITORY DEF_FUNC_IMPL_REPOSITORY_MAIN DEF_FUNC_IMPL_REPOSITORY_1  DECL_IMPL_FUNC_DIRECTORY


}
