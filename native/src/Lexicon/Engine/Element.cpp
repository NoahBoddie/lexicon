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

	SyntaxRecord& Element::GetPath(Record& path, bool right)
	{
		Record* ret = path.FindChild(right ? parse_strings::rhs : parse_strings::rhs);

		if (!ret)
			ret = &path.GetFront();

		return SyntaxRecord::To(*ret);
	}



	std::vector<Environment*> Element::GetAssociates(Element* a_this, RelateType)
	{
		return {};
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

			auto result = GetEnvironmentTMP(a_this, &path->GetFront(), search_scripts);

			path = nullptr;

			return result;
		}

		if constexpr (1 == 1)//If not generic.
		{
			Environment* _this = GetEnvironmentTMP(a_this, left, search_scripts);

			path = SyntaxRecord::To(path->FindChild(parse_strings::rhs));

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
					buffer.insert_range(buffer.end(), GetAssociates(env, a));
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

		SearchPathBase(a_this, SyntaxRecord::To(_path), [&](std::vector<Environment*>& query) -> bool
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

		case kNoneElement:
			if (auto elem = GetElementFromPath(a_this, path, kTypeElement))
				return elem;
			if (auto elem = GetElementFromPath(a_this, path, kFuncElement))
				return elem;
			if (auto elem = GetElementFromPath(a_this, path, kGlobElement))
				return elem;

			return nullptr;

		default:
			report::warn("Invalid element type {} detected.", std::to_underlying(elem));
			return nullptr;
		}
	}


	Environment* Element::GetEnvironmentTMP(Environment* a_this, Record* path, bool& search_scripts)
	{
		Environment* result;
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

				break;
			}

		default:
			result = nullptr; break;
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

	FunctionInfo* Element::SearchFunctionPath(Element* a_this, Record& path, OverloadKey* key, Overload* out)
	{
		FunctionInfo* result = nullptr;

		SearchPathBase(a_this, SyntaxRecord::To(path), [&](std::vector<Environment*>& query) -> bool
			{

				for (auto env : query)
				{
					//this should compile and then run.
					auto funcs = env->FindFunctions(path.GetView());

					//FunctionBase* test = functions[0]->Get();

					if (funcs.size() != 0)
					{
						if (key && out && CheckOverload(*key, { funcs.begin(), funcs.end() }, *out) == true)
						{
							result = dynamic_cast<FunctionInfo*>(out->clause);
						}
						else// if (funcs.size() == 1)
						{
							result = funcs[0];
						}

						return true;
					}

				}

				return false;
			});


		return result;
	}


	QualifiedField Element::SearchFieldPath(Element* a_this, Record& path)
	{
	
		QualifiedField result{ nullptr };

		SearchPathBase(a_this, SyntaxRecord::To(path), [&](std::vector<Environment*>& query) -> bool
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


	bool Element::SearchPathBase(Element* a_this, SyntaxRecord& rec, std::function<ElementSearch> func)
	{
		//Needs a function to handle these.
		/*
		this element
		up element until none (loop)
		Search from project
		Move onto commons
		move onto shared commons
		search for script in current project
		search for project
		//*/

		//So there has to be something that prevents it from using the main one, notably if someone is incredibly specific about which version they're using.

		//This needs to be proper at some point.
		SyntaxRecord* path = SyntaxRecord::To(rec.FindChild(parse_strings::path));

		//Identifier is searched for directly, it won't search up or to it's associates.
		bool is_direct = rec.GetSyntax().type == SyntaxType::Identifier;

		auto left = path ? path->FindChild(parse_strings::lhs) : nullptr;
		auto right = left ? path->FindChild(parse_strings::rhs) : nullptr;

		auto first = left ?
			&GetPath(*left, false) : path ?
			&path->GetFront() : nullptr;

		//This is what should be submitted to functions
		auto next = right ?
			&GetPath(*right, true) : path ?
			&path->GetFront() : &rec;



		Element* target = a_this;

		std::set<Element*> searched{};

		bool is_shared = true;

		
		//Move this out, I'd like a way to force project state. What it is would be determined if it's run on an element or not.
		// So that actually means this version of the function stays, to make it less confusing.
		// or is getting moved to the project manager
		enum ProjectState
		{
			kNone,
			kFind,
			kShared,
			kCurrent,
		} state = !a_this ? kFind : kNone;//Force to be find depending on what heads path.

		if (path && state == kNone)
		{
			switch (first->SYNTAX().type)
			{
			case SyntaxType::ProjectName:
				state = kFind;
				break;
			case SyntaxType::Scriptname:
				target = nullptr;
				[[fallthrough]];
			default:
				state = kCurrent;
				break;
			}
		}

		//Here's how it works, if there is no this element, it will use find. if there is a this element it will differ based on what
		// path is.


		while (target || state != kNone)
		{
			auto _next = next;

			if (!target)// && left) //Must have a left side
			{
				Project* project = nullptr;

				Record* tar = left ? left : &path->GetFront();

				switch (state)
				{
				case kCurrent:
					project = a_this->GetProject();
					goto _find;

				case kShared:
					project = ProjectManager::instance->GetShared();
					goto _find;

				case kFind:
					//Find is when project has to be found first, so move the finding of the script down here.
					if (!right)
						break;

					project = ProjectManager::instance->GetProject(tar->GetFront().GetView());

					tar = &GetPath(*right, false);
					//Revert to this to test for a crash
					//_next->GetView() == parse_strings::rhs || _next->GetView() == parse_strings::lhs
					if (_next->SYNTAX().type != SyntaxType::Path) {
						_next = nullptr;
					}



					if (!tar)
					{
						return false;
						state = kNone;
						//If target is no longer able to be found.
						//We break, nothing to do.
						break;
					}
					//_next = nullptr;//Unsure how to deal with this bit.
				_find:
					//state--;
					if (project) {
						bool is_path = tar->SYNTAX().type == SyntaxType::Path;

						if (auto script = project->FindScript(is_path ? tar->GetFront().GetView() : tar->GetView()); script) {
							target = script;
							break;
						}
					}

				case kNone:
					break;
				}
			}


			bool success = HandlePath(target, SyntaxRecord::To(_next), func, searched, !is_direct);

			if (success)
				return true;

			if (!is_direct)
			{
				if (state == kCurrent)
					target = target->FetchParent();

				if (!target)
					state--;
			}
			else
			{
				break;
			}
		}

		//ProjectManager::


		return false;
	}



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