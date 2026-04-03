#pragma once

#include "Lexicon/Interfaces/IRepository.h"

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

			};
		}

		CURRENT_VERSION(IProject, 1);

	}

	struct IMPL_VERSION(IProject, IRepository)
	{
	public:
		DEFINE_COMPONENT_OFFSET(ComponentType::IProject, ComponentType::Project);

	private:
#ifdef LEX_SOURCE
		using script_t = Script;
#else
		using script_t = IScript;
#endif	
	public:

		auto AddFormat(const std::string_view& name, const std::string_view& content, script_t* source)
		{
#ifdef LEX_SOURCE
			return AddFormatImpl(name, content, source);
#else
			return AddFormatInfc(name, content, source);
#endif	
		}




	private:
#ifdef LEX_SOURCE
		virtual void AddFormatImpl(const std::string_view& name, const std::string_view& content, Script* source) = 0;
#endif

		void AddFormatInfc(const std::string_view& name, const std::string_view& content, IScript* source) override;

	};
}