#pragma once

#include "Lexicon/Interfaces/IRepositoryBase.h"

#include "Lexicon/Engine/Directory.h"

namespace LEX
{
	struct ISubdirectory;

	struct Repository : public Directory, public IRepositoryBase
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::Repository)
	public:

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

	protected:

		bool CreateSyntaxTree(std::string_view file, std::string_view extension, std::string_view path,
			std::vector<std::string_view>& options, SyntaxRecord& ast);

		bool CreateSyntaxTreeFromString(std::string_view file, std::string_view contents,
			std::vector<std::string_view>& options, SyntaxRecord& ast);




		Script* FindScriptImpl(const std::string_view& name) override;
		Subdirectory* FindSubdirectoryImpl(const std::string_view& name) override { return nullptr; }
		Subdirectory* CreateSubdirectoryImpl(const std::string_view& name, Script* sub_to, std::span<std::string_view> options, const std::string_view& path) override { return nullptr; }

		Script* CreateScriptImpl(const std::string_view& name, std::span<std::string_view> options, const std::string_view& path, std::optional<std::string_view> content) override;


		Script* MakeScript(const std::string_view& name, bool is_commons, std::span<std::string_view> options, std::string_view path, std::optional<std::string_view> content);



		virtual void AddScript(Script* script);

		std::vector<Subdirectory*> _subdirectories;
		std::vector<Script*> _scripts;
		std::string _filePath;
	};
}