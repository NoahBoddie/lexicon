#pragma once

#include "Lexicon/Interfaces/IRepositoryBase.h"

#include "Lexicon/Engine/Directory.h"

namespace LEX
{
	struct Director;
	struct ISubdirectory;

	struct Repository : public Directory, public IRepositoryBase
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::Repository)
	public:
		ELEM_ENUM
		{
			ELEM_FLAG(kBatchLoading, 0),
			ELEM_FLAG(kDirectoryLoaded, 1),

			ELEM_NEXT,
		};



		std::string_view GetFilepath() const override
		{
			return _filePath;
		}


	INTERNAL:

		void SetFilepath(std::string_view path)
		{
			_filePath = std::string{ path };
		}

		Script* MakeCommons(std::string_view path, std::span<std::string_view> options = {}, std::optional<std::string_view> content = std::nullopt)
		{
			return MakeScript("", true, options, path, content);
		}

		bool ShouldLink(LinkFlag flag) override
		{
			if (flag == LinkFlag::Loaded) {
				return true;
			}

			if (IsBatchLoading() == true)
				return false;

			return Directory::ShouldLink(flag);
		}

	protected:

		bool IsDirectoryLoaded() const
		{
			return GetFlags() & Flag::kDirectoryLoaded;
		}

		bool IsBatchLoading() const
		{
			return GetFlags() & Flag::kBatchLoading;
		}


		void SetBatchLoading(bool value)
		{
			if (value) {
				GetFlags() |= Flag::kBatchLoading;
			}
			else {
				GetFlags() &= ~Flag::kBatchLoading;
			}
		}



		bool CreateSyntaxTree(std::string_view file, std::string_view extension, std::string_view path,
			Director& director, SyntaxRecord& ast);

		bool CreateSyntaxTreeFromString(std::string_view file, std::string_view contents,
			Director& director, SyntaxRecord& ast);




		Script* FindScriptImpl(const std::string_view& name) override;
		
		Script* CreateScriptImpl(const std::string_view& name, std::span<std::string_view> options, const std::string_view& path, std::optional<std::string_view> content) override;


		Script* MakeScript(const std::string_view& name, bool is_commons, std::span<std::string_view> options, std::string_view path, std::optional<std::string_view> content);

		Subdirectory* FindSubdirectoryImpl(const std::string_view& name) override;

		Subdirectory* CreateSubdirectoryImpl(const std::string_view& name, Script* sub_to, std::span<std::string_view> options, std::string_view path) override;
		
		Repository* GetRepositoryImpl() override
		{
			return this;
		}


		Directory* FindDirectory(SyntaxRecord& record, ITemplateInserter*) override;

		void LoadRepository(const std::span<std::string_view>& options = {});

		virtual void AddScript(Script* script);

		std::vector<Subdirectory*> _subdirectories;
		std::vector<Script*> _scripts;
		std::string _filePath;
	};
}