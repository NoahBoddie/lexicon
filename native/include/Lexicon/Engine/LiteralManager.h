#pragma once

#include "Lexicon/Variable.h"
#include "Lexicon/Engine/Literal.h"
#include "Lexicon/Engine/Expression.h"
#include "Lexicon/Engine/ExpressionType.h"

namespace LEX
{
	struct Literal;
	using Syntax = Impl::Syntax;
	using SyntaxType = Impl::SyntaxType;


	inline bool CreateNum(const std::string& value, bool is_int, Number& number) try
	{
		//TODO: CreateNum needs to be a bit more responsible. It cannot handle invalid arguments. Probably sorta important.

		
		if (is_int)
		{
			
			try {
				number = std::stoll(value);

				//int64_t value = std::stoll(code);
				//result = value;
			}
			catch (std::out_of_range sign_error) {
				number = std::stoull(value);
			}
		}
		else
		{
			if (value.back() != 'f'){
				double_anyways:
				number = std::stold(value);
			}
			else{
				
				try {
					number = std::stof(value);
				}
				catch (std::out_of_range sign_error) {
					report::warn("number value '{}' is too large to be a float. Making double instead.", value);
					goto double_anyways;
				}
			}
		}

		return true;
	}
	catch (std::out_of_range total_error)
	{
		return false;
	}

	//Move to constructor
	inline Variable CreateVariable(Record& ast)
	{
		Variable result;

		uint8_t free_value = 0;

		std::string& code = ast.GetTag();

		//This should be moved to the thing that the literal manager would be using to make literals
		get_switch(ast.SYNTAX().type)
		{
			case SyntaxType::Boolean:
				if (code == "true") {
					result = MakeVariable(true);
					//result = true;
				}
				else if (code == "maybe") {
					result = MakeVariable(NumberConstant::Maybe);
				}
				else {
					result = MakeVariable(false);
					//result = true;
				}
				break;

			case SyntaxType::Integer:
				free_value = 1;
				break;

			case SyntaxType::Number:
			{
				Number out;

				if (CreateNum(code, free_value, out) == false) {
					//send an exception
					report::compile::critical("invalid to be a number");
				}

				result = MakeVariable(out);
				//result = out;
			}
			break;

			case SyntaxType::Object:
			case SyntaxType::String:
				result = MakeVariable(code);
				//result = code;
				break;

			default:
				report::compile::critical("Not a literal expression."); break;
		}
		AbstractTypePolicy* policy = result.Policy();

		RGL_LOG(trace, "literal policy test {} -> {}", !!policy, policy ? (uint32_t)policy->GetTypeID() : 0);
		return result;
	}


	struct LiteralManager
	{
		//Whenever an object literal is used,
		//static std::unordered_map<size_t, Record*> resolveMap;

		//TODO: Ideally obtain literal, this returns const, will want to experiment with that later.
		static Literal ObtainLiteral(Record& ast);

		static Variable* GetLiteral(size_t index);
	

		static void FinalizeLiteral();

		
		static std::vector<Variable>& _LiteralList();

		inline static size_t size{ 5 };
		//The idea is to keep track of variables and a ref count. Just in case
		//inline static std::vector<Variable> _list{};

	};

}