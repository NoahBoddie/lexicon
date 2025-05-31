#include "Lexicon/Engine/Element.h"


#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"
#include "Lexicon/Interfaces/ProjectManager.h"
#include "Lexicon/Engine/Environment.h"

#include "Lexicon/Engine/TypeBase.h"
#include "Lexicon/Engine/ParserTest.h"
#include "Lexicon/Engine/FunctionInfo.h"
#include "Lexicon/Engine/GlobalBase.h"
#include "Lexicon/Engine/SyntaxRecord.h"

#include "Lexicon/Engine/ParseUtility.h"

#include "Lexicon/GenericArray.h"
#include "Lexicon/Engine/QualifiedName.h"
#include "Lexicon/Engine/GenericBase.h"
#include "Lexicon/MergeTemplate.h"
namespace LEX
{

	Script* Element::GetScript()
	{
		return GetParent()->FetchScript();
	}

	Project* Element::GetProject()
	{
		return GetParent()->FetchProject();
	}

	Project* Element::GetShared()
	{
		//Actually supposed to search for the project manager
		return ProjectManager::instance->GetShared();
	}
	
	Script* Element::GetCommons() { return GetScript()->FetchCommons(); }




	//the associated should maybe be a bool or just reject any other than include and import.

	SyntaxRecord& Element::GetPath(SyntaxRecord& path, std::optional<bool> right)
	{
		SyntaxRecord* ret = nullptr;
			
		if (right.has_value())
			path.FindChild(right.value() ? parse_strings::rhs : parse_strings::lhs);

		if (!ret)
			ret = &path.GetFront();
		RecordBase;
		static_assert(derived_record<SyntaxRecord>);
		static_assert(std::derived_from <SyntaxRecord, RecordBase>);
		static_assert(sizeof(BasicRecord<Syntax>) == sizeof(RecordBase));
		constexpr auto size = sizeof(SyntaxRecord);

		return ret->Transform<SyntaxRecord>();
	}


	
	Environment* Element::WalkEnvironmentPath(SyntaxRecord* path, ITemplateInserter& inserter)
	{
		TypeBase;

		Element* a_this = this;

		while (path && path->IsPath() == true)
		{
			//if (path->IsPath() == false) {
			//	path = nullptr;
			//	return FetchEnvironment();
			//}

			if (!a_this)
				return nullptr;


			if constexpr (1) {
				auto below = ParseUtility::SeekNextPath(path);

				a_this = a_this->FindEnvironment(*below, inserter);
			}
			else
			{
				auto left = path->FindChild(parse_strings::lhs);


				if (!left) {
					return a_this->FindEnvironment(path->GetFront(), inserter);
				}


				a_this = a_this->FindEnvironment(left->GetFront(), inserter);

				path = path->FindChild(parse_strings::rhs);
			}
		}

		return a_this->FetchEnvironment();

	}


	std::vector<QualifiedName> Element::GetEnvironments(Element* a_this, SyntaxRecord* step, RelateType a, std::set<Element*>& searched)
	{
		/*
		make variable that stores temp environment here.
		overloop here
		for each environment in the temp var:

		Check for environ being in the set.
		Add environ to the set.
		Call function.
		move temp var restart overloop.

		Overloop keeps going until absolutely nothing is created from GetAssociate
		//*/

		//This could be cleaner, but it works for now.

		std::vector<QualifiedName> result{};
		std::vector<Element*> elements{ };
		

		std::vector<Element*> out{ a_this };


		//maybe there's a better way to do this, but whatever innit.
		while (out.size() != 0)//Overloop
		{


			std::vector<Element*> buffer{};

			for (auto elem : out)
			{
				if (!elem || !searched.emplace(elem).second) {
					continue;
				}			
				

				GenericArray inserter{ FetchGenericElement(), };
				Environment* env = step ? elem->WalkEnvironmentPath(step, inserter) : elem->GetEnvironment();
				
				if (env)
					result.emplace_back(env, std::move(inserter));
				
				if (a != RelateType::None)
					buffer.insert_range(buffer.end(), elem->GetAssociates(a));
			}

			out = std::move(buffer);
		}

		return result;
	}

	TypeNode Element::SearchTypePath(SyntaxRecord& _path)
	{
		return SearchTypePath(this, _path);
	}


