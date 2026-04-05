#pragma once

#include "Lexicon/ComponentType.h"

namespace LEX
{
	struct IComponent;
	struct IDirectory;

	struct ISignature;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(DirectoryManager)
			{
				virtual IComponent* GetComponentFromPath(std::string_view path, ComponentType elem, const LEX::ISignature* sign = nullptr) = 0;

				virtual IDirectory* GetDirectoryFromFilepath(std::string_view path) = 0;
			};
		}

		CURRENT_VERSION(DirectoryManager, 1);
	}


	struct IMPL_SINGLETON(DirectoryManager)
	{
		IComponent* GetComponentFromPath(std::string_view path, ComponentType elem, const LEX::ISignature* sign = nullptr) override
		{
			return nullptr;
		}

		IDirectory* GetDirectoryFromFilepath(std::string_view path) override
		{
			return nullptr;
		}
	};
}