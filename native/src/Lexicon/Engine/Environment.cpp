#pragma once

#include "Lexicon/Engine/Environment.h"

//*src
//#include "Function.h"

#include "Lexicon/Engine/ConcreteType.h"
#include "Lexicon/Engine/Project.h"

#include "Lexicon/Engine/Component.h"
#include "Lexicon/Engine/SyntaxType.h"
#include "Lexicon/Exception.h"
#include "Lexicon/Engine/Parser.h"
#include "Lexicon/Engine/Element.h"

#include "Lexicon/TypeID.h"
#include "Lexicon/Engine/OverloadInfo.h"
#include "Lexicon/Engine/VariableInfo.h"//For tests only
//*src
#include "Lexicon/Engine/GenericFunction.h"
#include "Lexicon/Engine/ConcreteFunction.h"

#include "Lexicon/Engine/GlobalVariable.h"
#include "Lexicon/Engine/GlobalBase.h"


#include"Lexicon/Engine/ConcreteGlobal.h"
#include "Lexicon/Engine/GenericType.h"


#include "Lexicon/Engine/OverloadInput.h"

#include "Lexicon/Engine/QualifiedField.h"

#include "Lexicon/Interfaces/ProjectManager.h"

#include "Lexicon/Engine/TypeBase.h"

#include "Lexicon/Engine/parse_strings.h"

#include "Lexicon/Engine/Script.h"

#include "Lexicon/Interfaces/ProjectClient.h"

//SHOULD_NATIVE
#include "Lexicon/Engine/VariableInfo.h"


namespace LEX
{

	template <typename T>
	TypeBase* _PolicyMaker(std::string name, TypeOffset offset)
	{//helps with generic or concrete divide
		return new T{ std::string_view{name}, offset };
	}

	TypeOffset RecordToInt(SyntaxRecord& ast)
	{//helps with generic or concrete divide
		std::string tag = ast.GetTag();

		if (std::strncmp(tag.c_str(), "0x", 2) == 0 || std::strncmp(tag.c_str(), "0X", 2) == 0)
		{
			return std::stoi(tag, nullptr, 16);
		}
		else
		{
			return std::stoi(tag, nullptr, 10);
		}
	}





	Environment::~Environment()
	{
		//Destroy all info data.
	}


		void Environment::AddFunction(FunctionBase* tar)
		{
			if (!tar) {
				report::compile::error("Non - FunctionData IFunction attempted to be added");
				throw nullptr;
			}

			auto end = functions.end();

			auto name = tar->GetName();

			//if (auto it = functionMap.find(name); end != it) {
			//	report::compile::critical("Non - FunctionData IFunction attempted to be added");
			//	throw nullptr;
			//}
			//else {
				auto& info = functions[name].emplace_back(tar);
				DeclareParentTo(tar);
			//}

		}


		void Environment::AddVariable(GlobalBase* tar)
		{
			//This won't have an issue yet, because no abstraction.

			auto end = variables.end();

			auto name = tar->GetName();

			if (auto it = std::find_if(variables.begin(), end, [&](auto& i) {return name == i.first; }); end != it) {
				report::compile::error("Variable {} already existed", name);
			}
			else {
				variables[name].reset(tar->AsInfo());
				DeclareParentTo(tar);
			}
		}