	TypeNode Element::SearchTypePath(Element* a_this, SyntaxRecord& _path)
	{
		if (a_this && _path.FindChild(parse_strings::path) == nullptr)
		{
			if (auto gen_elem = a_this->AsGenericElement())
			{
				if (auto temp = gen_elem->GetTemplateByName(_path.GetView())) {
					return TypeNode{nullptr, temp };
				}
			}
		}



		TypeNode result;

		SearchPathBase(a_this, _path, [&](std::vector<QualifiedName>& query) -> bool
			{
				for (auto& env : query)
				{

					std::vector<TypeBase*> types = env->FindTypes(_path.GetView());

					//There's no situation where multiple can be observed, so it only needs the one.

					auto size = types.size();



					//TODO: VERY temporary idea. No pattern matching, no checking. This is basically the same that we did before
					if (size > 1) {
						report::compile::critical("mulitple types of same name detected.");
						throw nullptr;
					}
					else if (size)
					{
						result = types[0]->CreateNode(env);
						return true;
					}

				}

				return false;
			});


		return result;
	}

	Element* Element::GetElementFromPath(Element* a_this, std::string_view path, ElementType elem, OverloadArgument* sign)
	{
		//I feel like searching for a general element should be reusing this.
		SyntaxRecord path_record;
		
		if (auto result = LEX::Parser__::CreateSyntax<IdentifierParser>(path_record, path); !result){
			//Error here.
			return nullptr;
		}

		//if (path_record.error()) return nullptr;

		
		//<!> After parsing is done, the top most type needs to be set to the standards of what it's expecting.

		//From here, use the path functions that are in environment.
		// The project should be found from the first part, and from there we should just keep the environ search should finish it out.

		switch (elem)
		{
			//Do the search for each type here.

		case kTypeElement:
			//TODO: Types don't count as elements, neither do functions Only environments seem to matter
			report::fault::critical("cant search for types currently");
			return SearchTypePath(a_this, path_record).base;
		case kFuncElement:
		{
			if (!sign) {
				report::failure("Getting function from path requires a signature.");
				return nullptr;
			}

			auto func =  SearchFunctionPath(a_this, path_record, *sign);
			return func ? func.GetBase() : nullptr;
		}
		case kGlobElement:
			return dynamic_cast<GlobalBase*>(SearchFieldPath(a_this, path_record).GetField());
		
		case kScrpElement:
			return SearchScriptPath(a_this, path_record);


		case kNoneElement:
			if (auto elem = GetElementFromPath(a_this, path, kTypeElement))
				return elem;
			if (auto elem = GetElementFromPath(a_this, path, kFuncElement))
				return elem;
			if (auto elem = GetElementFromPath(a_this, path, kGlobElement))
				return elem;
			if (auto elem = GetElementFromPath(a_this, path, kScrpElement))
				return elem;

			return nullptr;

		default:
			report::warn("Invalid element type {} detected.", std::to_underlying(elem));
			return nullptr;
		}
	}


	Environment* Element::GetEnvironmentTMP(Environment* a_this, SyntaxRecord* path, bool& search_scripts)
	{
		Environment* result = nullptr;
		get_switch(path->SYNTAX().type)
		{
		case SyntaxType::Scopename:
		case SyntaxType::Typename:
		{

			auto types = a_this ? a_this->FindTypes(path->GetView()) : std::vector<TypeBase*>{};

			if (types.size() == 1)
				//result = nullptr;
				result = types[0];
			//Script name means nothing basically.

			if (result) {
				search_scripts = false;
				break;
			}
		}

		if (switch_value == SyntaxType::Typename)
			break;

		[[fallthrough]];
		case SyntaxType::Scriptname://This should be script name.
			if (search_scripts)
			{
				result = nullptr;
				//Script name means nothing basically.
				//But here it would search through required or subdirected stuff.

				
			}
			return nullptr;
		}


		return result;
	}


	size_t Element::CheckOverload(OverloadArgument& input, std::vector<FunctionInfo*> clauses, Overload& ret)
	{
		Overload* last = nullptr;

		size_t result = -1;

		for (auto i = 0; i < clauses.size(); i++)
		{
			auto clause = clauses[i];

			Overload buffer;

			if (clause->GetFieldName() == "TestGeneric")
			{
				logger::info("im in");
			}

			auto bias = input.Match(clauses[i], nullptr, buffer, last);
				
			switch (bias)
			{
			case OverloadBias::kAmbiguous:
				result = -1;
				break;

			case OverloadBias::kCurrent:
				ret = std::move(buffer);
				last = &ret;
				result = i;
				break;
			}
		}

		//if (last) {
		//	last->param->ResolveOverload()
		//}

		//if (last)
		//	ret = *last;//this should move

		//return last ? result : -1;
		return result;
	}



