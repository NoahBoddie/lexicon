#pragma once

#include "Lexicon/Interfaces/IDirectory.h"
#include "Lexicon/Interfaces/IEnvironmentBase.h"

namespace LEX
{


	struct IEnvironment : INTERFACE_DERIVES(IEnvironmentBase, IDirectory)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::IEnvironment)

			/*
			IFunction* GetFunctionFromPath(std::string_view path, ISignature& sign)
			{
				if (auto elem = GetElementFromPath(path, kFuncElement, &sign); elem)
					return elem->As<IFunction>();

				return nullptr;
			}

			ITypeInfo* GetTypeFromPath(std::string_view path)
			{
				if (auto elem = GetElementFromPath(path, kTypeElement); elem)
					return elem->As<ITypeInfo>();

				return nullptr;
			}
			//*/
	};
}