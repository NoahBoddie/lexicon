#pragma once

#include "Lexicon/Engine/Environment.h"

//*src
//#include "Function.h"

#include "Lexicon/Engine/ConcreteType.h"
#include "Lexicon/Engine/Project.h"

#include "Lexicon/Engine/Component.h"
#include "Lexicon/Engine/ExpressionType.h"
#include "Lexicon/Exception.h"
#include "Lexicon/Engine/Parser.h"
#include "Lexicon/Engine/Element.h"

#include "Lexicon/TypeID.h"
#include "Lexicon/Engine/FunctionInfo.h"
#include "Lexicon/Engine/VariableInfo.h"//For tests only
//*src
#include "Lexicon/Engine/GenericFunction.h"
#include "Lexicon/Engine/ConcreteFunction.h"

#include "Lexicon/Engine/GlobalVariable.h"
#include "Lexicon/Engine/GlobalBase.h"


#include "Lexicon/Engine/OverloadInput.h"

#include "Lexicon/Engine/QualifiedField.h"

#include "Lexicon/Interfaces/ProjectManager.h"

#include "Lexicon/Engine/TypeBase.h"

#include "Lexicon/Engine/parse_strings.h"

#include "Lexicon/Engine/Script.h"

//SHOULD_NATIVE
#include "Lexicon/Engine/FunctionInfo.h"
#include "Lexicon/Engine/VariableInfo.h"


namespace LEX
{

	Environment::~Environment()
	{
		//Destroy all info data.
	}


		void Environment::AddFunction(FunctionBase* tar)
		{
			if (!tar) {
				report::compile::critical("Non - FunctionData IFunction attempted to be added");
				throw nullptr;
			}

			auto end = functionMap.end();

			auto name = tar->GetName();

			//if (auto it = functionMap.find(name); end != it) {
			//	report::compile::critical("Non - FunctionData IFunction attempted to be added");
			//	throw nullptr;
			//}
			//else {
				auto& info = functionMap[name].emplace_back( new FunctionInfo);
				info->function = tar;
				info->signature = tar;
				DeclareParentTo(tar);
			//}

		}


		void Environment::AddVariable(GlobalBase* tar)
		{
			//This won't have an issue yet, because no abstraction.

			auto end = variables.end();

			auto name = tar->GetName();

			if (auto it = std::find_if(variables.begin(), end, [&](auto i) {return name == i->GetName(); }); end != it) {
				report::compile::critical("Variable already existed");
				throw nullptr;
			}
			else {
				variables.emplace_back(tar);
				DeclareParentTo(tar);
			}
		}

		std::vector<FunctionInfo*> Environment::FindFunctions(std::string_view name)
		{
			//std::vector<FunctionInfo*> result{};

			auto end = functionMap.end();

			//TODO: FindFunctions is busted because I need to LoadFromRecord for a name but needs to be added to Load.
			if (auto it = functionMap.find(name); end != it) {
			//if (auto it = std::find_if(functionMap.begin(), functionMap.end(), [&](auto i) { return name == i.second.Get()->GetName(); }); end != it) {
				return it->second;
			}
			
			return {};
		}




		
		GlobalBase* Environment::FindVariable(std::string_view name)
		{
			
			auto end = variables.end();

			if (auto it = std::find_if(variables.begin(), end, [&](auto i) {return name == i->GetName(); }); end != it) {
				return *it;
				return nullptr;
			}
			else {
				return nullptr;
			}
		}

		std::vector<TypeBase*> Environment::FindTypes(std::string_view name)
		{
			auto end = typeMap.end();

			if (auto it = typeMap.find(name); end != it) {
				return { it->second };
			}
			else {
				return {};
			}
		}







		Environment* Environment::GetEnvironment()
		{
			return this;
		}

		//source file type shit
		Element* Environment::GetParent()
		{
			return _parent;
		}



		void Environment::SetParent(Element* par)
		{
			if (_parent)
				return;
			//I'd like something easier for this
			//Perhaps fetch project and 

			//Environment* env = dynamic_cast<Environment*>(par);
			//Project* proj = dynamic_cast<Project*>(par);

			//if (par->FetchProject() != par && par->FetchEnvironment() != par) {
			if (!dynamic_cast<Project*>(par) && !dynamic_cast<Environment*>(par)) {
				report::compile::critical("parent set for Element was not an environment.");
				throw nullptr;
			}

			_parent = par;
		}

		void Environment::AddType(TypeBase* policy)
		{
			if (!policy) {
				report::compile::critical("Null Policy attempted to be added");
				throw nullptr;
			}

			auto end = typeMap.end();

			auto name = policy->GetName();

			if (auto it = typeMap.find(name); end != it) {
				report::compile::critical("Type already exists {}.", name);
				throw nullptr;
			}
			else {
				report::compile::info("type {} added to {}", name, GetName());
				typeMap[name] = policy;
				DeclareParentTo(policy);
			}

		}

		void Environment::CreateFunction(SyntaxRecord& node)
		{
			std::unique_ptr<FunctionBase> func;

			if (IsGenericElement() || node.FindChild(parse_strings::generic) != nullptr) {
				func = std::unique_ptr<FunctionBase>(Component::Create<GenericFunction>(node));
			}
			else {
				func = std::unique_ptr<FunctionBase>(Component::Create<ConcreteFunction>(node));
			}

			AddFunction(func.get());

			func.release();
		}



		SyntaxRecord* SecondaryEnvironment::GetSyntaxTree()
		{
			return _syntax;
		}

		void SecondaryEnvironment::SetSyntaxTree(SyntaxRecord& rec)
		{
			if (!_syntax)
				_syntax = &rec;
		}
}