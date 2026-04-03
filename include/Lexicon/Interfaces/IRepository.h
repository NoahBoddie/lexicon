#pragma once

#include "Lexicon/Interfaces/IRepositoryBase.h"
#include "Lexicon/Interfaces/IDirectory.h"

namespace LEX
{
	struct IRepository : INTERFACE_DERIVES(IRepositoryBase, IDirectory)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::IRepository, ComponentType::Repository)
	public:
		std::string_view GetFilepath() const override;
	
	private:
#ifdef LEX_SOURCE
		Script* FindScriptImpl(const std::string_view& name) override;
		Subdirectory* FindSubdirectoryImpl(const std::string_view& name) override;
		Script* CreateScriptImpl(const std::string_view& name, std::span<std::string_view> options, const std::string_view& path, std::optional<std::string_view> content) override;
		Subdirectory* CreateSubdirectoryImpl(const std::string_view& name, Script* sub_to, std::span<std::string_view> options, const std::string_view& path) override;
#endif
	};


}