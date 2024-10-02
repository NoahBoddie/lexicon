#include "Lexicon/Engine/Element.h"


#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"
#include "Lexicon/Interfaces/ProjectManager.h"
#include "Lexicon/Engine/Environment.h"

#include "Lexicon/Engine/PolicyBase.h"
#include "Lexicon/Engine/ParserTest.h"
#include "Lexicon/Engine/FunctionInfo.h"
#include "Lexicon/Engine/GlobalBase.h"
#include "Lexicon/Engine/SyntaxRecord.h"
namespace LEX
{

	IScript* Element::GetScriptI()
	{
		return GetParent()->FetchScript();
	}

	IProject* Element::GetProjectI()
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

	SyntaxRecord& Element::GetPath(Record& path, std::optional<bool> right)
	{
		Record* ret = nullptr;
			
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


	

	Environment* Element::WalkEnvironmentPath(Environment* a_this, SyntaxRecord*& path, bool search_scripts)
	{
		//If record is empty afterwords, this means that it has completed iteration.
		if (path->IsPath() == false) {
			path = nullptr;
			return a_this;
		}
		auto left = path->FindChild(parse_strings::lhs);

		if (!left) {

			auto result = GetEnvironmentTMP(a_this, &path->Demote(), search_scripts);

			path = nullptr;

			return result;
		}

		if constexpr (1 == 1)//If not generic.
		{
			Environment* _this = GetEnvironmentTMP(a_this, &left->Demote(), search_scripts);

			path = path->FindChild(parse_strings::rhs);

			return WalkEnvironmentPath(a_this, path, search_scripts);
		}
		else
		{

		}
	}

	Environment* Element::SpecializeEnvironments(std::vector<Environment*>& generics)
	{
		//Specialize environment brings everything down to a single point.
		return {};
	}

	std::vector<Environment*> Element::GetEnvironments(Element* a_this, SyntaxRecord* step, RelateType a, std::set<Element*>& searched)
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

		std::vector<Environment*> result{ };

		std::vector<Environment*> out{ a_this->GetEnvironment() };

		//maybe there's a better way to do this, but whatever innit.
		while (out.size() != 0)//Overloop
		{
			std::vector<Environment*> buffer{};


			for (auto env : out)
			{
				if (!env || !searched.emplace(env).second) {
					continue;
				}
				buffer.emplace_back(env);

				if (a != RelateType::None)
					buffer.insert_range(buffer.end(), env->GetAssociates(a));
			}
			result.insert_range(result.end(), buffer);
			out = std::move(buffer);
		}

		bool specialize = false;

		if (step)
		{
			//out.clear();
			//out.reserve(result.size());

			for (auto& env : result)
			{
				auto _step = step;
				env = WalkEnvironmentPath(env, _step);

				if (_step)
					specialize = true;
			}

			result.erase(std::remove(std::begin(result), std::end(result), nullptr), std::end(result));
		}

		if (specialize) {
			auto new_this = SpecializeEnvironments(result);

			if (new_this)
				return GetEnvironments(new_this, step, a, searched);

			return {};
		}
		else
		{
			return result;
		}
	}


	bool Element::HandlePath(Element* focus, SyntaxRecord* rec, std::function<ElementSearch>& func, std::set<Element*>& searched, bool need_associate)
	{
		if (!focus)
			return false;


		RelateType ship = RelateType::Included;

		Environment* target = focus->GetEnvironment();

		if (focus != target) {
			target = focus->GetCommons();
		}


		for (; ship != RelateType::None; ship--)
		{


			std::vector<Environment*> query = need_associate ? GetEnvironments(target, rec, ship, searched) : std::vector<Environment*>{ target };


			bool success = func(query);

			if (success)
				return true;
		}

		return false;
	}




