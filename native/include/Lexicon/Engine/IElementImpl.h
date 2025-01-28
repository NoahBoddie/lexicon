#pragma once

#include "Lexicon/Interfaces/IElement.h"

namespace LEX
{
	class Script;
	class Project;
	struct Element;
	struct Environment;
	struct OverloadKey;

	template <std::derived_from<IElement> Base>
	struct IElementBase : public Base
	{
	private:
		IScript* GetScript(bool) override final;
		IProject* GetProject(bool) override final;
		IElement* GetParent(bool)  override final;
		IElement* GetEnvironment(bool)  override final;
		IElement* GetElementFromPath(std::string_view path, ElementType elem, SignatureBase* base, bool) override final;
		IScript* GetCommons(bool) override final;

	public:
		virtual Script* GetScript() = 0;
		virtual Project* GetProject() = 0;
		virtual IElement* GetParent() = 0;
		virtual Environment* GetEnvironment() = 0;
		virtual Element* GetElementFromPath(std::string_view path, ElementType elem, OverloadKey* sign = nullptr) = 0;
		virtual Script* GetCommons() = 0;
	};


	struct IElementImpl : public IElementBase<IElement>
	{
		/*
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

		//*/
	};
}