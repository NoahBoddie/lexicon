#include "Lexicon/Impl/ParseModule.h"
#include "Lexicon/Impl/Parser.h"

namespace LEX::Impl
{
	Record ParseModule::_ExecuteModule(ParseModule* mdl, Parser* parser, Record* target)
	{
		if (!mdl) {
			parser->GetInput()->croak("something about static handle token.");
		}

		Record result{};

		//Doing it like this deals with process chains.
		parser->_ExecuteModule(result, target, mdl);

		return result;

		//Might have something about process chains right here.
		//Use a different version plz.
		//return mdl->HandleToken(parser, target);
	}


	
	bool ParseModule::_QuestionModule(ParseModule* mdl, Parser* parser, Record* target, ParseFlag flag)
	{
		if (!mdl) {
			parser->GetInput()->croak("something about static handle token.");
		}

		return parser->_QueryModule(target, mdl, flag);
	}

	bool ParseModule::_QueryModule(ParseModule* mdl, Parser* parser, Record& out, Record* target)
	{
		if (!mdl) {
			parser->GetInput()->croak("something about static handle token.");
		}

		return parser->_TryModule(out, target, mdl, ParseFlag::Direct);
	}




	Record ParseModule::_TryModule(ParseModule* mdl, Parser* parser, Record* target)
	{

		if (!mdl) {
			parser->GetInput()->croak("something about static handle token.");
		}


		Record result{};

		//Try module will try to use use try module, and if it's unsuccessful, it will croak.
		// Basically a checked ParseAtomic for specific modules.
		if (_QueryModule(mdl, parser, result, target) == false)
			parser->GetInput()->croak("Module failed to parse");

		return result;
	}

	ParseModule* ParseModule::_GetBuiltModule(Parser* parser, const std::type_info& info)
	{
		return parser->GetBuiltModule(info);
	}



	uint32_t ParseModule::GetPriority() const 
	{ 
		return ModulePriority::Minimal; 
	}


	//Here I'll place a line of common context keywords that will be enabled by using one of these.
	ParseKeyword ParseModule::GetKeywords() 
	{ 
		return ParseKeyword::None; 
	}

	bool ParseModule::HasKeyword(std::string_view str)
	{
		switch (Hash(str))
		{
		case "Statement"_ih:
			return GetKeywords() & ParseKeyword::Statement;
		}

		return false;
	}
}