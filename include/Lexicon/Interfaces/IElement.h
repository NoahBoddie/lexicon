#pragma once

#include "Lexicon/Interfaces/Interface.h"

#include "Lexicon/ElementType.h"

#include "Lexicon/Interfaces/IElementBase.h"

#include "Lexicon/Interfaces/IComponent.h"

namespace LEX
{
	struct SignatureBase;
	struct IFunction;
	struct ITypeInfo;

	struct IElement : INTERFACE_DERIVES(IElementBase, IComponent)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::IElement, ComponentType::Element)
	private:

#ifdef LEX_SOURCE
		Script* GetScriptImpl() override;
		Project* GetProjectImpl() override;
		Directory* GetParentImpl() override;
		Environment* GetEnvironmentImpl() override;
		Directory* GetDirectoryImpl() override;
		Script* GetCommonsImpl() override;

		Element* GetElementFromPathImpl(std::string_view path, ElementType elem, OverloadArgument* sign = nullptr) override;

#endif
	public:
		//*
		IFunction* GetFunctionFromPath(const std::string_view& path, SignatureBase* sign)
		{
			if (auto elem = GetElementFromPathInfc(path, kFuncElement, sign); elem)
				return elem->As<IFunction>(ComponentType::ITypeInfo);

			return nullptr;
		}

		ITypeInfo* GetTypeFromPath(const std::string_view& path)
		{
			if (auto elem = GetElementFromPathInfc(path, kTypeElement); elem)
				return elem->As<ITypeInfo>(ComponentType::ITypeInfo);

			return nullptr;
		}
		//*/
	};


}