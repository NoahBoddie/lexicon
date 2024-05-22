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


#include "OverloadInput.h"

#include "QualifiedField.h"

namespace LEX
{

		void Environment::AddFunction(FunctionBase* tar)
		{
			if (!tar) {
				report::compile::critical("Non - FunctionData IFunction attempted to be added");
				throw nullptr;
			}

			auto end = functionMap.end();

			auto name = tar->GetName();

			if (auto it = functionMap.find(name); end != it) {
				report::compile::critical("Non - FunctionData IFunction attempted to be added");
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
				report::compile::critical("Variable already existed");
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

		bool CheckOverload(OverloadKey& input, std::vector<OverloadClause*> clauses, Overload& ret)
		{
			Overload out;

			Overload* last = nullptr;


			for (auto clause : clauses)
			{
				OverloadFlag flags = OverloadFlag::None;

				Overload overload = input.Match(clause, nullptr, last, flags);

				if (flags & OverloadFlag::Failure) {
					logger::info("Failure"); 
					continue;
				}



				if (flags & OverloadFlag::Ambiguous) {
					logger::info("Ambiguous");
					last = nullptr;
					break;
				}

				if (0) {
					bool _continue = false;
					bool _break = false;


					cycle_switch(flags)
					{
					case OverloadFlag::Failure:
						logger::info("Failure");
						_continue = true;
						continue;

					case OverloadFlag::Ambiguous:
						logger::info("Ambiguous");
						_break = true;
						break;

					}

					if (_continue)
						continue;

					if (_break)
						break;

				}


				out = std::move(overload);

				last = &out;
			}

			if (last)
				ret = *last;

			return last;
		}


		QualifiedField Environment::SearchField(std::string name, OverloadKey& key, FieldPredicate pred)
		{
			std::vector<Environment*> query;

			//QualifiedType type = key.GetQualifiedType();


			//Submitting includes also includes self. If self does not exist, it will cease to continue.
			query.insert_range(query.end(), GetRecursiveIncluded());

			for (auto env : query)
			{
				std::vector<Field*> result;

				std::vector<FunctionInfo*> functions = env->FindFunctions(name);

				//There's no situation where multiple can be observed, so it only needs the one.
				Global* global = env->FindVariable(name);

				auto size = functions.size();

				//TODO: VERY temporary idea. No pattern matching, no checking. This is basically the same that we did before
				if (size > 1 || size && global) {
					report::compile::critical("mulitple fields of same name detected.");
					throw nullptr;
				}
				else if (global)
				{
					//return global;
					return nullptr;
				}
				else if (size)
				{
					//TODO: This is ass

					//Within field searches the look for a function will be handled very differently.

					FunctionBase* test = functions[0]->Get();

					Overload overload;//TODO: This is required for user conversions and for implementation ones. Address this.

					if (test && CheckOverload(key, { test }, overload) == true) {
						
						return QualifiedField{ functions[0], overload.target };//Is a qualified field like this right?
					}
				}

			}

			//Environment* commons = 

			//query = 

			return nullptr;
		}


		Overload Environment::SearchFunction(std::string name, OverloadKey& key, FieldPredicate pred)
		{
			//I want to work on this before I use it. Namely I want function because what if there's a function call operator you know?
			// This works for now, but I'll adapt it later.

			//Likely, this will simply be for callable thing. Operators like that will have to wait though.

			std::vector<Environment*> query;

			query.insert_range(query.end(), GetRecursiveIncluded());

			for (auto env : query)
			{
				std::vector<Field*> result;

				std::vector<FunctionInfo*> functions = env->FindFunctions(name);

				FunctionBase* test = functions[0]->Get();

				Overload overload;//TODO: This is required for user conversions and for implementation ones. Address this.

				if (test && CheckOverload(key, { test }, overload) == true)
					return overload;
			}
			
			return {};
		}



		ITypePolicy* Environment::SearchType(std::string name, OverloadKey& key, TypePredicate pred)
		{
			//One cannot get a type
			//Returns an ITypePolicy because the template args can still be

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
					report::compile::critical("mulitple types of same name detected.");
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


		QualifiedField Environment::TEMPSearchField(std::string name)
		{
			OverloadInput input;

			return SearchField(name, input, nullptr);
		}


		ITypePolicy* Environment::TEMPSearchType(std::string name)
		{
			OverloadInput input;
			return SearchType(name, input, nullptr);
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

		void Environment::AddType(PolicyBase* policy)
		{
			if (!policy) {
				report::compile::critical("Null Policy attempted to be added");
				throw nullptr;
			}

			auto end = typeMap.end();

			auto name = policy->GetName();

			if (auto it = typeMap.find(name); end != it) {
				report::compile::critical("Non - FunctionData IFunction attempted to be added");
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