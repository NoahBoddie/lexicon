#pragma once

#include "Variable.h"
#include "Literal.h"
#include "Expression.h"
#include "ExpressionType.h"

namespace LEX
{

	using Syntax = Impl::Syntax;
	using SyntaxType = Impl::SyntaxType;


	inline bool CreateNum(std::string value, bool is_int, Number& number) try
	{

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
				}

				result = out;
			}
			break;

			case SyntaxType::Object:
			case SyntaxType::String:
				result = code;
				break;

			default:
				RGL_LOG(critical, "Not a literal expression.");
				throw nullptr;
		}

		return result;
	}


	struct LiteralManager
	{


		//TODO: Ideally obtain literal, this returns const, will want to experiment with that later.
		static Literal ObtainLiteral(Record& ast)
		{
			//I have a concept within this of making hashes from the strings of records, and mutating the hash with it's children
			Variable entry = CreateVariable(ast);

			auto begin = _LiteralList().begin();
			auto end = _LiteralList().end();

			auto it = std::find_if(begin, end, [&](auto& arg) { return arg == entry; });
			
			size_t result = max_value<size_t>;

			if (it != end) {
				result = std::distance(begin, it);
			}
			else {
				result = _LiteralList().size();
				_LiteralList().emplace_back(entry);
				size = result;				
			}
			RGL_LOG(trace, "<%> hit {} , addr {}", _LiteralList().size(), (uintptr_t)&_LiteralList());
			//GetLiteral(result);
			return Literal{ result };
		}

		static Variable* GetLiteral(size_t index)
		{
			RGL_LOG(trace, "<%> get at {}, size {}, addr {}", index, _LiteralList().size(), (uintptr_t)&_LiteralList());

			return &_LiteralList()[index];
		}
	

		static void FinalizeLiteral()
		{
			//Because this goes off, things will need to use a literal key
			_LiteralList().shrink_to_fit();
		}
		inline static size_t size{5};
		//The idea is to keep track of variables and a ref count. Just in case
		//inline static std::vector<Variable> _list{};

		static std::vector<Variable>& _LiteralList();

	};

}