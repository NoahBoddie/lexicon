#pragma once

#include "Lexicon/Interfaces/Environment.h"

//*src
//#include "Function.h"

#include "Lexicon/Engine/ConcretePolicy.h"
#include "Lexicon/Interfaces/Project.h"

#include "Lexicon/Component.h"
#include "Lexicon/Engine/ExpressionType.h"
#include "Lexicon/Exception.h"
#include "Lexicon/Engine/Parser.h"
#include "Lexicon/Interfaces/Element.h"

#include "Lexicon/TypeID.h"
#include "Lexicon/Engine/FunctionInfo.h"
#include "Lexicon/Engine/VariableInfo.h"//For tests only
//*src
#include "Lexicon/Engine/ConcreteFunction.h"

#include "Lexicon/Engine/GlobalVariable.h"


#include "Lexicon/Engine/OverloadInput.h"

#include "Lexicon/Engine/QualifiedField.h"

#include "Lexicon/Interfaces/ProjectManager.h"

#include "Lexicon/Engine/PolicyBase.h"

#include "Lexicon/Engine/parse_strings.h"

#include "Lexicon/Interfaces/Script.h"

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

			if (auto it = functionMap.find(name); end != it) {
				report::compile::critical("Non - FunctionData IFunction attempted to be added");
				throw nullptr;
			}
			else {
				auto& info = functionMap[name] = new FunctionInfo;
				info->function = tar;
				info->signature = tar;
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

			//TODO: FindFunctions is busted because I need to LoadFromRecord for a name but needs to be added to Load.
			if (auto it = functionMap.find(name); end != it) {
			//if (auto it = std::find_if(functionMap.begin(), functionMap.end(), [&](auto i) { return name == i.second.Get()->GetName(); }); end != it) {
				result.push_back(it->second);
			}
			
			return result;
		}




		
		VariableInfo* Environment::FindVariable(std::string name)
		{
			report::fault::critical("Not implemented yet");
			auto end = variables.end();

			if (auto it = std::find_if(variables.begin(), end, [&](auto i) {return name == i->GetName(); }); end != it) {
				//return *it;
				return nullptr;
			}
			else {
				return nullptr;
			}
		}

		std::vector<PolicyBase*> Environment::FindTypes(std::string name)
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




		bool Environment::CheckOverload(OverloadKey& input, std::vector<FunctionInfo*> clauses, Overload& ret)
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
				ret = *last;//this should move

			return last;
		}


		inline PolicyBase* Environment::SearchTypeImpl(std::string name, OverloadKey* key)
		{
			//No overload key means it has is expected to have no generic, if so it has one.

			//if (!key)
			//	return dynamic_cast<PolicyBase*>(a_this->TEMPSearchType(name));
			//else
			//	return dynamic_cast<PolicyBase*>(a_this->SearchType(name, *key));


			//One cannot get a type
			//Returns an ITypePolicy because the template args can still be

			std::vector<Environment*> query;

			//Submitting includes also includes self. If self does not exist, it will cease to continue.
			query.insert_range(query.end(), GetRecursiveIncluded());

			for (auto env : query)
			{

				std::vector<PolicyBase*> types = env->FindTypes(name);

				//There's no situation where multiple can be observed, so it only needs the one.

				auto size = types.size();



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


		QualifiedField Environment::SearchFieldImpl(std::string name, OverloadKey* key)
		{
			//No overload key means it has is expected to have no generic, if so it has one.
			TargetObject;

			std::vector<Environment*> query;

			//QualifiedType type = key.GetQualifiedType();


			//Submitting includes also includes self. If self does not exist, it will cease to continue.
			query.insert_range(query.end(), GetRecursiveIncluded());

			for (auto env : query)
			{



				//There's no situation where multiple can be observed, so it only needs the one.
				VariableInfo* var = env->FindVariable(name);

				if (var)
				{
					//return global;
					return QualifiedField{ var, NULL_OP(key)->GetTarget() };
				}

			}

			//Environment* commons = 

			//query = 

			return nullptr;

		}


		FunctionInfo* Environment::SearchFunctionImpl(std::string name, OverloadKey& key, Overload& out)
		{
			//This requires a key, none optional

			//This needs a function above it that fills generic arguments. Possibly scratch that.


			std::vector<Environment*> query;

			//QualifiedType type = key.GetQualifiedType();


			//Submitting includes also includes self. If self does not exist, it will cease to continue.
			query.insert_range(query.end(), GetRecursiveIncluded());

			for (auto env : query)
			{
				//this should compile and then run.
				auto funcs = env->FindFunctions(name);

				//FunctionBase* test = functions[0]->Get();

				if (funcs.size() && CheckOverload(key, { funcs.begin(), funcs.end() }, out) == true)
				{
					return dynamic_cast<FunctionInfo*>(out.clause);
				}

			}

			return nullptr;


		}





		FunctionInfo* Environment::SearchFunction(Record& path, OverloadKey& key, Overload& out)
		{
			//This should prepare the generic arguments.

			return SearchFunctionImpl(path.GetTag(), key, out);
		}


		///These functions are supposed to resolve generic ambiguity.
		PolicyBase* Environment::SearchType(Record& path)
		{
			return SearchTypeImpl(path.GetTag(), nullptr);
		}


		QualifiedField Environment::SearchField(Record& path)
		{
			//Should do similar shit to search environment, creating generic args along the way or something like that.
			return SearchFieldImpl(path.GetTag(), nullptr);

		}







		Environment* Environment::SearchEnvironment(Record& path) {
			//A matter of preference here is that unless it's specifically referencing something, it will try all it's options.

			//So the order of checking goes from type -> script -> project

			SyntaxType type = path.SYNTAX().type;
			Prefer force;
			bool cont = false;
			Environment* result = nullptr;
			Element* stuff;

			//Instead, I will be getting elements and will refer to them as scopes.
			//If element, what can it not do?
			//Elements can getprojects so they can get scripts, projects need to become environments. At least in name. Kinda like a pivot.
			// An I environments.


			do
			{//the idea is that continue is set once when scope name is found once. After that, it's put into a cycle of which it prefers.

				switch (type)
				{
				case SyntaxType::Scriptname://This should be script name.
					type = SyntaxType::ProjectName;
					force = Prefer::Script; break;

				case SyntaxType::Scopename:
					cont = true;

					[[fallthrough]];
				case SyntaxType::Typename:
					type = SyntaxType::Scriptname;
					force = Prefer::Type; break;



				case SyntaxType::ProjectName:
					cont = false;
					force = Prefer::Project; break;
				default:
					return nullptr;//invalid
				}

				switch (force)
				{
				case Prefer::Script:
				{
					//Rule, no children.

					auto bybfi = GetProject()->FindScript(path.GetTag());
					result = bybfi;
					break;
				}

				case Prefer::Type:
				{
					//OverloadInput input{};

					OverloadKey* key = nullptr;

					/*//This hasn't been implemented yet, so curbing it.
					if (g_args)
					{
						auto& args = *g_args;

						//Load the generic arguments

						for (auto& arg : *g_args)
						{
							auto elem = SearchTypePath(a_this, std::string{ arg });

							ITypePolicy* type = dynamic_cast<ITypePolicy*>(elem);

							if (!type) {
								throw nullptr;
							}



							input.genericInput.push_back(type);
						}

						key = &input;
					}
					//*/

					result = SearchTypeImpl(path.GetTag(), key);


					break;
				}

				case Prefer::Project:
				{

					ProjectManager::instance->GetProject(path.GetTag());
					result = nullptr;
					break;
				}



				}



			} while (cont && !result);



			return result;
		}






		Environment* Environment::SearchEnvironmentPath(Record*& _path)
		{
			//This should keep going as long as the left path is a binary. When it stops being so, it should return the last searching environment, and adjust the
			// path reference.

			switch (_path->SYNTAX().type)
			{
			default:
				if (auto new_path = _path->FindChild(parse_strings::path); new_path) {
					_path = new_path;
					break;
				}
			[[fallthrough]];

			case SyntaxType::Typename:
			case SyntaxType::Scriptname:
				return this;
			
			case SyntaxType::Path:
				break;
			}



			if (auto left = _path->FindChild(parse_strings::lhs); left) {
				Environment* a_this = SearchEnvironment(*left);
				_path = _path->FindChild(parse_strings::rhs);
				return a_this->SearchEnvironmentPath(_path);
			}
			else
			{
				return SearchEnvironment(_path->GetFront());
			}
		}


		PolicyBase* Environment::SearchTypePath(Record& _path)
		{
			Record* path = &_path;

			Environment* env = SearchEnvironmentPath(path);

			return env->SearchType(_path);

		}

		//This actually probably will want to return the function info, because of something like generic functions.
		FunctionInfo* Environment::SearchFunctionPath(Record& _path, OverloadKey& input, Overload& out)
		{
			Record* path = &_path;

			Environment* env = SearchEnvironmentPath(path);

			return env->SearchFunction(_path, input, out);

			//While this can be a field, it will not be treated like one.

			//Here, record is only for the path, not for anything else.

			//out is only used when we actually need it.

			//THIS is the way these functions should look.
			if (!this) {
				return nullptr;
			}

			//Record* path = &_path;

			if (Environment* env = SearchEnvironmentPath(path); !env) {
				//This is mostly correct, but it should also use it's includes. Hm, that actually would be better suited in the search functions below
				// when compiling all viable named functions/types/fields.
				return env->SearchFunctionPath(_path, input, out);
			}
			else {
				return env->SearchFunction(*path, input, out);
			}


		}

		QualifiedField Environment::SearchFieldPath(Record& _path)
		{
			//Similar to type, this doesn't quite need overloads.


			Record* path = &_path;

			Environment* env = SearchEnvironmentPath(path);


			return env->SearchField(_path);
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