#pragma once

#include "Lexicon/Interfaces/IDirectory.h"

namespace LEX
{
	class Script;
	class Project;

	struct IScript;


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IProject)
			{
			private:
				virtual void AddFormatInfc(const std::string_view& name, const std::string_view& content, IScript* source) = 0;
				virtual IScript* FindScriptInfc(const std::string_view& name) = 0;

			};
		}

		CURRENT_VERSION(IProject, 1);

	}

	struct IMPL_VERSION(IProject), public IDirectory
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::IProject, ComponentType::Project);
		
		auto FindScript(const std::string_view& name)
		{
#ifdef LEX_SOURCE
			return FindScriptImpl(name);
#else
			return FindScriptInfc(name);
#endif
		}



#ifdef LEX_SOURCE
		auto AddFormat(const std::string_view& name, const std::string_view& content, Script* source)
		{
			return AddFormatImpl(name, content, source);
		}
#else
		auto AddFormat(const std::string_view& name, const std::string_view& content, IScript* source)
		{
			return AddFormatInfc(name, content, source);
		}
#endif




	private:
#ifdef LEX_SOURCE
		virtual void AddFormatImpl(const std::string_view& name, const std::string_view& content, Script* source) = 0;
		virtual Script* FindScriptImpl(const std::string_view& name) = 0;
#endif

		void AddFormatInfc(const std::string_view& name, const std::string_view& content, IScript* source) override;
		IScript* FindScriptInfc(const std::string_view& name) override;


	};
}