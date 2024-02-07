#pragma once

#include "Environment.h"

//*src
//#include "Function.h"
#include "GlobalVariable.h"
#include "ConcretePolicy.h"
#include "Project.h"

#include "Lexicon/Component.h"
#include "ExpressionType.h"
#include "Exception.h"
#include "Parser.h"
#include "Element.h"

#include "TypeID.h"
#include "FunctionInfo.h"
#include "VariableInfo.h"//For tests only
//*src
#include "ConcreteFunction.h"

#include "GlobalVariable.h"

namespace LEX
{

		void Environment::AddFunction(FunctionBase* tar)
		{
			if (!tar) {
				RGL_LOG(critical, "Non - FunctionData IFunction attempted to be added");
				throw nullptr;
			}

			auto end = functionMap.end();

			auto name = tar->GetName();

			if (auto it = functionMap.find(name); end != it) {
				RGL_LOG(critical, "Non - FunctionData IFunction attempted to be added");
				throw nullptr;
			}
			else {
				functionMap[name].function = tar;
				DeclareParentTo(tar);
			}

		}


		void Environment::AddVariable(Global* tar)
		{
			//This won't have an issue yet, because no abstraction.

			auto end = variables.end();

			auto name = tar->GetName();

			if (auto it = std::find_if(variables.begin(), end, [&](auto i) {return name == i->GetName(); }); end != it) {
				RGL_LOG(critical, "Variable already existed");
				throw nullptr;
			}
			else {
				variables.emplace_back(tar);
				DeclareParentTo(tar);
			}
		}

		std::vector<FunctionInfo*> Environment::FindFunctions(std::string name)
		{
			std::vector<FunctionInfo*> result{};

			auto end = functionMap.end();
			logger::trace("should have cap {} from {}", functionMap.size(), name);

			//TODO: FindFunctions is busted because I need to LoadFromRecord for a name but needs to be added to Load.
			if (auto it = functionMap.find(name); end != it) {
			//if (auto it = std::find_if(functionMap.begin(), functionMap.end(), [&](auto i) { return name == i.second.Get()->GetName(); }); end != it) {
				logger::trace("should have size {}", functionMap.size());
				result.push_back(&it->second);
			}

			return result;
		}




		
		Global* Environment::FindVariable(std::string name)
		{
			auto end = variables.end();

			if (auto it = std::find_if(variables.begin(), end, [&](auto i) {return name == i->GetName(); }); end != it) {
				return *it;
			}
			else {
				return nullptr;
			}
		}


		std::vector<ITypePolicy*> Environment::FindTypes(std::string name)
		{
			auto end = typeMap.end();

			if (auto it = typeMap.find(name); end != it) {
				return { it->second };
			}
			else {
				logger::critical("no type of {} found", !typeMap[name]);
				throw nullptr;
				return {};
			}
		}


		std::vector<Environment*> Environment::GetIncluded() { return {}; }

		std::vector<Environment*> Environment::GetImport() { return {}; }

		
		void Environment::GetRecursiveIncluded(std::set<Environment*>& cache)
		{
			if (!this || !cache.emplace(this).second) {
				logger::critical("CCCCCCCCCCCCCCCC");
				return;

			}

			std::vector<Environment*> result = GetIncluded();

			cache.insert_range(result);

			for (auto environment : result) {
				environment->GetRecursiveIncluded(cache);
			}


		}


		std::vector<Environment*> Environment::GetRecursiveIncluded()
		{
			std::set<Environment*> cache;

			GetRecursiveIncluded(cache);

			return { cache.begin(), cache.end() };

		}


		Field* Environment::SearchField(std::string name, Environment* scope, ITypePolicy* target, std::vector<ITypePolicy*>* temp_args, std::vector<ITypePolicy*>* func_args, FieldSearch func)
		{
			SearchParams params;

			params.name = name;
			params.scope = scope;
			params.target = target;
			//This is stupid, I don't need this.
			if (temp_args) params.tempArgs = *temp_args;
			if (func_args) params.funcArgs = *func_args;

			std::vector<Environment*> query;


			logger::critical("AAAAAAAAAAA");



			//Submitting includes also includes self. If self does not exist, it will cease to continue.
			query.insert_range(query.end(), GetRecursiveIncluded());

			for (auto env : query)
			{
				logger::critical("FFFFFF");
				std::vector<Field*> result;

				std::vector<FunctionInfo*> functions = env->FindFunctions(name);

				//There's no situation where multiple can be observed, so it only needs the one.
				Global* global = env->FindVariable(name);

				auto size = functions.size();

				//TODO: VERY temporary idea. No pattern matching, no checking. This is basically the same that we did before
				if (size > 1 || size && global) {
					RGL_LOG(critical, "mulitple fields of same name detected.");
					throw nullptr;
				}
				else if (global)
				{
					return global;
				}
				else if (size)
				{
					//IFunction is not a field
					return functions[0];
				}

			}

			//Environment* commons = 

			//query = 

			return nullptr;
		}


		ITypePolicy* Environment::SearchType(std::string name, Environment* scope, std::vector<ITypePolicy*>* temp_args, std::vector<ITypePolicy*>* func_args, TypeSearch func)
		{
			//One cannot get a type
			//Returns an ITypePolicy because the template args can still be


			SearchParams params;

			params.name = name;
			params.scope = scope;

			//This is stupid, I don't need this.
			if (temp_args) params.tempArgs = *temp_args;
			if (func_args) params.funcArgs = *func_args;

			std::vector<Environment*> query;

			//Submitting includes also includes self. If self does not exist, it will cease to continue.
			query.insert_range(query.end(), GetRecursiveIncluded());

			logger::trace(" query {}", query.size());

			for (auto env : query)
			{
				std::vector<Field*> result;

				std::vector<ITypePolicy*> types = env->FindTypes(name);

				//There's no situation where multiple can be observed, so it only needs the one.

				auto size = types.size();

				logger::trace("types query {}", size);

				//TODO: VERY temporary idea. No pattern matching, no checking. This is basically the same that we did before
				if (size > 1) {
					RGL_LOG(critical, "mulitple types of same name detected.");
					throw nullptr;
				}
				else if (size)
				{
					//IFunction is not a field
					return types[0];
				}

			}
			return nullptr;
		}


		Field* Environment::TEMPSearchField(std::string name)
		{
			return SearchField(name, nullptr, nullptr, nullptr, nullptr, nullptr);
		}


		ITypePolicy* Environment::TEMPSearchType(std::string name)
		{
			return SearchType(name, nullptr, nullptr, nullptr, nullptr);
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
				RGL_LOG(critical, "parent set for Element was not an environment.");
				throw nullptr;
			}

			_parent = par;
		}

		void Environment::AddType(PolicyBase* policy)
		{
			if (!policy) {
				RGL_LOG(critical, "Null Policy attempted to be added");
				throw nullptr;
			}

			auto end = typeMap.end();

			auto name = policy->GetName();

			if (auto it = typeMap.find(name); end != it) {
				RGL_LOG(critical, "Non - FunctionData IFunction attempted to be added");
				throw nullptr;
			}
			else {
				logger::critical("type {} added", name);
				typeMap[name] = policy;
				DeclareParentTo(policy);
			}

		}



		Record* SecondaryEnvironment::GetSyntaxTree()
		{
			return _syntax;
		}

		void SecondaryEnvironment::SetSyntaxTree(Record& rec)
		{
			if (!_syntax)
				_syntax = &rec;
		}
}