	FunctionNode Element::SearchFunctionPath(Element* a_this, SyntaxRecord& path, OverloadArgument& key, Overload& out)
	{
		FunctionNode result;

		SearchPathBase(a_this, path.Transform<SyntaxRecord>(), [&](std::vector<QualifiedName>& query) -> bool
			{
				std::vector<std::pair<size_t, ITemplatePart*>> genericList;

				std::vector<FunctionInfo*> funcs{};
				size_t i = 0;

				for (auto& env : query)
				{
					auto buff = env->FindFunctions(path.GetView());

					i += buff.size();

					//this should compile and then run.
					funcs.insert_range(funcs.end(), buff);


					genericList.emplace_back(std::make_pair(i, env.AsPart()));
				
				}


				if (funcs.size() != 0)
				{
					if (path.GetView() == "size")
					{
						logger::info("registering");
					}

					if (auto index = CheckOverload(key, { funcs.begin(), funcs.end() }, out); index != -1)
					{
						auto info = static_cast<FunctionInfo*>(out.param);

						auto pair = std::find_if(genericList.begin(), genericList.end(), [index](auto& it) {return index < it.first; });

						//Index will be useless right now
						//result = info->CreateNode(pair->second);
						MergeTemplate merger{ pair->second, out };

						//result = info->CreateNode(genericList[0].second);
						result = info->CreateNode(merger);
						return true;
					}

				}



				return false;

				for (int i = 0; i < query.size(); i++)
				{
					auto& env = query[i];

					//this should compile and then run.
					auto funcs = env->FindFunctions(path.GetView());

					//FunctionBase* test = functions[0]->Get();

					if (funcs.size() != 0)
					{
						//Maybe allow it to do this only if the out is to be tossed.
						//if (funcs.size() == 1)
						//{
						//	result = funcs[0];
						//	return true;
						//}

						if (auto index = CheckOverload(key, { funcs.begin(), funcs.end() }, out); index != -1)
						{
							//result = static_cast<FunctionInfo*>(out.clause);

							//Index will be useless right now
							//result->CreateNode(env);
							return true;
						}

						
					}

				}

				return false;
			});


		return result;
	}



	QualifiedField Element::SearchFieldPath(Element* a_this, SyntaxRecord& path)
	{
	
		QualifiedField result{ nullptr };

		SearchPathBase(a_this, path.Transform<SyntaxRecord>(), [&](std::vector<QualifiedName>& query) -> bool
			{
				for (auto env : query)
				{
					//There's no situation where multiple can be observed, so it only needs the one.
					GlobalBase* var = env->FindVariable(path.GetView());

					if (var)
					{
						//possible specialization here.

						//return global;
						result = QualifiedField{ var };

						return true;
					}
					//This isn't needed to qualify this because it doesn't matter what the target is for a global.
					//NULL_OP(key)->GetTarget();
				}

				return false;
			});


		return result;
	}
	
	Script* Element::SearchScriptPath(Element* a_this, SyntaxRecord& path)
	{

		Script* result = nullptr;

		SearchPathBase(a_this, path, [&](std::vector<QualifiedName>& query) -> bool
			{
				for (auto env : query)
				{
					//Has possible cross contamination issues. Like having 2 envs with different names.
					//if (env->GetName() == path.GetView())
					//{
					//	result = static_cast<Script*>(env);
					//	return true;
					//}

					//Later this will handle this a bit differently.
					Script* script = env->GetProject()->FindScript(path.GetView());

					if (script)
					{
						result = script;

						return true;
					}
				}

				return false;
			});


		return result;
	}

	
	bool FindNext(Element*& focus, SyntaxRecord* target, SyntaxRecord*& next)
	{
		//target is left, next is right
		Project* project = focus->GetProject();

		if (project && next->SYNTAX().type == SyntaxType::Path) {
			bool is_path = target->SYNTAX().type == SyntaxType::Path;

			//Ensure that this doesn't use a path.
			if (auto script = project->FindScript(target->GetFront().GetView()); script) {
				focus = script;
				
				
				return true;
			}
		}


		return false;
	}
	
	