		std::vector<OverloadInfo*> Environment::FindFunctions(const std::string_view& name)
		{
			//std::vector<FunctionInfo*> result{};

			auto end = functions.end();

			//TODO: FindFunctions is busted because I need to LoadFromRecord for a name but needs to be added to Load.
			if (auto it = functions.find(name); end != it) {
			//if (auto it = std::find_if(functionMap.begin(), functionMap.end(), [&](auto i) { return name == i.second.Get()->GetName(); }); end != it) {
				return reinterpret_cast<const std::vector<OverloadInfo*>&>(it->second);
			}
			
			return {};
		}




		
		std::vector<VarInfo*> Environment::FindVariables(const std::string_view& name)
		{
			
			auto end = variables.end();

			if (auto it = std::find_if(variables.begin(), end, [&](auto& i) {return name == i.first; }); 
				end != it) {
				return { it->second.get() };
			}
			else {
				return {};
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


		Directory* Environment::FindDirectory(SyntaxRecord& record, ITemplateInserter* inserter)
		{
			auto types = FindTypes(record.GetView());

			if (types.size() == 1)
				//result = nullptr;
				return types[0];

			return nullptr;
		}







		Environment* Environment::GetEnvironmentImpl()
		{
			return this;
		}

		//source file type shit
		Directory* Environment::GetParentImpl()
		{
			return _parent;
		}



		void Environment::SetParent(Directory* par)
		{
			if (_parent)
				return;
			//I'd like something easier for this
			//Perhaps fetch project and 

			//Environment* env = dynamic_cast<Environment*>(par);
			//Project* proj = dynamic_cast<Project*>(par);

			//if (par->FetchProject() != par && par->FetchEnvironment() != par) {
			if (!dynamic_cast<Project*>(par) && !dynamic_cast<Environment*>(par)) {
				report::fault::critical("parent set for Element was not an environment.");
				throw nullptr;
			}

			_parent = par;
		}

		void Environment::AddType(TypeBase* policy)
		{
			if (!policy) {
				report::compile::error("Null Policy attempted to be added");
			}

			auto end = typeMap.end();

			auto name = policy->GetName();

			if (auto it = typeMap.find(name); end != it) {
				report::compile::error("Type already exists {}.", name);;
			}
			else {
				report::compile::trace("type {} added to {}", name, GetName());
				typeMap[name] = policy;


				//if (policy->GetDataType() == DataType::Attribute && policy->GetName().ends_with("Attribute") == true) {}

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


		TypeBase* Environment::tempObtainPolicy(SyntaxRecord& ast, Element* parent)
		{
			SyntaxRecord& settings = ast.GetChild(0);




			//This part can be done on the policy, what really needs to be done is figuring out which policy to create, 
			// or to create at all. This relies on 1 setting. The rest can be fed verbatum later.

			//NOTE, this also includes template arguments.


			//Rules of obtaining
			//Intrinsic, no creation, just pull a policy. Doesn't matter what else it is.
			//ISpecial-Creates type policy from specific string and integer. Link error if not found.
			//Data, Creates TypePolicy plain, claiming the next free space.
			//Generic, a different TypePolicy has to be used, but otherwise it's fine.


			SyntaxRecord* genericSet = settings.FindChild(parse_strings::generic);
			bool is_generic = parent && parent->IsGenericElement() || genericSet && genericSet->size();


			using PolicyCtor = TypeBase*(std::string, TypeOffset);

			//using ConcreteType = ConcreteType;
			using GenericPolicy = ConcreteType;
			PolicyCtor* create_func = !is_generic ? _PolicyMaker<ConcreteType> : _PolicyMaker<GenericType>;




			std::string name;
			TypeOffset offset;

			auto LookUpOrMake = [&](const std::string_view& name, TypeOffset offset, bool lookup) -> TypeBase*
				{
					TypeBase* result = nullptr;

					if (lookup) {

						result = IdentityManager::instance->GetBaseByOffset(name, offset);
					}
					else
						result = is_generic ? static_cast<TypeBase*>(new GenericType{ name, offset }) : new ConcreteType{ name, offset };

					return result;
				};

			bool lookup = false;

			TypeBase* result;

			if (auto attach = ast.FindChild(parse_strings::settings)->FindChild(parse_strings::attach); attach)
			{

				if (attach->size() == 0) {
					attach->critical("external type requires some type.");
				}

				SyntaxRecord& attach_data = attach->GetFront();
				switch (Hash(attach_data.GetTag()))
				{
				case "intrinsic"_h:
					//Look up
					lookup = true;
					__fallthrough;
				case "external"_h:
				{
					//Handle error, I can't fucking be bothered.
					SyntaxRecord& category = attach_data.GetFront();
					TypeOffset offset;

					//this should more be if it's not number.
					if (auto& args = category.GetFront(); args.GetView() == "args")
					{
						auto& children = args.children();

						std::vector<std::string_view> string_args{ children.size() };

						std::transform(children.begin(), children.end(), string_args.begin(), [](SyntaxRecord& it) { return it.GetView(); });


						offset = IdentityManager::instance->GetTypeOffsetFromArgs(category.GetView(), string_args);
					}
					else
					{
						offset = RecordToInt(category.GetFront());
					}

					result = LookUpOrMake(category.GetTag(), offset, lookup);

					break;

				}
				break;//create

				default:
					report::apply::debug("Couldn't ObtainPolicy");
					result = nullptr;
					break;
				}

			}
			else
			{
				result = is_generic ? new GenericPolicy{} : new ConcreteType{};
			}

			if (result)
				result->Initialize(ast);

			return result;
		}


		void Environment::LoadFromSyntaxNode(SyntaxRecord& node)
		{
			get_switch(node.SYNTAX().type)
			{
			case SyntaxType::Function:
			{
				//auto* function = new ConcreteFunction{};

				//AddFunction(function);

				//function->ConstructFromRecord(node);
				//AddFunction(Component::Create<ConcreteFunction>(node));

				CreateFunction(node);
				break;

			}
			case SyntaxType::Type:
			{
				AddType(tempObtainPolicy(node));
				break;
			}
			case SyntaxType::Variable:
			{
				//This is very incorrect btw
				AddVariable(Component::Create<ConcreteGlobal>(node));
				break;
			}
			
			default:
				report::compile::critical("Syntax {} not valid for environment", magic_enum::enum_name(switch_value)); break;
			}
		}

		void Environment::LoadFromSyntaxTree(SyntaxRecord::Iterator begin, SyntaxRecord::Iterator end)
		{
			//for (auto& node : target.children())
			while (begin != end)
			{
				LoadFromSyntaxNode(*begin++);
			}
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