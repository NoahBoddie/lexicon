#pragma once

namespace LEX
{

	ENUM(OperatorType, uint8_t)
	{
		None = 0,
		Unary = 1 << 0,
		Binary = 1 << 1,
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

	



	INITIALIZE()
	{
		opSettings.emplace_back("=>", OperatorType::Binary, 1);
		opSettings.emplace_back("=", OperatorType::Binary, 2);
		opSettings.emplace_back("||", OperatorType::Binary, 3);
		opSettings.emplace_back("&&", OperatorType::Binary, 4);
		opSettings.emplace_back("<", OperatorType::Binary, 7);
		opSettings.emplace_back(">", OperatorType::Binary, 7);
		opSettings.emplace_back("<=", OperatorType::Binary, 7);
		opSettings.emplace_back(">=", OperatorType::Binary, 7);
		opSettings.emplace_back("==", OperatorType::Binary, 7);
		opSettings.emplace_back("!=", OperatorType::Binary, 7);
		opSettings.emplace_back("+", OperatorType::Binary, 10);
		opSettings.emplace_back("-", OperatorType::Both, 10);
		opSettings.emplace_back("*", OperatorType::Binary, 20);
		opSettings.emplace_back("/", OperatorType::Binary, 20);
		opSettings.emplace_back("%", OperatorType::Binary, 20);
		opSettings.emplace_back("^^", OperatorType::Binary, 22);
		opSettings.emplace_back("pow", OperatorType::Binary, 22);
		opSettings.emplace_back(".", OperatorType::Binary, 25);
		logger::critical("*%* opSettings set, size {}", opSettings.size());
	};




}