	bool Element::HandlePath(Element* focus, SyntaxRecord* rec, std::function<ElementSearch>& func, std::set<Element*>& searched, bool need_associate)
	{
		if (!focus) {
			auto name = ParseUtility::SeekNextPath(rec);

			auto project = ProjectManager::instance->GetProject(name->GetView());

			if (!project)
				return false;

			//I didn't want to do this, but I think this is probably proper.

			name = ParseUtility::SeekNextPath(rec);

			focus = project->FindScript(name->GetView());

			if (!focus)
				return false;

		}
			
		Element* secondary = nullptr;

		RelateType ship = RelateType::Included;

		Environment* env = focus->GetEnvironment();

		Element* target;

		if (!env) {
			//The only time something like this happens is if it's a project. SO, we will have a secondary bit where it will be loaded later, and if
			// it's not null, it will run HandlePath on commons.
			target = focus;
		
			if (need_associate)
				secondary = focus->GetCommons();
		}
		else
		{
			target = env;
		}

		//I'm thinking that the above should be handled before we get here maybe.

		for (; ship != RelateType::None; ship--)
		{


			std::vector<QualifiedName> query = need_associate ? GetEnvironments(target, rec, ship, searched) : std::vector<QualifiedName>{};

			if (env && !need_associate) {
				query.push_back(env);
			}


			bool success = func(query);

			if (success)
				return true;
		}

		if (secondary) {
			return HandlePath(secondary, rec, func, searched, need_associate);
		}


		return false;
	}

	bool Element::SearchPathBase(Element* a_this, SyntaxRecord& rec, std::function<ElementSearch> func)
	{
		//Failure occurs when searching for something with it's script name. Like including otherscript and then searching OtherScript::TestingPull

		SyntaxRecord* path = rec.FindChild(parse_strings::path);

		//Identifier is searched for directly, it won't search up or to it's associates.
		bool is_direct = rec.GetSyntax().type == SyntaxType::Identifier;

		auto first = ParseUtility::PeekCurrentPath(rec);

		

		//Here's how it works, if there is no this element, it will use find. if there is a this element it will differ based on what
		// path is.


		Element* target = a_this;

		//Each of these sets are used to hold data for when "find" is used on a project or when it's regularly used.
		std::set<Element*> full_search{};
		std::set<Element*> find_search{};

		do
		{
			auto _focus = first;


			bool cont = false;

			std::set<Element*>* searched = &full_search;
			//Each find will have something shaved off, so it will use a seperate set.
			//Don't remember how to apply this, but replicate the use of it. I think it's used for whenever we have to find a specific part first.
			//searched = &find_search;

			if (path) {
				//I need to use 
				switch (path->GetSyntax().type)
				{
				case SyntaxType::Path:
					if (!a_this && !is_direct)
						target = ProjectManager::instance->GetShared()->GetCommons();
					break;


					//If any of these happened, it actually is direct.
				case SyntaxType::SpecifyGlobal:
					target = nullptr;
					break;

				case SyntaxType::SpecifyProject:
					target = a_this->GetProject();

					if (!target)
						target = ProjectManager::instance->GetShared();

					break;

				case SyntaxType::SpecifyScript:
					target = a_this->FetchScript();

					if (!target)
						target = ProjectManager::instance->GetShared()->GetCommons();

					break;


				case SyntaxType::SpecifyCommons:
					target = a_this->FetchCommons();

					if (!target)
						target = ProjectManager::instance->GetShared()->GetCommons();


					break;


				case SyntaxType::SpecifyShared:
					target = ProjectManager::instance->GetShared();

					break;
					//case "__type"_ih:
				}
			}
			bool success = a_this->HandlePath(target, path, func, *searched, !is_direct);

			if (success)
				return true;

			if (!is_direct)
			{
					target = target->FetchParent();
			}
			else
			{
				break;
			}

			//if (target)
			//	state--;


			continue;
		}
		while (target);
		//ProjectManager::


		return false;
	}
	
	//*/


	Environment* SecondaryElement::GetEnvironment()
	{
		return _parent;
	}

	Element* SecondaryElement::GetParent()
	{
		return _parent;
	}

	void SecondaryElement::SetParent(Element* par)
	{
		if (_parent)
			return;

		Environment* env = dynamic_cast<Environment*>(par);

		if (!env) {
			report::compile::critical("parent set for Element was not an environment.");
		}

		_parent = env;
	}




}