	PolicyBase* Element::SearchTypePath(Element* a_this, Record& _path)
	{
		PolicyBase* result = nullptr;

		SearchPathBase(a_this, _path.Transform<SyntaxRecord>(), [&](std::vector<Environment*>& query) -> bool
			{
				for (auto env : query)
				{

					std::vector<PolicyBase*> types = env->FindTypes(_path.GetView());

					//There's no situation where multiple can be observed, so it only needs the one.

					auto size = types.size();



					//TODO: VERY temporary idea. No pattern matching, no checking. This is basically the same that we did before
					if (size > 1) {
						report::compile::critical("mulitple types of same name detected.");
						throw nullptr;
					}
					else if (size)
					{
						if (!result)
						{
							result = types[0];
						}
						else
						{
							report::error("path no work. Make better error later");
						}

						return true;
					}

				}

				return false;
			});


		return result;
	}

	Element* Element::GetElementFromPath(Element* a_this, std::string_view path, ElementType elem)
	{
		Record path_record = LEX::Impl::Parser__::CreateSyntax<Impl::IdentifierParser>(std::string{ path });

		//if (path_record.error()) return nullptr;

		report::warn("I'm not checking here, I should be checking :)");
		//<!> After parsing is done, the top most type needs to be set to the standards of what it's expecting.

		//From here, use the path functions that are in environment.
		// The project should be found from the first part, and from there we should just keep the environ search should finish it out.

		switch (elem)
		{
			//Do the search for each type here.

		case kTypeElement:
			return SearchTypePath(a_this, path_record);
		case kFuncElement:
		{
			auto func =  SearchFunctionPath(a_this, path_record);
			return func ? func->Get() : nullptr;
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


	Environment* Element::GetEnvironmentTMP(Environment* a_this, Record* path, bool& search_scripts)
	{
		Environment* result = nullptr;
		get_switch(path->SYNTAX().type)
		{
		case SyntaxType::Scopename:
		case SyntaxType::Typename:
		{

			auto types = a_this ? a_this->FindTypes(path->GetView()) : std::vector<PolicyBase*>{};

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


	bool Element::CheckOverload(OverloadKey& input, std::vector<FunctionInfo*> clauses, Overload& ret)
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

			out = std::move(overload);

			last = &out;
		}

		if (last)
			ret = *last;//this should move

		return last;
	}



	FunctionInfo* Element::SearchFunctionPath(Element* a_this, Record& path, OverloadKey* key, Overload* out)
	{
		FunctionInfo* result = nullptr;

		SearchPathBase(a_this, path.Transform<SyntaxRecord>(), [&](std::vector<Environment*>& query) -> bool
			{
				//std::verctor


				for (auto env : query)
				{
					//this should compile and then run.
					auto funcs = env->FindFunctions(path.GetView());

					//FunctionBase* test = functions[0]->Get();

					if (funcs.size() != 0)
					{
						if (key && out)
						{
							if (CheckOverload(*key, { funcs.begin(), funcs.end() }, *out) == true)
							{
								result = dynamic_cast<FunctionInfo*>(out->clause);
								return true;
							}
						}
						else if (funcs.size() == 1)
						{
							result = funcs[0];
							return true;
						}

						
					}

				}

				return false;
			});


		return result;
	}



	QualifiedField Element::SearchFieldPath(Element* a_this, Record& path)
	{
	
		QualifiedField result{ nullptr };

		SearchPathBase(a_this, path.Transform<SyntaxRecord>(), [&](std::vector<Environment*>& query) -> bool
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
	
	Script* Element::SearchScriptPath(Element* a_this, Record& path)
	{

		Script* result = nullptr;

		SearchPathBase(a_this, path.Transform<SyntaxRecord>(), [&](std::vector<Environment*>& query) -> bool
			{
				for (auto env : query)
				{
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

	
	bool FindNext(Element*& focus, Record* target, Record*& next)
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
	
	
	
	bool Element::SearchPathBase(Element* a_this, SyntaxRecord& rec, std::function<ElementSearch> func)
	{
		//Failure occurs when searching for something with it's script name. Like including otherscript and then searching OtherScript::TestingPull

		SyntaxRecord* path = rec.FindChild(parse_strings::path);

		//Identifier is searched for directly, it won't search up or to it's associates.
		bool is_direct = rec.GetSyntax().type == SyntaxType::Identifier;

		auto left = path ? path->FindChild(parse_strings::lhs) : nullptr;
		auto right = left ? path->FindChild(parse_strings::rhs) : nullptr;



		//I'm considering not using these.
		//auto first = left ?
		//	&GetPath(*left) : path ?
		//	&path->GetFront() : nullptr;

		//This is what should be submitted to functions
		//auto next = right ?
		//	&GetPath(*right, false) : path ?
		//	&path->GetFront() : &rec;


		auto first = left ? left : path;

		//If right exists then it's right. If it doesn't, theres only one path and thus 
		auto next = right ? right : nullptr;

		enum ProjectState
		{
			kNone,
			kFindProject,
			kFindShared,
			kGetShared,
			kFindCurrent,
			k_GetCurrent,
		} state = !a_this ? kFindProject : k_GetCurrent;//Force to be find depending on what heads path.

		if (first && state == k_GetCurrent)
		{
			switch (first->GetFront().GetSyntax().type)
			{
			case SyntaxType::ProjectName:
				state = kFindProject;
				break;
			case SyntaxType::Scriptname:
				state = kFindCurrent;
				break;
			}
		}
		
		//Here's how it works, if there is no this element, it will use find. if there is a this element it will differ based on what
		// path is.


		Element* target = a_this;

		//Each of these sets are used to hold data for when "find" is used on a project or when it's regularly used.
		std::set<Element*> full_search{};
		std::set<Element*> find_search{};

		while (state != kNone)
		{
			auto _focus = first;

			
			bool cont = false;

			std::set<Element*>* searched = &full_search;

			if (state != k_GetCurrent)
			{
				Project* project;
				if (path)
				{
					switch (state)
					{

					case kGetShared://Should get shared commons and search it
						target = ProjectManager::instance->GetShared();
						break;

					//case kGetCurrent://Should get current commons and search it
						//target = a_this->FetchCommons();
						//break;


					case kFindShared://Should get shared, then search
						project = ProjectManager::instance->GetShared();
						goto find;

					case kFindCurrent://Should get current project, then search
						project = a_this->FetchProject();

						goto find;

					case kFindProject:// should find project and search for script.
						project = ProjectManager::instance->GetProject(first->GetFront().GetView());


						_focus = next;

						if (!next || next->GetSyntax().type != SyntaxType::Path) {
							cont = true;
							break;
						}

						//next = &GetPath(*next, true);
						next = next->FindChild(parse_strings::rhs);
						
						//Thie final find project will have an even more shaved off, thus it should use a completely clear one.
						find_search.clear();

					find:
						//Each find will have something shaved off, so it will use a seperate set.
						searched = &find_search;

						if (project) {

							auto& tar = GetPath(*_focus, false);


							if (auto script = project->FindScript(tar.GetView()); script) {
								target = script;
								_focus = next;
								break;
							}
							else {
								cont = true;
							}
						}
						break;
					}
				}
				else
				{
					cont = true;
				}
			}

			if (!cont && target)
			{



				bool success = HandlePath(target, _focus, func, *searched, !is_direct);

				if (success)
					return true;

			}
			
			if (!is_direct)
			{
				if (state == k_GetCurrent)
					target = target->FetchParent();

				if (state != k_GetCurrent || !target) {
					state--;
				}
			}
			else
			{
				break;
			}

			//if (target)
			//	state--;
			

			continue;
		}

		//ProjectManager::


		return false;
	}
	//*/


	IEnvironment* SecondaryElement::GetEnvironmentI()
	{
		return _parent;
	}

	IElement* SecondaryElement::GetParentI()
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