//This file is able to be included multiple times in order to reuse the code declared within.


#include "Lexicon/Interfaces/IElement.h"


//#define IMPL_TYPE_NAME IElement

#ifdef IMPL_TYPE_NAME

namespace LEX
{
	class Script;
	class Project;
	struct Element;
	struct Environment;

	struct CONCAT(IMPL_TYPE_NAME, Impl) : public IMPL_TYPE_NAME
	{
	private:
		IScript* GetScript() override final;
		IProject* GetProject() override final;
		IElement* GetParent()  override final;
		IEnvironment* GetEnvironment()  override final;
		IElement* GetElementFromPath(std::string_view path, ElementType elem) override final;
		IScript* GetCommons() override final;

	public:
		virtual Script* GetScript(bool = {}) = 0;
		virtual Project* GetProject(bool = {}) = 0;
		virtual IElement* GetParent(bool = {}) = 0;
		virtual Environment* GetEnvironment(bool = {}) = 0;
		virtual Element* GetElementFromPath(std::string_view path, ElementType elem, bool = {}) = 0;
		virtual Script* GetCommons(bool = {}) = 0;
	};
}

#endif

#undef IMPL_TYPE_NAME