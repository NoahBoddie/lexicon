#include "Lexicon/Interfaces/DirectoryManager.h"

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

#include "Lexicon/Engine/ProjectDirectory.h"

namespace LEX
{
	

	//Hash of file string and the signature that was used to recieve it.
	//std::unordered_map<std::pair<size_t, size_t>, IComponent*> _lookupMap;
	//std::unordered_map<std::string_view, IDirectory*> _directoryMap;



	//the associated should maybe be a bool or just reject any other than include and import.





	//*/



	/*
	namespace NEW
	{
		namespace _UNDER
		{
#ifndef DISABLE_UNDER

			SyntaxRecord& GetPath(SyntaxRecord& path, std::optional<bool> right)
			{
				SyntaxRecord* ret = nullptr;

				if (right.has_value())
					path.FindChild(right.value() ? parse_strings::rhs : parse_strings::lhs);

				if (!ret)
					ret = &path.GetFront();
				
				static_assert(derived_record<SyntaxRecord>);
				static_assert(std::derived_from <SyntaxRecord, RecordBase>);
				static_assert(sizeof(BasicRecord<Syntax>) == sizeof(RecordBase));
				constexpr auto size = sizeof(SyntaxRecord);

				return ret->Transform<SyntaxRecord>();
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

#endif
		}


		//For this bit, I need to use the interface types, if you don't mind. Because a concrete
		// directory might not always be on the other side (IE, anything generic).
		// Here's the thing about that though, those types won't really remain specialized so 
		// I'm unsure if it matters.



		Directory* WalkDirectoryPath(Directory* focus, SyntaxRecord* path, ITemplateInserter& inserter)
		{
			while (path && path->IsPath() == true)
			{
				//if (path->IsPath() == false) {
				//	path = nullptr;
				//	return FetchEnvironment();
				//}

				if (!focus)
					return nullptr;

				auto below = ParseUtility::SeekNextPath(path);

				focus = focus->FindDirectory(*below, &inserter);
			}

			return focus;

			//Must this be an environ?
			//return NULL_OP(NULL_Q(a_this)->GetEnvironment());

		}


		std::vector<SpecialDirectory> GetDirectories(Element* a_this, Directory* focus, SyntaxRecord* step, RelateType& relation, std::set<Element*>& searched)
		{
			//This could be cleaner, but it works for now.

			std::vector<SpecialDirectory> result{};
			

			std::vector<Directory*> out{ focus };

			Script* script = focus->As<Script>();

			//No need to loop if we aren't doing a script.
			if (!script)
				relation = RelateType::None;
			//maybe there's a better way to do this, but whatever innit.
			while (out.size() != 0)//Overloop
			{


				std::vector<Directory*> buffer{};

				for (Directory* dir : out)
				{
					if (!dir || !searched.emplace(dir).second) {
						continue;
					}

					GenericArray inserter{ NULL_OP(NULL_Q(a_this)->AsGenericElement()), };
					
					if (step)
						dir = WalkDirectoryPath(dir, step, inserter);
					
					if (dir)
						result.emplace_back(dir, std::move(inserter));

					if (script && relation != RelateType::None) {
						buffer.insert_range(buffer.end(), script->GetAssociates(relation));
					}
						
				}

				out = std::move(buffer);
			}


			return result;
		}





		using DirectorySearch = bool(std::vector<SpecialDirectory>&);

		//I no longer have to do this, instead I can just make qualified name use directory.
		using DirectorySearchFn = std::function<DirectorySearch>;




		bool HandlePath(Element* a_this, Element* focus, SyntaxRecord* rec, const DirectorySearchFn& func, std::set<Element*>& searched, bool need_associate)
		{
			if (!focus)
				return false;



			RelateType ship = need_associate ? RelateType::Included : RelateType::None;
			
			Directory* dir = focus->GetDirectory();

			if (dir) {

				do
				{
					//std::vector<QualifiedName> query = need_associate ? GetEnvironments(target, rec, ship, searched) : std::vector<QualifiedName>{};
					std::vector<SpecialDirectory> query = GetDirectories(a_this, dir, rec, ship, searched);

					if (rec) {
						query.push_back(dir);
					}

					//if (env && !need_associate) {
						//query.push_back(env);
					//}

					bool success;

					success = func(query);

					if (success)
						return true;

				} while (ship-- != RelateType::None);

				if (Project* project = dir->As<Project>()) {
					return HandlePath(a_this, project->GetCommons(), rec, func, searched, need_associate);
				}

			}

			return false;
		}



		bool SearchPathBase(Element* a_this, SyntaxRecord& rec, const DirectorySearchFn& func)
		{

			SyntaxRecord* path = rec.FindChild(parse_strings::path);

			bool is_direct = rec.GetSyntax().type == SyntaxType::Identifier;

			auto first = ParseUtility::PeekCurrentPath(rec);



			Element* target = a_this ? a_this : ProjectDirectory::GetSingleton();


			std::set<Element*> searched{};

			do
			{
				auto _focus = first;


				bool cont = false;

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
						target = NULL_OP(NULL_Q(a_this)->GetScript());

						if (!target)
							target = ProjectManager::instance->GetShared()->GetCommons();

						break;


					case SyntaxType::SpecifyCommons:
						target = NULL_OP(NULL_Q(a_this)->GetCommons());

						if (!target)
							target = ProjectManager::instance->GetShared()->GetCommons();


						break;


					case SyntaxType::SpecifyShared:
						target = ProjectManager::instance->GetShared();

						break;
						//case "__type"_ih:
					}
				}

				if (target)
				{
					bool success = HandlePath(a_this, target, path, func, searched, !is_direct);

					if (success)
						return true;
				}

				if (is_direct)
					break;

				target = NULL_OP(NULL_Q(target)->GetParent());
			}
			while (target);


			return false;
		}






		TypeNode SearchTypePath(Element* a_this, SyntaxRecord& _path)
		{
			if (a_this && _path.FindChild(parse_strings::path) == nullptr)
			{
				if (auto gen_elem = a_this->AsGenericElement())
				{
					if (auto temp = gen_elem->GetTemplateByName(_path.GetView())) {
						return TypeNode{ nullptr, temp };
					}
				}
			}



			TypeNode result;

			SearchPathBase(a_this, _path, [&](std::vector<SpecialDirectory>& query) -> bool
				{
					for (auto& dir : query)
					{
						if (auto env = dir->As<Environment>())
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
								result = types[0]->CreateNode(dir);
								return true;
							}
						}

						

					}

					return false;
				});


			return result;
		}


		size_t CheckOverload(OverloadArgument& input, std::vector<FunctionInfo*> clauses, Overload& ret)
		{
			Overload* last = nullptr;

			size_t result = -1;

			for (auto i = 0; i < clauses.size(); i++)
			{
				auto clause = clauses[i];

				Overload buffer;

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



		FunctionNode SearchFunctionPath(Element* a_this, SyntaxRecord& path, OverloadArgument& key, Overload& out)
		{
			FunctionNode result;

			SearchPathBase(a_this, path.Transform<SyntaxRecord>(), [&](std::vector<SpecialDirectory>& query) -> bool
				{
					std::vector<std::pair<size_t, ITemplatePart*>> genericList;

					std::vector<FunctionInfo*> funcs{};
					size_t i = 0;

					for (auto& dir : query)
					{
						if (auto env = dir->As<Environment>())
						{
							auto buff = env->FindFunctions(path.GetView());

							//TODO: I think I'd actually just not have this work so something not working wouldn't disrupt the expected order of the objects

							auto it = std::remove_if(buff.begin(), buff.end(), [](FunctionInfo* other) {return !other->IsValid(); });
							if (auto end = buff.end(); end != it) {
								//Testing with the idea of just not having this work full stop so it doesn't change the intent of the code when compiling.
								return true;
								buff.erase(it, end);

							}

							i += buff.size();

							//this should compile and then run.
							funcs.insert_range(funcs.end(), buff);


							genericList.emplace_back(std::make_pair(i, dir.AsPart()));
						}
					}


					if (funcs.size() != 0)
					{

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
				});


			return result;
		}

		FunctionNode SearchFunctionPath(Element* a_this, SyntaxRecord& path, OverloadArgument& key)
		{
			Overload out{};
			auto result = SearchFunctionPath(a_this, path, key, out);
			return result;
		}

		QualifiedField SearchFieldPath(Element* a_this, SyntaxRecord& path)
		{

			QualifiedField result{ nullptr };

			SearchPathBase(a_this, path.Transform<SyntaxRecord>(), [&](std::vector<SpecialDirectory>& query) -> bool
				{
					for (auto dir : query)
					{
						if (auto env = dir->As<Environment>())
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
					}

					return false;
				});


			return result;
		}

		Script* SearchScriptPath(Element* a_this, SyntaxRecord& path)
		{
			Script* result = nullptr;

			SearchPathBase(a_this, path, [&](std::vector<SpecialDirectory>& query) -> bool
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


		namespace Detail
		{

		}


		bool SearchDirectory(std::vector<SpecialDirectory>& query, ComponentType type)
		{
			switch (type)
			{
				//ComponentType::
			}

			return false;
		}


		Element* GetElementFromPath(Element* a_this, std::string_view path, ElementType elem, OverloadArgument* sign = nullptr)
		{
			//I feel like searching for a general element should be reusing this.
			SyntaxRecord path_record;

			if (elem != ElementType::kTypeElement) {
				if (auto result = LEX::Parser::CreateSyntax<IdentifierParser>(path_record, path); !result) {
					//Error here.
					return nullptr;
				}
			}
			else {
				if (auto result = LEX::Parser::CreateSyntax<HeaderParser>(path_record, path); !result) {
					//Error here.
					return nullptr;
				}

				if (path_record.GetChild(KeywordType::TypeQual).size() || path_record.GetChild(KeywordType::DeclSpec).size()) {
					report::warn("type qualifiers and declaration specifiers are ignored. ({})", path);
				}
				if (auto& spec = path_record.GetChild(KeywordType::TypeSpec); spec.size() == 0) {
					return nullptr;
				}
				else {
					path_record = spec;
				}
			}
			//if (path_record.error()) return nullptr;


			//<!> After parsing is done, the top most type needs to be set to the standards of what it's expecting.

			//From here, use the path functions that are in environment.
			// The project should be found from the first part, and from there we should just keep the environ search should finish it out.



			//TODO: Nothing specializable will ever work on generics like this, so I'm going to just overhaul this at some point.


			switch (elem)
			{
				//Do the search for each type here.

			case kTypeElement:
				//report::fault::critical("cant search for types currently");
				return dynamic_cast<Element*>(GetPolicyFromSpecifiers(path_record, a_this));

			case kFuncElement:
			{
				if (!sign) {
					report::failure("Getting function from path requires a signature.");
					return nullptr;
				}

				auto func = SearchFunctionPath(a_this, path_record, *sign);
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

	}
	//*/
}