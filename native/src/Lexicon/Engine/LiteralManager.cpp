#include "Lexicon/Engine/LiteralManager.h"

#include "Lexicon/Engine/Solution.h"
#include "Lexicon/Engine/Declaration.h"

namespace LEX
{

	//TODO: This method of handling literals as is will cause the assert to be hit because all the data destructs
	std::set<Literal> literalList{};



	std::pair<Solution, ITypeInfo*> LiteralManager::ObtainLiteral(SyntaxRecord& ast, Element* elem)
	{

		SyntaxType syn = ast.GetSyntax().type;
		Literal literal{ syn == SyntaxType::Object };
		ITypeInfo* solution = nullptr;
		ITypeInfo* type = nullptr;


		uint8_t free_value = 0;

		std::string& code = ast.GetTag();

		//This should be moved to the thing that the literal manager would be using to make literals
		get_switch(syn)
		{
			case SyntaxType::Boolean:
				if (code == "true") {
					literal.value = true;
					//result = true;
				}
				else if (code == "maybe") {
					literal.value = NumberConstant::Maybe;
				}
				else {
					literal.value = false;
					//result = true;
				}
				break;

			case SyntaxType::Integer:
				free_value = 1;
				[[fallthrough]];

			case SyntaxType::Number:
			{
				Number out;

				if (CreateNum(code, free_value, out) == false) {
					//send an exception
					ast.critical("invalid to be a number");
				}

				literal.value = out;
				//result = out;
			}
			break;

			case SyntaxType::Object:
				if constexpr (1)
				{
					literal.data->code = ast.GetTag();
					auto& front = ast.GetFront();
					ITypeInfo* declare = GetPolicyFromSpecifiers(ast, elem);
					//Declaration::CreateOnly(front, elem, Refness::Temp, HeaderFlag::TypeSpecifiers);
					
					if (!declare)
						front.error("typename {} not found", front.GetView());
					
					
					ObjectPolicy* policy = declare->GetObjectPolicy();
					
					if (!policy)
						front.error("typename {} isn't an object", front.GetView());

					bool success = policy->CreateLiteralData(ast.GetView(), literal._hash, literal.data->ctor);

					if (!success) {
						ast.error("typename {} cannot generate an object literal from {}", front.GetView(), ast.GetView());
					}

					//What if it's generic? I think accounting for these is a terrible idea, actually. Multiple inheritance exists, 
					// and while I think I'd only allow it to derive from one object type, I have difficulty thi

					solution = policy->GetTypeInfo();
					type = declare;

				}

				break;
			case SyntaxType::String:
				literal.value = code;

				//result = code;
				break;

			case SyntaxType::Constant:
				if constexpr (1)
				{
					switch (Hash(code))
					{
					case "none"_h:
					case "null"_h:
					case "default"_h:
						literal._info = common_type::null_t();
						break;

					case "undefined"_h:
						literal._info = common_type::voidable();
						break;
					default:
						ast.critical("Not a literal expression."); break;
					}
				}
				break;

			default:
				ast.critical("Not a literal expression."); break;
		}



		if (syn != SyntaxType::Object)
		{
			if (!literal._info)
			{
				assert(!literal.value.IsObject());
				solution = literal._info = literal.value.GetTypeInfo();
			}
			else
			{
				solution = literal._info;
			}
			
		}

		auto it = literalList.emplace(std::move(literal));



		//Trying to figure out how to get this out as a solution
		//std::make_pair(it.first->ptr(), type);
		Solution sol{ QualifiedType{ solution, Constness::Const }, OperandType::Literal, it.first->ptr() };
		
		return std::make_pair(sol, type);
		

		//If not an object, the literal type becomes the very same. If not, 

	}
	std::pair<Solution, ITypeInfo*>  LiteralManager::ObtainUndefined()
	{
		SyntaxRecord rec{ "undefined", SyntaxType::Constant };
		return ObtainLiteral(rec, nullptr);
	}

}