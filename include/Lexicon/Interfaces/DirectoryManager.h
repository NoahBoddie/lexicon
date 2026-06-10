#pragma once

#include "Lexicon/InfoType.h"
#include "Lexicon/ComponentType.h"
#include "Lexicon/Interfaces/IComponent.h"

namespace LEX
{
	struct Info;
	struct IComponent;
	struct IElement;
	struct IFunction;
	struct IDirectory;

	struct ISignature;
	struct Element;

	struct TypeNode;
	struct QualifiedField;
	struct FunctionNode;
	struct SyntaxRecord;
	struct Script;
	struct Overload;
	struct OverloadArgument;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(DirectoryManager)
			{
				virtual Info* GetInfoFromPath(IElement* a_this, std::string_view path, InfoType type, const LEX::ISignature* sign = nullptr) = 0;
				
				virtual IComponent* GetComponentFromPath(IElement* a_this, std::string_view path, ComponentType elem, const LEX::ISignature* sign = nullptr) = 0;
			};
		}

		CURRENT_VERSION(DirectoryManager, 1);
	}


	struct IMPL_SINGLETON(DirectoryManager)
	{
		Info* GetInfoFromPath(IElement* a_this, std::string_view path, InfoType type, const LEX::ISignature* sign = nullptr) override { return nullptr; }

		IComponent* GetComponentFromPath(IElement* a_this, std::string_view path, ComponentType comp, const LEX::ISignature* sign = nullptr) override;


		IFunction* GetConstructorFromPath(IElement* a_this, std::string_view path, const LEX::ISignature* sign = nullptr)
		{
			return GetComponentFromPath(a_this, path, ComponentType::Constructor, sign)->AS_COMPONENT(IFunction);
		}

#ifdef LEX_SOURCE

		//This is supposed to use a record.
		IComponent* GetComponentFromPath(Element* a_this, SyntaxRecord& path, ComponentType comp, OverloadArgument* sign = {});

		TypeNode SearchTypePath(Element* a_this, SyntaxRecord& path);


		FunctionNode SearchFunctionPath(Element* a_this, SyntaxRecord& path, OverloadArgument& key, Overload& out);

		FunctionNode SearchFunctionPath(Element* a_this, SyntaxRecord& path, OverloadArgument& key);

		QualifiedField SearchFieldPath(Element* a_this, SyntaxRecord& path);

		Script* SearchScriptPath(Element* a_this, SyntaxRecord& path);
#endif

		//TODO: I'd like to have versions of these that are

	};
}