#pragma once

#include "Variable.h"
#include "Literal.h"
#include "Expression.h"
#include "ExpressionType.h"

namespace LEX
{
	struct Literal;
	using Syntax = Impl::Syntax;
	using SyntaxType = Impl::SyntaxType;


	inline bool CreateNum(std::string value, bool is_int, Number& number) try
	{

		if (is_int)
		{
			throw nullptr;//TODO: REMOVE THROW, SHIT MAKES IT A DOUBLE
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
			number = std::stold(value);
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
					result = true;
				}
				else {
					result = false;
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

				result = out;
			}
			break;

			case SyntaxType::Object:
			case SyntaxType::String:
				result = code;
				break;

			default:
				report::compile::critical("Not a literal expression."); break;
		}
		AbstractTypePolicy* policy = result.GetPolicy();

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