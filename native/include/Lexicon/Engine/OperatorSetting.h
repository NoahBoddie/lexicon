#pragma once

#include "Lexicon/Engine/Tokenizer.h"

namespace LEX
{

	ENUM(OperatorType, uint8_t)
	{
		None = 0,
		Unary = 1 << 0,
		PostUnary = 1 << 1,
		Binary = 1 << 2,
		Both,
	};

	struct OperatorSetting
	{
		using Priority = uint8_t;
	
		
		std::string_view opCode;
		OperatorType type;
		Priority precedence;
		//Instruction can go here.

		constexpr OperatorSetting(std::string_view op, OperatorType t, Priority p) noexcept : opCode{ op }, type{ t }, precedence{ p }
		{

		}


		constexpr OperatorSetting Unary(std::string_view op, Priority p)
		{
			return OperatorSetting(op, OperatorType::Unary, p);
		}

		constexpr OperatorSetting Binary(std::string_view op, Priority p)
		{
			return OperatorSetting(op, OperatorType::Binary, p);
		}


	
	};
	
	inline std::vector<OperatorSetting> opSettings{};


	inline int GetPrecedence(std::string_view op, OperatorType type)
	{
		for (auto& setting : opSettings)
		{
			if (setting.opCode == op && type & setting.type)
				return setting.precedence;
		}

		return 0;
	}

	inline bool IsOperator(std::string_view op)
	{
		return GetPrecedence(op, OperatorType::Both);
	}

	inline void AddOperator(const std::string_view& op, OperatorType type, uint8_t prec)
	{
		//scriptTokens.AddOperator(op);
		basicTokens.AddOperator(op);//Unsure how this should be handled. Some operators are important for preprocessing
		opSettings.emplace_back(op, type, prec);
	}



	INITIALIZE("main_init")
	{
		/*
		"=>",
		"+",
		"-",
		"*",
		"/",
		"%",
		"?",
		"=",
		"<",
		">",
		"!",
		"|",
		"||",
		"&",
		"&&",
		".",
		"^",
		"^^",
		"++",
		"--",
		"<<",
		">>",
		"==",
		"+=",
		"!=",
		">=",
		"<=",
		"|=",
		"&=",
		//*/
		int test = 1;
		auto test2 = +test;

		AddOperator("=>", OperatorType::Binary, 1);


		AddOperator("+=", OperatorType::Binary, 4);
		AddOperator("-=", OperatorType::Binary, 4);
		AddOperator("%=", OperatorType::Binary, 4);
		AddOperator("^^=", OperatorType::Binary, 4);
		AddOperator("^=", OperatorType::Binary, 4);
		AddOperator("*=", OperatorType::Binary, 4);
		AddOperator("|=", OperatorType::Binary, 4);
		AddOperator("&=", OperatorType::Binary, 4);
		AddOperator("<<=", OperatorType::Binary, 4);
		AddOperator(">>=", OperatorType::Binary, 4);



		AddOperator("=", OperatorType::Binary, 5);
		AddOperator("||", OperatorType::Binary, 6);
		AddOperator("&&", OperatorType::Binary, 7);
		

		AddOperator("|", OperatorType::Binary, 9);
		AddOperator("^", OperatorType::Binary, 10);
		AddOperator("&", OperatorType::Binary, 11);



		AddOperator("==", OperatorType::Binary, 12);
		AddOperator("!=", OperatorType::Binary, 12);


		AddOperator("<", OperatorType::Binary, 14);
		AddOperator(">", OperatorType::Binary, 14);
		AddOperator("<=", OperatorType::Binary, 14);
		AddOperator(">=", OperatorType::Binary, 15);
		

		AddOperator("<<", OperatorType::Binary, 16);
		AddOperator(">>", OperatorType::Binary, 16);


		AddOperator("+", OperatorType::Binary, 18);
		AddOperator("-", OperatorType::Both, 18);//TODO: Should this be at this level of precedence?

		AddOperator("*", OperatorType::Binary, 20);
		AddOperator("/", OperatorType::Binary, 20);
		AddOperator("%", OperatorType::Binary, 20);
		AddOperator("^^", OperatorType::Binary, 22);
		AddOperator("pow", OperatorType::Binary, 22);
		AddOperator("*", OperatorType::Unary, 23);


		AddOperator("++", OperatorType::Unary, 24);
		AddOperator("--", OperatorType::Unary, 24);
		AddOperator("+", OperatorType::Unary, 24);
		AddOperator("-", OperatorType::Unary, 24);
		AddOperator("!", OperatorType::Unary, 24);
		AddOperator("~", OperatorType::Unary, 24);


		AddOperator(".", OperatorType::Binary, 25);

		AddOperator("++", OperatorType::PostUnary, 26);
		AddOperator("--", OperatorType::PostUnary, 26);
	};




}